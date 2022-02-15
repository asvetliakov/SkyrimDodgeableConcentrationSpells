cmake_minimum_required(VERSION 3.21)

########################################################################################################################
## Define project
########################################################################################################################
project(
        CommonLibSSESamplePlugin
        VERSION 1.0.0
        LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.rc
        @ONLY
)

set(headers
        include/Sample/Sample.h
)

set(sources
        src/Main.cpp
        src/Papyrus.cpp)

set(tests
        test/Test.cpp
)

source_group(
        TREE ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
        ${headers}
        ${sources}
        ${tests}
)

########################################################################################################################
## Add CMake features
########################################################################################################################
include(CheckIPOSupported)
include(GNUInstallDirs)
include(GoogleTest)

#########################################################################################################################
### Build options
#########################################################################################################################
message("Options:")
option(BUILD_TESTS "Build unit tests." OFF)
message("\tTests: ${BUILD_TESTS}")

########################################################################################################################
## Find dependencies
########################################################################################################################
find_package(CommonLibSSE REQUIRED)

if (BUILD_TESTS)
    find_package(GTest CONFIG REQUIRED)
endif ()

########################################################################################################################
## Configuration for all targets
########################################################################################################################
if (MSVC)
    add_compile_definitions(
            UNICODE
            _UNICODE
            NOMINMAX
            _AMD64_
            WIN32_LEAN_AND_MEAN
            _CRT_USE_BUILTIN_OFFSETOF # Fixes MSVC being non-compliant with offsetof behavior by default.
    )

    if ($ENV{CLION_IDE})
        add_compile_definitions(
                __cpp_lib_char8_t         # Workaround for CLion bug.
                __cpp_consteval           # Workaround for CLion bug.
        )
    endif ()

    add_compile_options(
            /Zc:preprocessor # Enable preprocessor conformance mode
            /W4 # Warning level (all warnings)

            # warnings -> errors
            /we4715	# 'function' : not all control paths return a value

            # disable warnings
            /wd4005 # macro redefinition, needed for workarounds for CLion
            /wd4061 # enumerator 'identifier' in switch of enum 'enumeration' is not explicitly handled by a case label
            /wd4117 # definition of reserved macro, needed for workarounds for CLion
            /wd4200 # nonstandard extension used : zero-sized array in struct/union
            /wd4201 # nonstandard extension used : nameless struct/union
            /wd4265 # 'type': class has virtual functions, but its non-trivial destructor is not virtual; instances of this class may not be destructed correctly
            /wd4266 # 'function' : no override available for virtual member function from base 'type'; function is hidden
            /wd4371 # 'classname': layout of class may have changed from a previous version of the compiler due to better packing of member 'member'
            /wd4459 # declaration of 'identifier' hides global declaration
            /wd4514 # 'function' : unreferenced inline function has been removed
            /wd4582 # 'type': constructor is not implicitly called
            /wd4583 # 'type': destructor is not implicitly called
            /wd4623 # 'derived class' : default constructor was implicitly defined as deleted because a base class default constructor is inaccessible or deleted
            /wd4625 # 'derived class' : copy constructor was implicitly defined as deleted because a base class copy constructor is inaccessible or deleted
            /wd4626 # 'derived class' : assignment operator was implicitly defined as deleted because a base class assignment operator is inaccessible or deleted
            /wd4710 # 'function' : function not inlined
            /wd4711 # function 'function' selected for inline expansion
            /wd4820 # 'bytes' bytes padding added after construct 'member_name'
            /wd5026 # 'type': move constructor was implicitly defined as deleted
            /wd5027 # 'type': move assignment operator was implicitly defined as deleted
            /wd5045 # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
            /wd5053 # support for 'explicit(<expr>)' in C++17 and earlier is a vendor extension
            /wd5204 # 'type-name': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
            /wd5220 # 'member': a non-static data member with a volatile qualified type no longer implies that compiler generated copy / move constructors and copy / move assignment operators are not trivial
    )
endif ()

check_ipo_supported(RESULT USE_IPO OUTPUT IPO_OUTPUT)
if (USE_IPO)
    message("Enabling interprocedural optimizations.")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
else ()
    message("Interprocedural optimizations are not supported.")
endif ()

########################################################################################################################
## Configure target DLL
########################################################################################################################
add_library(${PROJECT_NAME} SHARED
        ${headers}
        ${sources}
        ${CMAKE_CURRENT_BINARY_DIR}/version.rc
)

target_compile_features(${PROJECT_NAME}
        PUBLIC
        cxx_std_23
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:src>
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        CommonLibSSE
        spdlog::spdlog
)

target_precompile_headers(${PROJECT_NAME}
        PRIVATE
        src/PCH.h
)

########################################################################################################################
## Configure unit tests
########################################################################################################################
if (BUILD_TESTS)
    enable_testing()

    add_executable(
            ${PROJECT_NAME}Tests
            ${headers}
            ${tests}
    )

    target_compile_features(${PROJECT_NAME}Tests
            PUBLIC
            cxx_std_23
    )

    target_link_libraries(
            ${PROJECT_NAME}Tests
            PRIVATE
            ${PROJECT_NAME}
            GTest::gmock
            GTest::gtest
            GTest::gmock_main
            GTest::gtest_main
    )

    target_precompile_headers(${PROJECT_NAME}Tests
            PRIVATE
            src/PCH.h
    )

    gtest_discover_tests(${PROJECT_NAME}Tests)
endif ()