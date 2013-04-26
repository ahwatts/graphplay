#!/usr/bin/env ruby
# -*- encoding: utf-8 -*-

require 'rubygems'
require 'nokogiri'
require 'pp'

def camelize(string)
  string.split("_").map { |s| s.capitalize }.join  
end

def underscore(string)
  string.scan(/[A-Z][a-z0-9]*/).map { |s| s.downcase }.join("_")
end

module Collada
  class Geometry
    attr_reader :id, :name, :sources, :vertices, :primitives

    def self.from_node(node)
      child = node.xpath("*").first
      klass = Collada.const_get("#{camelize(child.name)}Geometry")
      klass.new(node)
    end

    def initialize(node)
      @id = node["id"]
      @name = node["name"]
      @sources = node.xpath("xmlns:mesh/xmlns:source").map { |n| Source.new(n) }
      @vertices = Vertices.new(node.xpath("xmlns:mesh/xmlns:vertices").first)
    end
  end

  class MeshGeometry < Geometry
    def initialize(node)
      super(node)
    end
  end

  class Source
    attr_reader :id, :name, :data

    def initialize(node)
      @id = node["id"]
      @name = node["name"]
      @data = []

      # Use the accessor to convert the data in to something
      # semantically meaningful.
      accessor = Accessor.new(node.xpath("xmlns:technique_common/xmlns:accessor").first)
      array_node = node.css(accessor.source).first

      convert_method = case array_node.name
                       when "float_array" then :to_f
                       else nil
                       end

      data_array = array_node.text.split(" ").map { |v| v.send(convert_method) }

      index = accessor.offset
      (0...accessor.count).each do |pass|
        v = {}
        vals = data_array[index, accessor.stride]
        accessor.params.each_with_index do |p, i|
          if p[:name]
            v[p[:name]] = vals[i]
          end
        end
        @data << v
        index += accessor.stride
      end
    end
  end

  class Accessor
    attr_reader :source, :count, :offset, :stride, :params

    def initialize(node)
      @source = node["source"]
      @count = node["count"].to_i
      @offset = node["offset"].to_i
      @stride = (node["stride"] || 1).to_i
      @params = node.xpath("xmlns:param").map { |n| { type: n["type"], name: n["name"] } }
    end
  end

  class Vertices
    attr_reader :id, :name, :inputs

    def initialize(node)
      @id = node["id"]
      @name = node["name"]
      @inputs = node.xpath("xmlns:input")

      @inputs = @inputs.map do |n|
        usi = UnSharedInput.new(n)
        { usi.semantic => usi.source }
      end.reduce(:merge)
    end
  end

  class PolyList
    attr_reader :name, :count, :inputs, :vcount, :elements

    def initialize(node)
      @name = node["name"]
      @count = node["count"].to_i
      @inputs = node.xpath("xmlns:input").map { |n| SharedInput.new(n) }
      @vcount = node.xpath("xmlns:vcount").text.split(" ").map(&:to_i)
    end
  end

  class UnSharedInput
    attr_reader :semantic, :source

    def initialize(node)
      @semantic = node["semantic"]
      @source = node["source"]
    end
  end

  class SharedInput
    attr_reader :semantic, :source, :offset, :set

    def initialize(node)
      @semantic = node["semantic"]
      @source = node["source"]
      @offset = node["offset"].to_i
      @set = node["set"].to_i
    end
  end
end

doc = Nokogiri::XML(ARGF)
geometries = doc.xpath("//xmlns:geometry").map { |n| Collada::Geometry.from_node(n) }
pp geometries

# def polylist(node)
#   rv = { name: node["name"], count: node["count"].to_i }
#   rv[:inputs] = node.xpath("xmlns:input").map { |n| input_shared(n) }
#   rv[:vcount] = node.xpath("xmlns:vcount").text.split(" ").map(&:to_i)
#   rv[:elements] = node.xpath("xmlns:p").text.split(" ").map(&:to_i)
#   rv
# end

# def input_shared(node)
#   { semantic: node["semantic"], source: node["source"],
#     set: node["set"].to_i, offset: node["offset"].to_i }
# end

# def setup_buffers(geometry)
  
# end
