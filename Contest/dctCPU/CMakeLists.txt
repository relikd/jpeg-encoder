cmake_minimum_required(VERSION 3.6)
project(dctCPU)

if ("${CMAKE_GENERATOR}" MATCHES "Visual Studio*")
	# Try to set Intel Platform
	#SET( CMAKE_GENERATOR_TOOLSET "Intel C++ Compiler 17.0" CACHE STRING "Platformtoolset" FORCE )

	# Optimization 
	# - Optimization                     : Maximize Speed (/O2)
	# - Inline Function Expansion        : Any Suitable (/Ob2)
	# - Enable Intrinsic Functions       : Yes (/Oi)
	# - Favor Size or Speed              : Favor Fast Code (/Ot)
	# - Enable Fiber-Safe Optimization   : Yes (/GT)
	SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ob2 /Oi /Ot /GT" )

	# Code Generation
	# - Disable Security Checks (access illegal memory)
	# - Disable Control Flow Guard (access illegal code block)
	SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GS-" )
	message(STATUS "Manually set:")
	message("[Code Generation]    > Disable Control Flow Guard (/guard:cf-)")

	# Language
	# - Enable Run-Time Type Information : No (/GR-) 
	# - Float Point Model                : Fast (/fp:fast)
	SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GR- /fp:fast" )
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
	# Intel specific Optimization
	# - Optimize for Windows Application : Yes (/GA)
	# - Float Point Model                : Fast=2 (/fp:fast=2)
	# - Intel Architecture Optimization  : Skylake (/tune:skylake)
	SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GA /fp:fast=2 /tune:skylake" )
	message("[Optimization Intel] > Optimize for Windows Application (/GA)")
	message("[Code Generation]    > Float Point Model (/fp:fast=2)")
	message("[Code Generation]    > Intel Architecture Optimization (/tune:skylake)")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" # GCC
	OR
	"${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") # also match AppleClang
	# Add C++ 11 support
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
	SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -0fast" )
endif()


file( GLOB_RECURSE SOURCE_FILES src/* )

set( EXECUTABLE_OUTPUT_PATH "../bin" CACHE PATH "Output directory" )

add_executable( dctCPU ${SOURCE_FILES} )
set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT dctCPU )
