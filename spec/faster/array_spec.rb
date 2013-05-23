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
      let(:new_item) { rand(123) }
      subject { array << new_item }

      it { should == items + [new_item] }
      its(:length) { should == items.length + 1 }
    end

    describe "#insert" do
      let(:new_item) { rand(123) }
      subject { array.insert(new_index, new_item) }

      context "in middle" do
        let(:new_index) { items.length / 2 }
        it { should == items[0...new_index] + [new_item] + items[new_index..-1] }
        its(:length) { should == items.length + 1 }
      end

      context "at start" do
        let(:new_index) { 0 }
        it { should == [new_item] + items }
        its(:length) { should == items.length + 1 }
      end

      context "at end" do
        let(:new_index) { array.length }
        it { should == items + [new_item] }
        its(:length) { should == items.length + 1 }
      end
    end

    describe "#dup" do
      it "should work" do
        other = array.dup
        array.delete_at(0)
        other.should == items
      end
    end

    describe "#delete_at" do
      it "should work" do
        array.insert(0,-4)
        array.length.should == items.length + 1
        array.delete_at(0).should == -4
        array.should == items
      end
    end

    describe "simple" do
      subject { array }

      its(:last) { should == items.last }
      its(:length) { should == items.length }
      its(:size) { should == items.size }
      its(:empty?) { should == items.empty? }
    end
  end

  describe "Ruby Array" do
    let(:items) { (0..rand(150..200)).to_a }
    let(:klass) { ::Array }
    it_should_behave_like "an array"
  end

  describe "int" do
    let(:klass) { Faster::Array::Int }

    context "no items" do
      let(:items) { [] }
      it_should_behave_like "an array"
    end

    context "some items" do
      let(:items) { (0..rand(150..200)).to_a }
      it_should_behave_like "an array"
    end
  end

end