{
  "targets": [
    {
      "target_name": "addon",
      "cxxflags": ["-std=c++0x"],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
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
      "sources": [ "addon.cc", "MemorySlice.cc", "SharedMemory.cc" ],
    }
  ]
}
