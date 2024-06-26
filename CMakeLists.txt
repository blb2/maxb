cmake_minimum_required(VERSION 3.16)

if(WIN32)
	# Default to Windows 10.
	set(CMAKE_SYSTEM_VERSION 10.0 CACHE INTERNAL "")

	# Set some variables/values that were introduced with CMake >= 3.27.
	if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.27)
		# Ensure that the version of the Windows SDK that's selected is 10.0 and
		# not a specific version. This is only done if version= is not given on
		# command-line. This has to be done before project().
		if(CMAKE_GENERATOR MATCHES "Visual Studio" AND NOT CMAKE_GENERATOR_PLATFORM MATCHES "version=")
			if(CMAKE_GENERATOR_PLATFORM)
				string(APPEND CMAKE_GENERATOR_PLATFORM ",")
			endif()
			string(APPEND CMAKE_GENERATOR_PLATFORM "version=10.0")
		endif()
	endif()
endif()

project(maxb VERSION 0.1.0 LANGUAGES CXX)

message(STATUS "CMake Version: ${CMAKE_VERSION}")
message(STATUS "CMake System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMake Generator: ${CMAKE_GENERATOR}")
message(STATUS "CMake Source Dir: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMake Binary Dir: ${CMAKE_BINARY_DIR}")

# Not allowing mixing of the CMake source and build directories.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	message(FATAL_ERROR "The source tree and build tree cannot be the same.")
endif()

# Set the default build type. See the following link,
#   https://www.kitware.com/cmake-and-the-default-build-type/
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to 'Release' as none was specified.")
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
elseif(CMAKE_BUILD_TYPE)
	message(STATUS "CMake Build Type: ${CMAKE_BUILD_TYPE}")
elseif(CMAKE_CONFIGURATION_TYPES)
	message(STATUS "CMake Build Types: ${CMAKE_CONFIGURATION_TYPES}")
endif()

