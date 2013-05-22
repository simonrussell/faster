require 'spec_helper'

describe Faster::Array do

  shared_examples "an array" do
    let(:array) { klass.new(items) }

    describe "#at" do
      context "in array" do
        it "should work" do
          (0...items.length).each do |i|
            array.at(i).should == items[i]
          end
        end
      end

      context "after end" do
        it "should work" do
          (items.length...items.length+100).each do |i|
            array.at(i).should be_nil
          end
        end
      end
    end

    describe "#<<" do
      it "should work" do
        array << -4
        (0...items.length).each do |i|
          array.at(i).should == items[i]
        end
        array.at(array.length - 1).should == -4
      end
    end

    describe "#insert" do
      it "should work" do
        array.insert(3, -4)
        (0...3).each do |i|
          array.at(i).should == items[i]
        end
        array.at(3).should == -4
        (4...items.length).each do |i|
          array.at(i).should == items[i-1]
        end
      end
    end

    describe "#dup" do
      it "should work" do
        other = array.dup
        array.delete_at(0)

        (0...items.length).each do |i|
          other.at(i).should == items[i]
        end
      end
    end

    describe "#delete_at" do
      it "should work" do
        array.insert(3,-4)
        array.length.should == items.length + 1
        array.delete_at(3).should == -4
        array.length.should == items.length

        (0...items.length).each do |i|
          array.at(i).should == items[i]
        end
      end
    end

    describe "simple" do
      subject { array }

      its(:last) { should == items.last }
      its(:length) { should == items.length }
      its(:size) { should == items.size }
    end
  end

  describe "Ruby Array" do
    let(:items) { (0..rand(200)).to_a }
    let(:klass) { ::Array }
    it_should_behave_like "an array"
  end

  describe "int" do
    let(:items) { (0..rand(200)).to_a }
    let(:klass) { Faster::Array::Int }
    it_should_behave_like "an array"
  end

end