$: << File.expand_path('..', __FILE__)

module Faster
end

require 'faster/version'

require 'faster_mri'

class Faster::Array::Int

  # TODO in C
  def eql?(other)
    return false unless other.is_a?(self.class) && length == other.length

    (0...length).each do |index|
      return false unless at(index) == other.at(index)
    end
  end

  # TODO in C
  def ==(other)
    return false unless length == other.length

    (0...length).each do |index|
      return false unless at(index) == other.at(index)
    end
  end

end