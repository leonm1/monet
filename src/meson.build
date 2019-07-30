BOOST_ROOT = '/usr/'
boost_dep = dependency('boost', modules : ['multiprecision'])

executable('monet',
           'main.cpp',
           'Memory.cpp',
           'Interpreter.cpp',
           'Exception.cpp',
           install : true,
           dependencies: boost_dep)
