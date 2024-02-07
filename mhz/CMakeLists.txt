# Linux / gcc will fails to run test when nested dependencies.

# Configure RPath
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# get source files
file(GLOB mhz_SOURCES ./src/*.c)
file(GLOB mhz_HEADERS ./include/*.h)

# make sure libraries exist
check_library_exists(m sin "" HAVE_LIB_M)

if(HAVE_LIB_M)
    list(APPEND mhz_LIBS m)
endif(HAVE_LIB_M)

# Generate version header
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/version.h
)

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

# add executable
add_executable(mhz
    MACOSX_BUNDLE ${mhz_SOURCES}
)

target_link_libraries(mhz PRIVATE ${mhz_LIBS})
target_compile_options(mhz PRIVATE -Wno-unused-result)

# Optional IPO. Do not use IPO if it's not supported by compiler.
check_ipo_supported(RESULT HAS_LTO LANGUAGES C)

if(HAS_LTO)
    set_target_properties(mhz PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(WARNING "IPO is not supported: ${output}")
endif()

install(TARGETS mhz
    RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)
