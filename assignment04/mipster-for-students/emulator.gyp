{
  'variables': {
    'default_cflags': [
      '-Wall',
      '-Werror',
      '-g',
      '-O0',
      '-std=c++11',
    ],
    'default_ldflags': [
      '-lelf',
      '-lgflags',
    ]
  },
  'targets': [
    {
      'target_name': 'mipster',
      'product_name': 'mipster',
      'type': 'executable',
      'cflags': [
        '<@(default_cflags)',
        '-I/usr/include'
      ],
      'libraries': [
        '<@(default_ldflags)'
      ],
      'xcode_settings': {
        'OTHER_CFLAGS':  [
          '<@(default_cflags)',
          '-I/opt/local/include',
          '-I/opt/local/include/libelf',
        ],
        'OTHER_LDFLAGS': [
          '<@(default_ldflags)'
        ],
        'LIBRARY_SEARCH_PATHS': [
          '/opt/local/lib'
        ],
      },
      'sources': [
        'emulator/bin/main.cc',
        'emulator/config.h',
        'emulator/machine/memory.h',
        'emulator/machine/memory.cc',
        'emulator/machine/mips32.h',
        'emulator/machine/mips32.cc',
        'emulator/machine/mips32_isa.h',
        'emulator/machine/mips32_isa.cc',
        'emulator/os/loader.h',
        'emulator/os/loader.cc',
        'emulator/os/process.h',
        'emulator/os/process.cc',
        'emulator/os/os.h',
        'emulator/os/os.cc',
        'emulator/os/os_syscall.cc',
        'emulator/utils.h',
      ],
      'include_dirs': [
        '.',
        'emulator/',
      ]
    }
  ]
}
