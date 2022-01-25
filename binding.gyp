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
      'conditions': [
        ['OS=="win"', {
          "defines": [
            "UNICODE",
          ],
          'sources': [
            'src/main.cc',
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
      ],
    }
  ]
}
