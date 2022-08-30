{
  'targets': [
    {
      'target_name': 'nedis',
      'sources': [ 'src/nedis.cc' ],
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
      "dependencies": [
        "deps/hiredis.gyp:hiredis"
      ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'cflags_cc+': [ '-std=c++11 ' ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7'
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      }
    }
  ]
}