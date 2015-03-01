#!/usr/bin/env ruby

# Add the lib dir to the load path.
$: << File.expand_path("../../lib", __FILE__)

require "rubygems"
require "nokogiri"
require "collada"

doc = Nokogiri::XML(ARGF)
geometries = doc.xpath("//xmlns:geometry").map { |n| Collada::Geometry.from_node(n) }
geometries.each do |g|
  g.print_structure
  # File.open("#{g.name}.mesh", "wb") { |f| f.write(g.packed_string) }
end
