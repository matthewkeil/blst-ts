{
  'targets': [
    {
      'target_name': 'blst-ts-addon-native',
      'sources': [
        "blst/bindings/blst.h",
        'src/addon/blst_ts.hpp',
        'src/addon/blst_ts_addon.cc',
        'src/addon/blst_ts_utils.h',
        'src/addon/vmas_worker.cpp',
        'src/addon/verify_multiple_aggregate_signatures.cpp',
      ],
      'libraries': [
        "-lsodium",
        "-Wl,-rpath,blst/libblst.a",
      ],
      'include_dirs': [
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
        'MACOSX_DEPLOYMENT_TARGET': '12'
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      }
    }
  ]
}