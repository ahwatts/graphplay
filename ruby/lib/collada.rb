# -*- encoding: utf-8 -*-

module StringExtensions
  refine String do
    def camelize
      split("_").map { |s| s.capitalize }.join  
    end

    def underscore
      scan(/[A-Z][a-z0-9]*/).map { |s| s.downcase }.join("_")
    end
  end
end

using StringExtensions

module Collada
  class Geometry
    attr_reader :id, :name

    def self.from_node(node)
      child = node.xpath("*").first
      klass = Collada.const_get("#{child.name.camelize}Geometry")
      if klass.nil?
        raise "Could not find class for #{child.name}"
      end
      klass.new(node)
    end

    def initialize(node)
      @id = node["id"]
      @name = node["name"]
      @objects_by_id = {}
    end

    def resolve_input_to_sources(input)
      if input.respond_to?(:source)
        source = @objects_by_id[input.source.gsub(/^#/, "")]
        semantic = input.semantic

        if source.respond_to?(:data)
          { semantic => source }
        elsif source.respond_to?(:inputs)
          {}.tap do |rv|
            source.inputs.each do |si|
              sub_sources = resolve_input_to_sources(si)
              sub_sources.each do |sub_sem, ss|
                rv["#{semantic}_#{sub_sem}"] = ss
              end
            end
          end
        end
      end
    end

    def print_structure
      raise "Not implemented!"
    end
  end

  class MeshGeometry < Geometry
    attr_reader :sources, :vertices, :primitives

    def initialize(node)
      super(node)
      mesh_node = node.xpath("xmlns:mesh")

      @sources = mesh_node.xpath("xmlns:source").map do |n|
        Source.new(n).tap { |s| @objects_by_id[s.id] = s }
      end

      @vertices = Vertices.new(mesh_node.xpath("xmlns:vertices").first)
      @objects_by_id[@vertices.id] = @vertices

      @primitives = []
      mesh_node.children.each do |child_node|
        next if child_node.text? || %w{ source vertices }.include?(child_node.name)
        primitive_klass = Collada.const_get("#{child_node.name.camelize}Primitive")
        @primitives << primitive_klass.new(child_node)
      end
    end

    def print_structure
      @primitives.each do |p|
        if !p.vcount.all? { |v| v == 3 }
          raise "Don't know what to do with non-triangles!"
        end

        puts "Number of primitives: #{p.count}"
        puts "Vertices per primitive: 3"
        puts

        p.inputs.sort { |i1, i2| i1.offset <=> i2.offset }.each do |input|
          sources = resolve_input_to_sources(input)
          if sources.size > 1
            raise "Not sure how to handle more than 1 source!"
          else
            semantic, source = *sources.first
            puts "Attribute offset: #{input.offset}"
            puts "Attribute: #{semantic}"
            puts "Data: (count #{source.data.size})"
            source.data.each_with_index do |d, i|
              puts "%4d: %p" % [ i, d.to_a ]
            end
            puts
          end
        end

        puts "Element indices: #{p.elements.inspect}"
      end
    end

    def packed_string
      contents = [ "AHWGMESH", @primitives.size ]
      format = "A8L"
      
      @primitives.each do |p|
        contents += [ p.count, 3, p.inputs.size ]
        format << "LLL"

        p.inputs.sort { |i1, i2| i1.offset <=> i2.offset }.each do |input|
          sources = resolve_input_to_sources(input)
          if sources.size > 1
            raise "Not sure how to handle more than 1 source!"
          else
            semantic, source = *sources.first
            sem_length = semantic.bytesize
            sem_length = (sem_length.to_f / 4.0).ceil * 4
            contents << sem_length
            contents << semantic
            format << "LA#{sem_length}"

            contents << input.offset
            format << "L"

            contents << source.data.size
            format << "L"

            contents << source.data.first.to_a.size
            format << "L"

            contents += source.data.map(&:to_a).flatten
            format << "F#{source.data.size*source.data.first.to_a.size}"
          end
        end

        contents << p.elements.size
        format << "L"

        contents += p.elements
        format << "L#{p.elements.size}"
      end
      
      contents.pack(format)
    end
  end

  class XyzData
    attr_accessor :x, :y, :z

    def initialize(vals)
      self.x = vals["X"]
      self.y = vals["Y"]
      self.z = vals["Z"]
    end
    
    def to_a
      [ x, y, z ]
    end
  end

  class StData
    attr_accessor :s, :t

    def initialize(vals)
      self.s = vals["S"]
      self.t = vals["T"]
    end
    
    def to_a
      [ s, t ]
    end
  end

  class RgbData
    attr_accessor :r, :g, :b

    def initialize(vals)
      self.r = vals["R"]
      self.g = vals["G"]
      self.b = vals["B"]
    end
    
    def to_a
      [ r, g, b ]
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

      data_klass = nil
      accessor_names = accessor.params.map { |p| p[:name] }.compact
      if accessor_names.size == 3 && %w{ X Y Z }.all? { |n| accessor_names.include?(n) }
        data_klass = XyzData
      elsif accessor_names.size == 2 && %w{ S T }.all? { |n| accessor_names.include?(n) }
        data_klass = StData
      elsif accessor_names.size == 3 && %w{ R G B }.all? { |n| accessor_names.include?(n) }
        data_klass = RgbData
      end

      raise "Could not find data class for accessor #{accessor_names.inspect}" if data_klass.nil?

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
        @data << data_klass.new(v)
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
        SharedInput.new(n)
      end
    end
  end

  class PolylistPrimitive
    attr_reader :name, :count, :inputs, :vcount, :elements

    def initialize(node)
      @name = node["name"]
      @count = node["count"].to_i
      @inputs = node.xpath("xmlns:input").map { |n| SharedInput.new(n) }
      @vcount = node.xpath("xmlns:vcount").text.split(" ").map(&:to_i)
      @elements = node.xpath("xmlns:p").text.split(" ").map(&:to_i)
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
