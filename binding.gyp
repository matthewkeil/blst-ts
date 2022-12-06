{
  'targets': [
    {
      'target_name': 'blst-ts',
      'sources': [
        'blst/src/server.c',
        'blst/build/assembly.S',
        'src/bindings/bindings.cc',
        'src/bindings/blst_ts_utils.cpp',
        'src/bindings/secret_key.cpp',
        'src/bindings/public_key.cpp',
        'src/bindings/signature.cpp',
        'src/bindings/functions/tests/byte_array.cpp',
        'src/bindings/functions/aggregate_public_keys.cpp', 
      ],
      'libraries': [ '-lsodium' ],
      'dependencies': [ "<!(node -p \"require('node-addon-api').gyp\")" ],
      'include_dirs': [
        'blst/bindings',
        "<!@(node -p \"require('node-addon-api').include\")",
      ],
      'defines': [ 'NAPI_CPP_EXCEPTIONS' ],
      'cflags!': [
          '-fno-builtin-memcpy',
          '-fexceptions',
          '-Wextern-c-compat'
      ],
      'cflags_cc!': [
          '-fexceptions',
      ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '12'
      },
      'msvs_settings': { 'VCCLCompilerTool': { 'ExceptionHandling': 1 } },
      'conditions': [
        [ 'OS=="win"', {
            'sources': [ 'blst/build/win64/*-x86_64.asm' ],
          }
        ],
        [ 'OS=="linux"', {
            'ldflags': [ '-Wl,-Bsymbolic' ],
          }
        ],
      ],
    }
  ]
}