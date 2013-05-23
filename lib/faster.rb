$: << File.expand_path('..', __FILE__)

module Faster
end

require 'faster/version'

require 'faster_mri'

class Faster::Array::Int

  # TODO in C
  def eql?(other)
    return true if equal?(other)
    return false unless other.is_a?(self.class) && length == other.length

    (0...length).each do |index|
      return false unless at(index) == other.at(index)
    end
  end

  # TODO in C
  def ==(other)
    return true if eql?(other)
    return false unless length == other.length

    (0...length).each do |index|
      return false unless at(index) == other.at(index)
    end
  end

end

module Faster::Map; end
module Faster::Map::FlatSorted; end

class Faster::Map::FlatSorted::IntObject

  def initialize(items = nil)
    @keys = Faster::Array::Int.new([])
    @values = []

    if items
      items.each do |k, v|
        self[k] = v
      end
    end
  end

  def [](key)
    index = insert_index(key)
    @keys[index] == key ? @values[index] : nil
  end

  def key?(key)
    index = insert_index(key)
    @keys[index] == key
  end

  def []=(key, value)
    index = insert_index(key)

    if index == @keys.size
      @keys = @keys.dup if @keys.frozen?
      @values = @values.dup if @values.frozen?

      @keys << key
      @values << value
    else
      index_key = @keys[index]

      if index_key.equal?(key)     # replace
        unless @values[index].equal?(value)
          @values = @values.dup if @values.frozen?
          @values[index] = value
        end
      else                    # insert
        @keys = @keys.dup if @keys.frozen?
        @values = @values.dup if @values.frozen?

        @keys.insert(index, key)
        @values.insert(index, value)
      end
    end
  end

  def delete(key)
    index = insert_index(key)

    if index < @keys.size && @keys[index] == key
      @keys = @keys.dup if @keys.frozen?
      @values = @values.dup if @values.frozen?

      @keys.delete_at(index)
      @values.delete_at(index)    # returns deleted value
    else
      nil
    end
  end

  def empty?
    @keys.empty?
  end

  def size
    @keys.size
  end

  alias :length :size

  def freeze
    @keys.freeze
    @values.freeze
    super
  end

  def each
    raise "enumerators not supported" unless block_given?

    0.upto(@keys.length - 1) do |i|
      yield @keys.at(i), @values.at(i)
    end
  end

  def bound(start_key, end_key)
    raise "enumerators not supported" unless block_given?

    start_index = insert_index(start_key)
    return if start_index > @keys.size

    start_index.upto(@keys.size - 1) do |index|
      key = @keys[index]
      break if key > end_key

      yield key, @values[index]
    end

    self
  end

  def bound_exclusive(start_key, end_key)
    raise "enumerators not supported" unless block_given?

    start_index = insert_index(start_key)
    return if start_index > @keys.size

    start_index.upto(@keys.size - 1) do |index|
      key = @keys[index]
      break if key >= end_key

      yield key, @values[index]
    end

    self
  end

  def eql?(other)
    if equal?(other)
      true
    elsif other.is_a?(self.class)
      @keys == other.instance_variable_get(:@keys) &&
      @values == other.instance_variable_get(:@values)
    else
      false
    end
  end

  def ==(other)
    return true if eql?(other)

    case other
    when Hash
      other.size == size && other.all? { |k, v| self[k] == v }
    else
      false
    end
  end

  EMPTY = new.freeze

  private

  def insert_index(key)
    @keys.binary_search_ge(key)
  end

end