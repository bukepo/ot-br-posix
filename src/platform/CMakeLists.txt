# Add compilation logic for eBPF if option is toggled ON
if (OTBR_BPF_FILTER)
    find_program(CLANG_PROGRAM clang REQUIRED)
    find_program(XXD_PROGRAM xxd REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBBPF REQUIRED IMPORTED_TARGET libbpf)

    # Compile source into BPF instruction bytecode
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/filter.o
        COMMAND ${CLANG_PROGRAM} -O2 -target bpf -g -c ${CMAKE_CURRENT_SOURCE_DIR}/bpf/filter.c -o ${CMAKE_CURRENT_BINARY_DIR}/filter.o
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/bpf/filter.c
    )

    # Transmute the compiled binary into a C Array Header representation
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/filter_bytecode.h
        COMMAND ${XXD_PROGRAM} -i filter.o > ${CMAKE_CURRENT_BINARY_DIR}/filter_bytecode.h
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/filter.o
    )

    # Ensure the header is generated before compiling filter_loader.cpp
    add_custom_target(generate_bpf_bytecode DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/filter_bytecode.h)

    add_library(otbr-bpf STATIC
        bpf/filter_loader.cpp
    )
    add_dependencies(otbr-bpf generate_bpf_bytecode)

    target_include_directories(otbr-bpf PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
    )
    # filter_loader.hpp uses openthread/error.h
    target_include_directories(otbr-bpf PUBLIC
        ${PROJECT_SOURCE_DIR}/third_party/openthread/repo/include
        ${CMAKE_CURRENT_SOURCE_DIR} # to allow including platform/bpf/filter_loader.hpp as "platform/bpf/filter_loader.hpp" if we add src/platform to include path of parent?
        # Actually if we include it as "platform/bpf/filter_loader.hpp", and this file is in "src/platform/bpf/filter_loader.hpp",
        # then if "src" is in include path, we can use "platform/bpf/filter_loader.hpp".
        # "src" is already in include path for most targets.
    )

    # We need to export the include directory of this library so headers can be found.
    # If someone includes "platform/bpf/filter_loader.hpp", and "src" is in include path, it works.
    # But we also want to make sure they can find it.
    target_include_directories(otbr-bpf INTERFACE
        ${PROJECT_SOURCE_DIR}/src
    )

    target_link_libraries(otbr-bpf PRIVATE
        PkgConfig::LIBBPF
        otbr-config
    )
endif()
