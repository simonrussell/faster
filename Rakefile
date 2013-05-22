require 'rspec/core/rake_task'
require 'rake/extensiontask'

Rake::ExtensionTask.new('faster_mri')

desc 'Default: run specs.'
task :default => [:spec]

desc "Run specs"
RSpec::Core::RakeTask.new do |t|
#  t.pattern = "./spec/**/*_spec.rb" # don't need this, it's default.
  # Put spec opts in a file named .rspec in root
end
