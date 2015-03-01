#!/usr/bin/env ruby

# Add the lib dir to the load path.
$: << File.expand_path("../../lib", __FILE__)

require "ply"
require "vector"
require "zlib"
require "pp"

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

# Link vertices to faces.
puts "Linking vertices to faces..."
doc.elements_by_name["vertex"].add_list_property("face_indices", :uint8, :int32)
doc.elements_by_name["face"].data.each_with_index do |face, i|
  doc.elements_by_name["vertex"].data.values_at(*face["vertex_indices"]).each do |v|
    v["face_indices"] ||= []
    v["face_indices"] << i
  end
end

# Add normals to each face.
puts "Adding normals to faces..."
doc.elements_by_name["face"].add_property("nx", :float32)
doc.elements_by_name["face"].add_property("ny", :float32)
doc.elements_by_name["face"].add_property("nz", :float32)
doc.elements_by_name["face"].data.each do |face|
  p1, p2, p3 = doc.elements_by_name["vertex"].data.values_at(*face["vertex_indices"])
  v1, v2, v3 = [ p1, p2, p3 ].map { |p| Vector.new(p) }
  norm = (v2 - v1).cross(v3 - v1).normalize

  face["nx"] = norm.x
  face["ny"] = norm.y
  face["nz"] = norm.z
end

# Print out a random member of each element, as a sanity check.
doc.elements.each do |elem|
  i = rand(elem.count)
  puts "#{elem.name}[#{i}] = #{elem.data[i].inspect}"
end
