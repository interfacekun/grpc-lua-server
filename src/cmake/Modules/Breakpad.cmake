set(BREAKPAD_SRC_PATH ${PROJECT_SOURCE_DIR}/thirdpart/breakpad/src)

set(BREAKPAD_INCLUDE_DIR ${BREAKPAD_SRC_PATH})

if(WINDOWS)
 set(BREAKPAD_SRC 
        ${BREAKPAD_SRC_PATH}/common/windows/string_utils-inl.h
        ${BREAKPAD_SRC_PATH}/common/windows/guid_string.h
        ${BREAKPAD_SRC_PATH}/client/windows/handler/exception_handler.h
        ${BREAKPAD_SRC_PATH}/client/windows/common/ipc_protocol.h
        ${BREAKPAD_SRC_PATH}/google_breakpad/common/minidump_format.h
        ${BREAKPAD_SRC_PATH}/google_breakpad/common/breakpad_types.h
        ${BREAKPAD_SRC_PATH}/client/windows/crash_generation/crash_generation_client.h
        ${BREAKPAD_SRC_PATH}/common/scoped_ptr.h
        ${BREAKPAD_SRC_PATH}/client/windows/handler/exception_handler.cc
        ${BREAKPAD_SRC_PATH}/common/windows/string_utils.cc
        ${BREAKPAD_SRC_PATH}/common/windows/guid_string.cc
        ${BREAKPAD_SRC_PATH}/client/windows/crash_generation/crash_generation_client.cc
    )

elseif(MACOSX)
    set(BREAKPAD_SRC
        ${BREAKPAD_SRC_PATH}/client/mac/crash_generation/crash_generation_client.cc
        ${BREAKPAD_SRC_PATH}/client/mac/crash_generation/crash_generation_server.cc
        ${BREAKPAD_SRC_PATH}/client/mac/handler/breakpad_nlist_64.cc
        ${BREAKPAD_SRC_PATH}/client/mac/handler/dynamic_images.cc
        ${BREAKPAD_SRC_PATH}/client/mac/handler/exception_handler.cc
        ${BREAKPAD_SRC_PATH}/client/mac/handler/minidump_generator.cc
        ${BREAKPAD_SRC_PATH}/client/mac/handler/protected_memory_allocator.cc
        ${BREAKPAD_SRC_PATH}/client/minidump_file_writer.cc
        ${BREAKPAD_SRC_PATH}/common/convert_UTF.c
        ${BREAKPAD_SRC_PATH}/common/mac/arch_utilities.cc
        ${BREAKPAD_SRC_PATH}/common/mac/bootstrap_compat.cc
        ${BREAKPAD_SRC_PATH}/common/mac/file_id.cc
        ${BREAKPAD_SRC_PATH}/common/mac/launch_reporter.cc
        ${BREAKPAD_SRC_PATH}/common/mac/MachIPC.mm
        ${BREAKPAD_SRC_PATH}/common/mac/macho_id.cc
        ${BREAKPAD_SRC_PATH}/common/mac/macho_utilities.cc
        ${BREAKPAD_SRC_PATH}/common/mac/macho_walker.cc
        ${BREAKPAD_SRC_PATH}/common/mac/string_utilities.cc
        ${BREAKPAD_SRC_PATH}/common/md5.cc
        ${BREAKPAD_SRC_PATH}/common/simple_string_dictionary.cc
        ${BREAKPAD_SRC_PATH}/common/string_conversion.cc
    )

    set_source_files_properties(
        ${BREAKPAD_SRC_PATH}/common/mac/MachIPC.mm
        PROPERTIES
        COMPILE_FLAGS "-x objective-c++")
elseif(LINUX)

endif()
