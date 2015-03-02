#!/usr/bin/env ruby

# Add the lib dir to the load path.
$: << File.expand_path("../../lib", __FILE__)

require "ply"
require "set"
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

vertices = doc.elements_by_name["vertex"].data
faces = doc.elements_by_name["face"].data

vertices.each_with_index do |vertex, vertex_index|
  vertex["index"] = vertex_index
end

faces.each_with_index do |face, face_index|
  fvs = vertices.values_at(*face["vertex_indices"])
  fvs.each do |v|
    v["face_indices"] ||= []
    v["face_indices"] << face_index
  end
end

checked = Set.new
to_check = []
to_check.push(0)

while (face_index = to_check.pop)
  if !checked.include?(face_index)
    face = faces[face_index]
    v0, v1, v2 = vertices.values_at(*face["vertex_indices"])
    any_bad = false

    [ [ v0, v1 ], [ v1, v2 ], [ v2, v0 ] ].each do |e0, e1|
      i1, i2 = e0["index"], e1["index"]

      neighbor_face_index = (e0["face_indices"] & e1["face_indices"]) - [ face_index ]
      raise "Multiple neighbor faces?!" if neighbor_face_index.size > 1
      next if neighbor_face_index.size == 0

      neighbor_face_index = neighbor_face_index.first
      neighbor_face = faces[neighbor_face_index]

      ni1, ni2, ni3 = neighbor_face["vertex_indices"]
      if [ [ ni2, ni1 ], [ ni3, ni2 ], [ ni1, ni3 ] ].include?([ i1, i2 ]) &&
          ![ [ ni1, ni2 ], [ ni2, ni3 ], [ ni3, ni1 ] ].include?([ i1, i2 ])
        to_check.push(neighbor_face_index)
      else
        puts "face_index = #{face_index}"
        puts "vertex indices = #{[ i1, i2 ].inspect}"
        puts "neighbor face = index #{neighbor_face_index}: #{neighbor_face.inspect}"
        any_bad = true
      end
    end

    if any_bad
      raise "Bad face: #{face_index}"
    else
      checked << face_index
    end
  end
end
