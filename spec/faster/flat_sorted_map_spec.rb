require 'spec_helper'

describe Faster::Map::FlatSorted::IntObject do

  let(:items) { { 123 => 123, 456 => 'howdy', 1366273875 => 123 } }

  let(:hash) do
    h = Faster::Map::FlatSorted::IntObject.new(items)
    h[150] = 46
    h.delete(150)
    h
  end

  describe "construction + accessors" do
    subject { hash }

    its(:size) { should == items.size }
    its(:length) { should == items.length }
    it { should_not be_empty }
  end

  describe "#[]" do
    context "known items" do
      it "should return all items" do
        items.each do |k, v|
          hash[k].should == v
        end
      end
    end

    context "unknown item" do
      it "should return nil" do
        hash[0].should be_nil
      end
    end
  end

  describe "#key?" do
    context "known items" do
      it "should return all items" do
        items.each do |k, v|
          hash.key?(k).should be_true
        end
      end
    end

    context "unknown item" do
      it "should return nil" do
        hash.key?(0).should be_false
      end
    end
  end

  describe "#delete" do
    context "known items" do
      it "should return all items" do
        items.each do |k, v|
          hash.delete(k).should == v
        end

        hash.should be_empty
      end
    end

    context "unknown item" do
      it "should return nil" do
        hash.delete(0).should be_nil
      end
    end
  end

  describe "#each" do
    subject { result = []; hash.each { |k, v| result << [k, v] }; result }
    it { should == items.to_a }
  end

  describe "#bound" do
    subject { result = []; hash.bound(start_time, end_time) { |k, v| result << [k, v] }; result }
    let(:expected) { items.to_a.select { |(k, v)| k >= start_time && k <= end_time } }

    context "all time items" do
      let(:start_time) { 0 }
      let(:end_time) { 1000 }

      it { should == expected }
      its(:length) { should == 2 }
    end

    context "all inclusive end, inclusive start" do
      let(:start_time) { 123 }
      let(:end_time) { 456 }

      it { should == expected }
      its(:length) { should == 2 }
    end

    context "nothing" do
      let(:start_time) { 1000 }
      let(:end_time) { 2000 }

      it { should == expected }
      its(:length) { should == 0 }
    end
  end

  describe "#bound_exlusive" do
    subject { result = []; hash.bound_exclusive(start_time, end_time) { |k, v| result << [k, v] }; result }
    let(:expected) { items.to_a.select { |(k, v)| k >= start_time && k < end_time } }

    context "all time items" do
      let(:start_time) { 0 }
      let(:end_time) { 1000 }

      it { should == expected }
      its(:length) { should == 2 }
    end

    context "all inclusive end, inclusive start" do
      let(:start_time) { 123 }
      let(:end_time) { 456 }

      it { should == expected }
      its(:length) { should == 1 }
    end

    context "nothing" do
      let(:start_time) { 1000 }
      let(:end_time) { 2000 }

      it { should == expected }
      its(:length) { should == 0 }
    end
  end

  describe "#==" do
    subject { hash == other }

    context "with a Hash" do
      context "that's similar" do
        let(:other) { items }
        it { should be_true }
      end

      context "that's dissimilar" do
        let(:other) { items.merge(:x => :y) }
        it { should be_false }
      end

      context "that's empty" do
        let(:other) { {} }
        it { should be_false }
      end
    end

    context "with another of the same class" do
      context "that's similar" do
        let(:other) { Faster::Map::FlatSorted::IntObject.new(items) }
        it { should be_true }
      end

      context "that's dissimilar" do
        let(:other) { Faster::Map::FlatSorted::IntObject.new(items.merge(1000 => :y)) }
        it { should be_false }
      end

      context "that's empty" do
        let(:other) { Faster::Map::FlatSorted::IntObject.new }
        it { should be_false }
      end
    end
  end

  describe "#eql?" do
    subject { hash.eql? other }

    context "with a Hash" do
      context "that's similar" do
        let(:other) { items }
        it { should be_false }
      end

      context "that's dissimilar" do
        let(:other) { items.merge(:x => :y) }
        it { should be_false }
      end

      context "that's empty" do
        let(:other) { {} }
        it { should be_false }
      end
    end

    context "with another of the same class" do
      context "that's similar" do
        let(:other) { Faster::Map::FlatSorted::IntObject.new(items) }
        it { should be_true }
      end

      context "that's dissimilar" do
        let(:other) { Faster::Map::FlatSorted::IntObject.new(items.merge(1000 => :y)) }
        it { should be_false }
      end

      context "that's empty" do
        let(:other) { Faster::Map::FlatSorted::IntObject.new }
        it { should be_false }
      end
    end
  end

  context "empty" do
    let(:items) { {} }
    subject { hash }

    its(:size) { should == 0 }
    its(:length) { should == 0 }
    it { should be_empty }
  end
end