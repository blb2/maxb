cmake_minimum_required(VERSION 3.16)

project(maxb VERSION 0.1.0 LANGUAGES CXX)

message(STATUS "CMake Version: ${CMAKE_VERSION}")
message(STATUS "CMake System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMake Generator: ${CMAKE_GENERATOR}")
message(STATUS "CMake Source Dir: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMake Binary Dir: ${CMAKE_BINARY_DIR}")

# Not allowing mixing of the CMake source and build directories.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
	message(FATAL_ERROR "The source tree and build tree cannot be the same.")
endif()

# Allow usage of folders if supported.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# This project requires at least C++14.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add options specifically for building with Visual Studio. Most of these were
# obtained by porting over the default settings that Visual Studio uses. These
# are set here in order to override what CMake sets as the default flags. For
# example, CMake hard-codes /Zi for builds with debugging information, however
# I want to use what VS uses -- /ZI.
if(MSVC)
	set(CMAKE_CXX_FLAGS "/W3 /WX- /sdl /EHsc /GS /permissive- /Zc:inline")
	set(CMAKE_CXX_FLAGS_DEBUG "/ZI /Od /RTC1")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Oi /GL /Gy")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "/O1 /Oi /GL /Gy")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Zi ${CMAKE_CXX_FLAGS_RELEASE}")
	set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG         OFF)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE        ON)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL     ON)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)

	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/OPT:REF /OPT:ICF")
	set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
	set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/DEBUG ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
	set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
	set(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL}")
	set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")
endif()

# Define the appropriate debug macros for every platform.
string(APPEND CMAKE_CXX_FLAGS_DEBUG          " /D_DEBUG ")
string(APPEND CMAKE_CXX_FLAGS_RELEASE        " /DNDEBUG ")
string(APPEND CMAKE_CXX_FLAGS_MINSIZEREL     " /DNDEBUG ")
string(APPEND CMAKE_CXX_FLAGS_RELWITHDEBINFO " /DNDEBUG ")

if(WIN32)
	# Target Windows 7 at a minimum.
	string(APPEND CMAKE_CXX_FLAGS " /D_WIN32_WINNT=0x0601")
	# Ensure the WIN32 macro is defined.
	string(APPEND CMAKE_CXX_FLAGS " /DWIN32")
	# Put the Win32 API onto a diet.
	string(APPEND CMAKE_CXX_FLAGS " /DWIN32_LEAN_AND_MEAN /DSTRICT /DSTRICT_TYPED_ITEMIDS /DNOMINMAX")
	# Ensure we're using Unicode APIs when building for Windows.
	string(APPEND CMAKE_CXX_FLAGS " /DUNICODE /D_UNICODE")
else()
	string(APPEND CMAKE_CXX_FLAGS " /DUNICODE /D_GNU_SOURCE")
endif()

string(APPEND CMAKE_CXX_FLAGS " /DASIO_STANDALONE")

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
