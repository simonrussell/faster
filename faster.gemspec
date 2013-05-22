require File.expand_path('../lib/faster/version', __FILE__)

Gem::Specification.new do |s|
  s.name = "faster"
  s.version = Faster::VERSION
  s.summary = "Faster collections and stuff"

  s.author = "Simon Russell"
  s.email = "spam+faster@bellyphant.com"
  s.homepage = "http://github.com/simonrussell/faster"

  s.add_development_dependency 'rspec'
  s.add_development_dependency 'ffaker'

  s.required_ruby_version = '>= 1.9.3'

  s.files = Dir['lib/**/*.rb'] + ['LICENSE']
end
