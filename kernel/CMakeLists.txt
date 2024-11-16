

add_executable(os
  main.cpp
  printf.cpp)

target_include_directories(os PRIVATE
  ${PROJECT_SOURCE_DIR}/include)

target_compile_options(os PRIVATE
    -std=gnu++20
    -Wall
    -Wextra
    -ffreestanding
    -fno-stack-protector
    -fno-stack-check
    -fPIE
    -m64
    -march=x86-64
    -mno-80387
    -mno-mmx
    -mno-sse
    -mno-sse2
    -mno-red-zone
    -fno-rtti
    -fno-exceptions
    -MMD
    -MP)

target_link_options(os PRIVATE
    -nostdlib
    -Wl,-m,elf_x86_64
    -Wl,-nostdlib
    -Wl,-pie
    -Wl,-z,text
    -Wl,-z,max-page-size=0x1000
    -Wl,-T,${PROJECT_SOURCE_DIR}/linker.ld)
