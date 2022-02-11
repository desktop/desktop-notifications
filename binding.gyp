{
  'targets': [
    {
      'target_name': 'desktop-notifications',
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      },
      'include_dirs': [
        '<!(node -p "require(\'node-addon-api\').include_dir")' ],
      'defines': [
        "NAPI_VERSION=<(napi_build_version)",
      ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.14',
      },
      'conditions': [
        ['OS=="win"', {
          "defines": [
            "UNICODE",
          ],
          'sources': [
            'src/main_win.cc',
            'src/DesktopNotificationsManager.cpp',
            'src/DesktopNotification.cpp',
            'src/Utils.cpp'
          ],
          "libraries": [
            "runtimeobject.lib"
          ],
          'msvs_disabled_warnings': [
            4267,  # conversion from 'size_t' to 'int', possible loss of data
            4530,  # C++ exception handler used, but unwind semantics are not enabled
            4506,  # no definition for inline function
          ],
        }],
        ['OS=="mac"', {
          'sources': [
            'src/main_mac.mm'
          ],
          'libraries': [
            '$(SDKROOT)/System/Library/Frameworks/UserNotifications.framework',
          ],
        }],
      ],
    }
  ]
}
