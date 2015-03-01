# -*- encoding: utf-8; -*-

class Vector
  attr_accessor :comps

  def initialize(*args)
    if Array === args && args.size == 1 && !(Numeric === args.first)
      args = args.first
    end

    if Hash === args &&
        args.values_at(:x, :y, :z, :w, :r, :g, :b, :a, "x", "y", "z", "w", "r", "g", "b", "a").compact.all? { |v| Numeric === v }
      self.comps = [
        args[:x] || args["x"] || args[:r] || args["r"],
        args[:y] || args["y"] || args[:g] || args["g"],
        args[:z] || args["z"] || args[:b] || args["b"],
        args[:w] || args["w"] || args[:a] || args["a"],
      ].compact
    elsif Array === args && args.all? { |v| Numeric === v }
      self.comps = args
    else
      raise ArgumentError, "Cannot construct Vector from #{args.inspect}"
    end
  end

  def x
    comps[0]
  end
  alias_method(:r, :x)

  def x=(new_x)
    comps[0] = new_x
  end
  alias_method(:r=, :x=)

  def y
    comps[1]
  end
  alias_method(:g, :y)

  def y=(new_y)
    comps[1] = new_y
  end
  alias_method(:g=, :y=)

  def z
    comps[2]
  end
  alias_method(:b, :z)

  def z=(new_z)
    comps[3] = new_z
  end
  alias_method(:b=, :z=)

  def w
    comps[3]
  end
  alias_method(:a, :w)

  def w=(new_w)
    comps[3] = new_w
  end
  alias_method(:a=, :w=)

  def +(other)
    unless Vector === other
      raise ArgumentError, "Cannot add Vector to #{other.class}"
    end

    unless comps.size == other.comps.size
      raise ArgumentError, "Cannot add Vectors of two different sizes (lhs: #{comps.size} rhs: #{other.comps.size})" 
    end

    Vector.new(*comps.zip(other.comps).map { |c, oc| c + oc })
  end

  def *(val)
    if Numeric === val
      Vector.new(*comps.map { |c| val * c })
    elsif Vector === val
      if comps.size != val.comps.size
        raise ArgumentError, "Cannot dot Vectors of two different sizes (lhs: #{comps.size} rhs: #{val.comps.size})" 
      end

      comps.zip(val.comps)
        .map { |c, oc| c * oc }
        .inject(0) { |m, v| m += v }
    else
      raise ArgumentError, "Cannot multiply Vector and #{val.class}"
    end
  end

  def -(other)
    self + (other * -1)
  end

  def /(val)
    unless Numeric === val
      raise ArgumentError, "Cannot divide Vector by #{val.class}"
    end

    Vector.new(*comps.map { |c| c / val })
  end

  def cross(other)
    unless Vector === other
      raise ArgumentError, "Cannot cross Vector with #{other.class}"
    end

    unless comps.size == 3 && other.comps.size == 3
      raise ArgumentError, "Cannot cross non-3d vectors (lhs: #{comps.size} rhs: #{comps.size})"
    end

    Vector.new(
      y*other.z - z*other.y,
      x*other.z - z*other.x,
      x*other.y - y*other.z)
  end

  def magnitude
    Math.sqrt(self * self)
  end

  def normalize
    self / magnitude
  end
end
