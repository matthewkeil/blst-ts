{
  'targets': [
    {
      'target_name': 'blst-ts-addon-native',
      'sources': [
        'src/addon/utils.h',
        'src/addon/blst.hpp',
        'src/addon/blst_ts_addon.cc',
        'src/addon/blst_ts_addon.h',
      ],
      'libraries': [
        "-lsodium",
        "-Wl,-rpath,blst/libblst.a",
      ],
      'include_dirs': [
        "blst/bindings",
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'dependencies': [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      'cflags!': [
         '-fno-exceptions',
         '-Wextern-c-compat'
      ],
      'cflags_cc!': [
         '-fno-exceptions',
         '-Wextern-c-compat'
      ],
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