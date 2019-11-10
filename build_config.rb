MRuby::Build.new do |conf|
  toolchain :gcc

  conf.gem __dir__
  conf.gem core: 'mruby-bin-mirb'
  conf.gem core: 'mruby-bin-mruby'
  conf.gem core: 'mruby-print'

  conf.enable_debug
end
