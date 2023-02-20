{
  'targets': [
    {
      'target_name': 'blst_ts_addon',
      'sources': [
        'deps/blst/src/server.c',
        'deps/blst/build/assembly.S',
        'src/addon.cc',
        'src/utils.cc',
        # 'src/functions.cc',
        # 'src/public_key.cc',
        'src/secret_key.cc',
        # 'src/signature.cc',
      ],
      'dependencies': [ "<!(node -p \"require('node-addon-api').gyp\")" ],
      'include_dirs': [
        'deps/blst/bindings',
        "<!@(node -p \"require('node-addon-api').include_dir\")",
      ],
      'defines': [ 'NAPI_CPP_EXCEPTIONS' ],
      'cflags!': [
          '-fno-exceptions',
          '-fno-builtin-memcpy',
          '-Wextern-c-compat',
          '-Werror',
          '-Wall',
          '-Wextra',
          '-Wpedantic',
          '-Wunused-parameter',
      ],
      'cflags_cc!': [
          '-fno-exceptions',
          '-Werror',
          '-Wall',
          '-Wextra',
          '-Wpedantic',
          '-Wunused-parameter',
      ],
      'conditions': [
        [ 'OS=="win"', {
            'sources': [ 'blst/build/win64/*-x86_64.asm' ],
            'defines': [ '_HAS_EXCEPTIONS=1' ],
            'msvs_settings': {
              'VCCLCompilerTool': {
                'ExceptionHandling': 1,
                'EnablePREfast': 'true',
              },
            },
          }
        ],
        [ 'OS=="linux"', {
            'ldflags': [ '-Wl,-Bsymbolic' ],
          }
        ],
        ['OS=="mac"', {
          'cflags+': ['-fvisibility=hidden'],
          'xcode_settings': {
            'OTHER_CFLAGS': ['-fvisibility=hidden'],
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'CLANG_CXX_LIBRARY': 'libc++',
            'MACOSX_DEPLOYMENT_TARGET': '12',
          }
        }]
      ],
    }
  ]
}