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

# Link vertices to faces, and add normals to each face.
puts "Linking vertices to faces, and adding normals to faces..."
doc.elements_by_name["vertex"].add_list_property("face_indices", :uint8, :int32)
doc.elements_by_name["face"].add_property("nx", :float32)
doc.elements_by_name["face"].add_property("ny", :float32)
doc.elements_by_name["face"].add_property("nz", :float32)

doc.elements_by_name["face"].data.each_with_index do |face, i|
  p1, p2, p3 = doc.elements_by_name["vertex"].data.values_at(*face["vertex_indices"])
  v1, v2, v3 = [ p1, p2, p3 ].map { |p| Vector.new(p) }
  norm = (v2 - v1).cross(v3 - v1).normalize

  face["nx"] = norm.x
  face["ny"] = norm.y
  face["nz"] = norm.z

  [ p1, p2, p3 ].each do |v|
    v["face_indices"] ||= []
    v["face_indices"] << i
  end
end

# Print out a random member of each element, as a sanity check.
doc.elements.each do |elem|
  i = rand(elem.count)
  puts "#{elem.name}[#{i}] = #{elem.data[i].inspect}"
end

puts "Computing vertex normals..."
doc.elements_by_name["vertex"].add_property("nx", :float32)
doc.elements_by_name["vertex"].add_property("ny", :float32)
doc.elements_by_name["vertex"].add_property("nz", :float32)

doc.elements_by_name["vertex"].data.each do |vertex|
  faces = doc.elements_by_name["face"].data.values_at(*vertex["face_indices"])
  face_normals = faces.map { |f| Vector.new(f["nx"], f["ny"], f["nz"]) }
  vertex_normal = face_normals
    .inject(Vector.zero(3)) { |m, fn| m = m + fn }
    .normalize
  vertex["nx"] = vertex_normal.x
  vertex["ny"] = vertex_normal.y
  vertex["nz"] = vertex_normal.z
end

# Print out a random member of each element, as a sanity check.
doc.elements.each do |elem|
  i = rand(elem.count)
  puts "#{elem.name}[#{i}] = #{elem.data[i].inspect}"
end

bb_max = Vector.new(doc.elements_by_name["vertex"].data.first)
bb_min = Vector.new(doc.elements_by_name["vertex"].data.first)

doc.elements_by_name["vertex"].data.each do |v|
  vv = Vector.new(v["x"], v["y"], v["z"])
  bb_max = Vector.new(
    [ bb_max.x, vv.x ].max,
    [ bb_max.y, vv.y ].max,
    [ bb_max.z, vv.z ].max)
  bb_min = Vector.new(
    [ bb_min.x, vv.x ].min,
    [ bb_min.y, vv.y ].min,
    [ bb_min.z, vv.z ].min)
end

centroid = (bb_max + bb_min) / 2.0

puts "centroid = #{PP.pp(centroid, '')}"
puts "bounding-box minimum = #{PP.pp(bb_min, '')}"
puts "bounding-box maximum = #{PP.pp(bb_max, '')}"

basename = File.basename(filename).match(/^(.*)\.ply(?:\.gz)?$/).captures.first
pcn_filename = File.join(File.dirname(filename), "#{basename}.pcn")

File.open(pcn_filename, "wb") do |f|
  f.write("pcn\0")
  f.write([ doc.elements_by_name["vertex"].count ].pack("L"))
  doc.elements_by_name["vertex"].data.each do |v|
    vp = Vector.new(v["x"], v["y"], v["z"])
    vc = Vector.new(1.0, 0.0, 1.0, 1.0)
    vn = Vector.new(v["nx"], v["ny"], v["nz"])
    f.write(vp.comps.pack("f3"))
    f.write(vc.comps.pack("f4"))
    f.write(vn.comps.pack("f3"))
  end

  f.write([ doc.elements_by_name["face"].count * 3 ].pack("L"))
  doc.elements_by_name["face"].data.each do |face|
    f.write(face["vertex_indices"].pack("L3"))
  end
end