# Allow usage of folders if supported.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# This project requires at least C++14.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add options specifically for building with Visual Studio. Most of these were
# obtained by porting over the default settings that Visual Studio uses. These
# are set here in order to override what CMake sets as the default flags. For
# example, CMake hard-codes /Zi for builds with debugging information, however
# I want to use what VS uses -- /ZI.
if(MSVC)
	set(PROJ_CXX_FLAGS "/W3 /WX- /sdl /EHsc /GS /permissive- /Zc:inline")
	set(PROJ_CXX_FLAGS_DEBUG "/ZI /Od /RTC1")
	set(PROJ_CXX_FLAGS_RELEASE "/O2 /Oi /GL /Gy")
	set(PROJ_CXX_FLAGS_MINSIZEREL "/O1 /Oi /GL /Gy")
	set(PROJ_CXX_FLAGS_RELWITHDEBINFO "/Zi ${PROJ_CXX_FLAGS_RELEASE}")
	set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG         OFF)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE        ON)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL     ON)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)

	set(PROJ_EXE_LINKER_FLAGS_DEBUG "/DEBUG /INCREMENTAL")
	set(PROJ_EXE_LINKER_FLAGS_RELEASE "/OPT:REF /OPT:ICF /INCREMENTAL:NO")
	set(PROJ_EXE_LINKER_FLAGS_MINSIZEREL "${PROJ_EXE_LINKER_FLAGS_RELEASE}")
	set(PROJ_EXE_LINKER_FLAGS_RELWITHDEBINFO "/DEBUG ${PROJ_EXE_LINKER_FLAGS_RELEASE}")
	set(PROJ_SHARED_LINKER_FLAGS_DEBUG "${PROJ_EXE_LINKER_FLAGS_DEBUG}")
	set(PROJ_SHARED_LINKER_FLAGS_RELEASE "${PROJ_EXE_LINKER_FLAGS_RELEASE}")
	set(PROJ_SHARED_LINKER_FLAGS_MINSIZEREL "${PROJ_EXE_LINKER_FLAGS_MINSIZEREL}")
	set(PROJ_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${PROJ_EXE_LINKER_FLAGS_RELWITHDEBINFO}")

	# Define the appropriate debug macros for every platform.
	string(APPEND PROJ_CXX_FLAGS_DEBUG " /D_DEBUG")
	string(APPEND PROJ_CXX_FLAGS_RELEASE " /DNDEBUG")
	string(APPEND PROJ_CXX_FLAGS_MINSIZEREL " /DNDEBUG")
	string(APPEND PROJ_CXX_FLAGS_RELWITHDEBINFO " /DNDEBUG")
else()
	# Define the appropriate debug macros for every platform.
	set(PROJ_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
	set(PROJ_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")
	set(PROJ_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -DNDEBUG")
	set(PROJ_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DNDEBUG")
	set(PROJ_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
	set(PROJ_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
	set(PROJ_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL}")
	set(PROJ_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")
	set(PROJ_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
	set(PROJ_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
	set(PROJ_SHARED_LINKER_FLAGS_MINSIZEREL "${CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL}")
	set(PROJ_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO}")
endif()

if(WIN32)
	# Target Windows 7 at a minimum.
	string(APPEND PROJ_CXX_FLAGS " /D_WIN32_WINNT=0x0601")
	# Ensure the WIN32 macro is defined.
	string(APPEND PROJ_CXX_FLAGS " /DWIN32")
	# Put the Win32 API onto a diet.
	string(APPEND PROJ_CXX_FLAGS " /DWIN32_LEAN_AND_MEAN /DSTRICT /DSTRICT_TYPED_ITEMIDS /DNOMINMAX")
	# Ensure we're using Unicode APIs when building for Windows.
	string(APPEND PROJ_CXX_FLAGS " /DUNICODE /D_UNICODE")
else()
	string(APPEND PROJ_CXX_FLAGS " /D_GNU_SOURCE")
endif()

string(APPEND PROJ_CXX_FLAGS " /DASIO_STANDALONE")

# Set the flag overrides.
set(CMAKE_CXX_FLAGS "${PROJ_CXX_FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "${PROJ_CXX_FLAGS_DEBUG}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "${PROJ_CXX_FLAGS_RELEASE}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL "${PROJ_CXX_FLAGS_MINSIZEREL}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${PROJ_CXX_FLAGS_RELWITHDEBINFO}" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${PROJ_EXE_LINKER_FLAGS_DEBUG}" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${PROJ_EXE_LINKER_FLAGS_RELEASE}" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${PROJ_EXE_LINKER_FLAGS_MINSIZEREL}" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${PROJ_EXE_LINKER_FLAGS_RELWITHDEBINFO}" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${PROJ_SHARED_LINKER_FLAGS_DEBUG}" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${PROJ_SHARED_LINKER_FLAGS_RELEASE}" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "${PROJ_SHARED_LINKER_FLAGS_MINSIZEREL}" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${PROJ_SHARED_LINKER_FLAGS_RELWITHDEBINFO}" CACHE STRING "" FORCE)

set(
	HEADERS
	src/platform.h
)

set(SOURCES src/maxb.cpp)

set(
	PCH
	<cassert>
	<cstdint>
	<cstdio>
	<cstdlib>
	<algorithm>
	<chrono>
	<future>
	<memory>
	<random>
	<string>
	<thread>
	<vector>
	<asio.hpp>
)

if(WIN32)
	list(APPEND SOURCES src/platform_win32.cpp)
	list(APPEND PCH <windows.h>)
elseif(APPLE)
	list(APPEND SOURCES src/platform_macos.cpp)
	list(APPEND PCH <pthread.h> <sched.h>)
elseif(LINUX)
	list(APPEND SOURCES src/platform_linux.cpp)
	list(APPEND PCH <pthread.h> <sched.h>)
else()
	message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

add_executable(maxb ${SOURCES} ${HEADERS})
target_precompile_headers(maxb PRIVATE ${PCH})
target_include_directories(maxb PRIVATE "external/asio/include")
