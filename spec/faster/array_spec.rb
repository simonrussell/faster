require 'spec_helper'

describe Faster::Array do

  shared_examples "an array" do

  end

  describe "Ruby Array" do
    let(:klass) { ::Array }
    it_should_behave_like "an array"
  end

  describe "int" do
    let(:klass) { Faster::Array::Int }
    it_should_behave_like "an array"
  end

end