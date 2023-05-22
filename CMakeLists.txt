cmake_minimum_required(VERSION 3.16)

project(helloworld)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#####################

include(cmake/dependencies.txt)

####

# Add project_options v0.21.0
# https://github.com/aminya/project_options
# Change the version in the following URL to update the package (watch the releases of the repository for future updates)
include(FetchContent)
FetchContent_Declare(_project_options URL https://github.com/aminya/project_options/archive/refs/tags/v0.21.0.zip)
FetchContent_MakeAvailable(_project_options)
include(${_project_options_SOURCE_DIR}/Index.cmake)

option(FEATURE_DOCS "Enable the docs" OFF)

# Enable sanitizers and static analyzers when running the tests
set(ENABLE_CLANG_TIDY ON)
set(ENABLE_CPPCHECK ON)
set(ENABLE_SANITIZER_ADDRESS OFF)

if(FEATURE_TESTS)
    set(ENABLE_CLANG_TIDY "ENABLE_CLANG_TIDY")
    set(ENABLE_CPPCHECK "ENABLE_CPPCHECK")

    string(FIND "$ENV{PATH}" "$ENV{VSINSTALLDIR}" index_of_vs_install_dir)
    if(# not windows
            NOT
            "${CMAKE_SYSTEM_NAME}"
            STREQUAL
            "Windows"
            # or is MSVC and has run vcvarsall
            OR (MSVC AND "${index_of_vs_install_dir}" STREQUAL "-1"))
        set(ENABLE_SANITIZER_ADDRESS "ENABLE_SANITIZER_ADDRESS")
    endif()
endif()

if(FEATURE_DOCS)
    set(ENABLE_DOXYGEN "ENABLE_DOXYGEN")
else()
    set(ENABLE_DOXYGEN OFF)
endif()

# Initialize project_options variable related to this project
# This overwrites `project_options` and sets `project_warnings`
# uncomment to enable the options. Some of them accept one or more inputs:
project_options(
        ENABLE_CACHE
        ${ENABLE_CPPCHECK}
        ${ENABLE_CLANG_TIDY}
        ENABLE_VS_ANALYSIS
        # ENABLE_CONAN
        # ENABLE_INTERPROCEDURAL_OPTIMIZATION
        # ENABLE_NATIVE_OPTIMIZATION
        ${ENABLE_DOXYGEN}
        # ENABLE_COVERAGE
        ${ENABLE_SANITIZER_ADDRESS}
        # ENABLE_SANITIZER_UNDEFINED_BEHAVIOR
        # ENABLE_SANITIZER_THREAD
        # ENABLE_SANITIZER_MEMORY
        # ENABLE_PCH
        # PCH_HEADERS
        # WARNINGS_AS_ERRORS
        # ENABLE_INCLUDE_WHAT_YOU_USE
        # ENABLE_USER_LINKER
        # ENABLE_BUILD_WITH_TIME_TRACE
        # ENABLE_UNITY
)

######

#####################

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE ftxui::screen ftxui::dom ftxui::component fmt::fmt project_options project_warnings)
target_link_system_libraries(
    main
    PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component)
