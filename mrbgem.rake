MRuby::Gem::Specification.new('mitamae-apt') do |spec|
  spec.license = 'MIT'
  spec.author = ['Kasumi Hanazuki']
  spec.summary = 'Provide MItamae::InlineBackends::Apt for faster package management'
  spec.version = '0.1.0'

  spec.linker.libraries << 'apt-pkg'

  spec.add_dependency 'mruby-io'
end
