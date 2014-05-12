{
  "targets": [
    {
      "target_name": "addon",
      "cflags!": [ "-fno-exceptions", "-fpermissive" ],
      "cflags_cc!": [ "-fno-exceptions", "-fpermissive" ],
      'conditions': [
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
          }
        }]
      ],
      "ldflags": [
        "-lrt"
      ],
      "sources": [ "addon.cc", "SharedMemory.cc" ],
    }
  ]
}
