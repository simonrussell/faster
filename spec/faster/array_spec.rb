require 'spec_helper'

describe Faster::Array do

  shared_examples "an array" do
    let(:array) { klass.new(items) }

    describe "#length" do
      subject { array.length }
      it { should == items.length }
    end

    describe "#at" do
      context "in array" do
        it "should work" do
          (0...items.length).each do |i|
            array.at(i).should == items[i]
          end
        end
      end

      context "negative index" do

      end
    end
  end

  describe "Ruby Array" do
    let(:items) { [1,2,3] * 100 }
    let(:klass) { ::Array }
    it_should_behave_like "an array"
  end

  describe "int" do
    let(:items) { [1,2,3] * 100 }
    let(:klass) { Faster::Array::Int }
    it_should_behave_like "an array"
  end

end