cmake_minimum_required(VERSION 3.27)

project(
  os
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(CMAKE_CXX_STANDARD 20)

# For some reason, adding any custom target doesn't work,
# maybe this will fix?
set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS 1)

add_subdirectory(kernel)

# Custom commands to build the ISO and run the program :)
find_program(XORRISO xorriso REQUIRED)

set(FILES
  ${PROJECT_SOURCE_DIR}/limine.cfg)

set(QEMU_FLAGS
  -M q35
  -m 2G
  -debugcon stdio)

add_custom_command(OUTPUT image.iso DEPENDS ${FILES} os
	COMMAND cmake -E copy_if_different ${FILES} kernel/os ${PROJECT_BINARY_DIR}/iso_root
	COMMAND ${XORRISO} -as mkisofs -b limine-bios-cd.bin -no-emul-boot
	-boot-load-size 4 -boot-info-table --efi-boot limine-uefi-cd.bin
	--efi-boot-part --efi-boot-image --protective-msdos-label
	${PROJECT_BINARY_DIR}/iso_root -o ${PROJECT_BINARY_DIR}/image.iso
	-quiet &> /dev/null VERBATIM USES_TERMINAL
)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/limine")
	execute_process(
		COMMAND git clone https://github.com/limine-bootloader/limine.git
		--branch=v7.x-binary --depth=1 "${CMAKE_BINARY_DIR}/limine")

	execute_process(COMMAND mkdir -p ${PROJECT_BINARY_DIR}/iso_root)
	execute_process(COMMAND cmake -E copy_if_different
		${PROJECT_BINARY_DIR}/limine/limine-bios.sys
		${PROJECT_BINARY_DIR}/limine/limine-bios-cd.bin
		${PROJECT_BINARY_DIR}/limine/limine-uefi-cd.bin
		${PROJECT_BINARY_DIR}/iso_root
	)
endif()

add_custom_target(run
	COMMAND qemu-system-x86_64
             -boot d
             -cdrom ${PROJECT_BINARY_DIR}/image.iso ${QEMU_FLAGS}
	DEPENDS image.iso USES_TERMINAL VERBATIM)
