{
  'targets': [
    {
      'target_name': 'hiredis',
      'type': 'static_library',
      'direct_dependent_settings': {
        'include_dirs': [ '.' ],
      },
      'sources': [
        './hiredis/alloc.c',
        './hiredis/async.c',
        './hiredis/dict.c',
        './hiredis/hiredis.c',
        './hiredis/net.c',
        './hiredis/read.c',
        './hiredis/sds.c',
        './hiredis/sockcompat.c',
      ],
      'conditions': [
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_C_LANGUAGE_STANDARD': 'c99'
          }
        }],
        ['OS=="solaris"', {
          'cflags+': [ '-std=c99' ]
        }],
        ['OS=="linux"', {
          'cflags+': [ '-std=c99' ]
        }]
      ]
    }
  ]
}
