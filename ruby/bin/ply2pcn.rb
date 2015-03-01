#!/usr/bin/env ruby

# Add the lib dir to the load path.
$: << File.expand_path("../../lib", __FILE__)

require "ply"
require "vector"
require "zlib"

filename = ARGV.shift

# Assume the file is initially a raw file.
file_klass = File
open_opts = [ filename, "rb" ]

# But handle gzipped files, too.
if filename =~ /\.gz$/
  file_klass = Zlib::GzipReader
  open_opts = [ filename ]
end

# Load it up.
puts "Reading #{filename}..."
doc = file_klass.open(*open_opts) { |f| Ply::Document.new(f) }

# Print out a random member of each element, as a sanity check.
doc.elements.each do |elem|
  i = rand(elem.count)
  puts "#{elem.name}[#{i}] = #{elem.data[i].inspect}"
end

doc.elements_by_name["face"]
