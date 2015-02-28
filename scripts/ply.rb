#!/usr/bin/env ruby

require "zlib"

module Ply
  class Element
    attr_accessor :name, :count, :properties, :properties_by_name, :data

    def initialize(name, count)
      self.name = name
      self.count = count.to_i
    end

    def inspect
      props_str = properties.map do |p|
        if p[:count_type]
          "%p => list of %p, count is %p" % [ p[:name], p[:type], p[:count_type] ]
        else
          "%p => %p" % [ p[:name], p[:type] ]
        end
      end.join(", ")

      "Ply::Element name = %p count = %p properties = { %s }" %
        [ name, count, props_str ]
    end
    alias :to_s :inspect

    def add_property(name, type)
      self.properties ||= []
      self.properties_by_name ||= {}

      self.properties << { name: name, type: type.to_sym }
      self.properties_by_name[name] = { name: name, type: type.to_sym }
    end

    def add_list_property(name, count_type, elem_type)
      self.properties ||= []
      self.properties_by_name ||= {}

      self.properties << { name: name, count_type: count_type.to_sym, type: elem_type.to_sym }
      self.properties_by_name[name] = { name: name, count_type: count_type.to_sym, type: elem_type.to_sym }
    end
  end

  class Document
    attr_accessor :format, :comments, :elements, :elements_by_name

    def initialize(file = nil)
      unless file.nil?
        load!(file)
      end
    end

    def clear!
      self.format = nil
      self.comments = []
      self.elements = []
      self.elements_by_name = {}
      @current_element = nil
    end

    def load!(file)
      magic = file.read(4)
      if magic != "ply\n"
        raise "#{file.path} does not appear to be a PLY file: #{magic.inspect}"
      end

      clear!

      file.each_line do |line|
        break unless handle_line(line.chomp)
      end

      if format == :ascii
        elements.each do |element|
          puts "Reading element: #{element.inspect}"
          read_ascii_element(file, element)
        end
      else
        elements.each do |element|
          puts "Reading element: #{element.inspect}"
          read_binary_element(file, element)
        end
      end
    end

    protected

    def add_element(element)
      self.elements ||= []
      self.elements_by_name ||= {}

      self.elements << element
      self.elements_by_name[element.name] = element
    end

    def handle_line(line)
      case line
      when /^format (\w+) \d\.\d/
        if %w{ ascii binary_big_endian binary_little_endian }.include?($1)
          self.format = $1.to_sym
        else
          raise "Unknown format: #{$1.inspect}"
        end
        true
      when /^comment (.*)$/
        self.comments ||= []
        self.comments << $1
        true
      when /^element (\w+) (\d+)$/
        if @current_element
          add_element(@current_element)
        end
        @current_element = Element.new($1, $2.to_i)
        true
      when /^property list (\w+) (\w+) (\w+)$/
        @current_element.add_list_property($3, $1, $2)
        true
      when /^property (\w+) (\w+)$/
        @current_element.add_property($2, $1)
        true
      when "end_header"
        if @current_element
          add_element(@current_element)
        end
        @current_element = nil
        false
      else
        true
      end
    end

    def convert_ascii_value(string, type)
      case type
      when :char, :int8, :uchar, :uint8, :short, :int16, :ushort, :uint16, :int, :int32, :uint, :uint32
        string.to_i
      when :float, :float32, :double, :float64
        string.to_f
      end
    end

    def convert_binary_value(file, type)
      endian = { binary_big_endian: ">", binary_little_endian: "<" }[format]

      case type
      when :char, :int8
        file.read(1).unpack("c").first
      when :uchar, :uint8
        file.read(1).unpack("C").first
      when :short, :int16
        file.read(2).unpack("s#{endian}").first
      when :ushort, :uint16
        file.read(2).unpack("S#{endian}").first
      when :int, :int32
        file.read(4).unpack("l#{endian}").first
      when :uint, :uint32
        file.read(4).unpack("L#{endian}").first
      when :float, :float32
        d = { binary_big_endian: "g", binary_little_endian: "e" }[format]
        file.read(4).unpack(d).first
      when :double, :float64
        d = { binary_big_endian: "G", binary_little_endian: "E" }[format]
        file.read(8).unpack(d).first
      end
    end

    def read_ascii_element(file, element)
      read = 0
      element.data = []

      file.each_line do |line|
        vals = line.chomp.split(" ")
        whole_val = {}

        element.properties.each do |prop|
          if prop[:count_type]
            count = convert_ascii_value(vals.shift, prop[:count_type])
            whole_val[prop[:name]] = []

            count.times do
              whole_val[prop[:name]] << convert_ascii_value(vals.shift, prop[:type])
            end
          else
            whole_val[prop[:name]] = convert_ascii_value(vals.shift, prop[:type])
          end
        end

        element.data << whole_val
        read += 1
        break if read >= element.count
      end
    end

    def read_binary_element(file, element)
      read = 0
      element.data = []

      while !file.eof?
        whole_val = {}

        element.properties.each do |prop|
          if prop[:count_type]
            count = convert_binary_value(file, prop[:count_type])
            whole_val[prop[:name]] = []
            count.times do
              whole_val[prop[:name]] << convert_binary_value(file, prop[:type])
            end
          else
            whole_val[prop[:name]] = convert_binary_value(file, prop[:type])
          end
        end

        element.data << whole_val
        read += 1
        break if read >= element.count
      end
    end

  end
end

ARGV.each do |filename|
  file_klass = File
  open_opts = [ filename, "rb" ]

  if filename =~ /\.gz$/
    file_klass = Zlib::GzipReader
    open_opts = [ filename ]
  end

  file_klass.open(*open_opts) do |f|
    doc = Ply::Document.new(f)

    doc.elements.each do |elem|
      i = rand(elem.count)
      puts "#{elem.name}[#{i}] = #{elem.data[i].inspect}"
    end
  end
end
