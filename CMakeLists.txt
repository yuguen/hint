cmake_minimum_required(VERSION 3.10)
project(Hint
		VERSION 0.4.0
	DESCRIPTION "A type safe compatibility layer between major HLS tools"
	LANGUAGES CXX
	)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#set(CPACK_PACKAGE_NAME "libhint")
#set(CPACK_PACKAGE_VENDOR "(Florent de Dinechin, Luc Forget, Yohann Uguen)@[Citi Lab - INSA Lyon - INRIA]  David Thomas@Imperial College London")

set(INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(hint INTERFACE)
target_include_directories(
	hint INTERFACE
	$<BUILD_INTERFACE:${INCLUDE_ROOT}>
	$<INSTALL_INTERFACE:include/hint>
)

install(DIRECTORY ${INCLUDE_ROOT}/ DESTINATION include/hint FILES_MATCHING PATTERN "*.hpp" PATTERN "*.ipp")
install(
	TARGETS hint
	EXPORT libhint
	PUBLIC_HEADER DESTINATION "include/hint"
	)
install(EXPORT libhint DESTINATION ${CMAKE_INSTALL_PREFIX}/include/hint/)
export(EXPORT libhint FILE libhint-config.cmake)
export(PACKAGE libhint)


#find_path(GMP_INCLUDE_DIR gmpxx.h)
#find_library(GMP gmp)
#find_library(GMPXX gmpxx)

#if (NOT (GMPXX-NOTFOUND OR GMP-NOTFOUND OR GMP_INCLUDE_DIR-NOTFOUND))
#	set(GMPTARGET-OK ON)
#	message(STATUS "GMP found")
#else()
#	set(GMPTARGET-NOTFOUND ON)
#	message(STATUS "GMP Not found")
#endif()

OPTION(BUILD_UNIT_TEST "Build the operators unit tests")
if(BUILD_UNIT_TEST)
	find_package(Boost 1.55 REQUIRED COMPONENTS unit_test_framework)

	enable_testing()

	set(VIVADO_INCLUDES ${VIVADO_INCLUDES} CACHE PATH "Path to vivado ap_int includes")

	if(EXISTS ${VIVADO_INCLUDES}/ap_int.h)
		set(VIVADO_BACKEND ON)
		message(STATUS "Using ${VIVADO_INCLUDES} as vivado include directory")
	else()
			message(STATUS "Vivado include path unset please set VIVADO_INCLUDES if you intend to use Vivado HLS backend" )
	endif()

	set(INTEL_INCLUDES ${INTEL_INCLUDES} CACHE PATH "Path to INTEL ac_int includes")
	if(EXISTS ${INTEL_INCLUDES}/ref/ac_int.h)
		set(INTEL_BACKEND ON)
		message(STATUS "Using ${INTEL_INCLUDES} as intelHLS include directory")
	else()
			message(STATUS "IntelHLS include path unset please set INTEL_INCLUDES if you intend to use Intel HLS backend" )
	endif()

	set(CATAPULT_INCLUDES ${CATAPULT_INCLUDES} CACHE PATH "Path to CATAPULT ac_int includes")
	if(EXISTS ${CATAPULT_INCLUDES}/ac_int.h)
		set(CATAPULT_BACKEND ON)
		message(STATUS "Using ${CATAPULT_INCLUDES} as CatapultHLS include directory")
	else()
			message(STATUS "CatapultHLS include path unset please set CATAPULT_INCLUDES if you intend to use Catapult HLS backend" )
	endif()

	if(VIVADO_BACKEND)
		add_executable(testVivadoBackend_exe tests/backend/vivado_main.cpp tests/backend/vivado_table.cpp)
		target_link_libraries(testVivadoBackend_exe hint ${Boost_LIBRARIES})
		target_include_directories(testVivadoBackend_exe PRIVATE ${Boost_INCLUDE_DIR})
		target_include_directories(testVivadoBackend_exe PRIVATE ${VIVADO_INCLUDES})
		target_compile_definitions(testVivadoBackend_exe PRIVATE VIVADO_BACKEND)
		add_test(testVivadoBackend testVivadoBackend_exe)
	endif()
	if(INTEL_BACKEND)
		add_executable(testIntelBackend_exe tests/backend/intel_main.cpp tests/backend/intel_table.cpp)
		target_link_libraries(testIntelBackend_exe hint ${Boost_LIBRARIES})
		target_include_directories(testIntelBackend_exe PRIVATE ${Boost_INCLUDE_DIR})
		target_include_directories(testIntelBackend_exe PRIVATE ${INTEL_INCLUDES})
		target_compile_definitions(testIntelBackend_exe PRIVATE INTEL_BACKEND)
		add_test(testIntelBackend testIntelBackend_exe)
	endif()

	if(CATAPULT_BACKEND)
		add_executable(testCatapultBackend_exe tests/backend/catapult_main.cpp)
		target_link_libraries(testCatapultBackend_exe hint ${Boost_LIBRARIES})
		target_include_directories(testCatapultBackend_exe PRIVATE ${Boost_INCLUDE_DIR})
		target_include_directories(testCatapultBackend_exe PRIVATE ${CATAPULT_INCLUDES})
		target_compile_definitions(testCatapultBackend_exe PRIVATE CATAPULT_BACKEND)
		add_test(testCatapultBackend testCatapultBackend_exe)
	endif()

		#if(GMPTARGET-OK)
		#	add_executable(testGMPBackend_exe tests/backend/gmp_main.cpp)
		#	target_link_libraries(testGMPBackend_exe hint ${Boost_LIBRARIES} ${GMP} ${GMPXX})
		#	target_compile_definitions(testGMPBackend_exe PRIVATE FASTSIM_BACKEND)
		#	add_test(testGMPBackend testGMPBackend_exe)
		#endif()

	if(VIVADO_BACKEND OR INTEL_BACKEND)
		add_executable(testPrimitives_exe tests/primitives/lzoc_shifter_and_shifter_sticky_main.cpp)
	target_link_libraries(testPrimitives_exe hint ${Boost_LIBRARIES})
	target_include_directories(testPrimitives_exe PRIVATE ${Boost_INCLUDE_DIR})
	if(VIVADO_BACKEND)
		target_include_directories(testPrimitives_exe PRIVATE ${VIVADO_INCLUDES})
		target_compile_definitions(testPrimitives_exe PRIVATE VIVADO_BACKEND)
	endif()
	if(INTEL_BACKEND)
		target_include_directories(testPrimitives_exe PRIVATE ${INTEL_INCLUDES})
		target_compile_definitions(testPrimitives_exe PRIVATE INTEL_BACKEND)
	endif()
		add_test(testPrimitives testPrimitives_exe)
	endif()

	add_executable(testArithmetic_exe tests/arithmetic/arithmetic_main.cpp tests/arithmetic/tiling.cpp)
	target_link_libraries(testArithmetic_exe hint ${Boost_LIBRARIES})
	add_test(testArithmetic testArithmetic_exe)
endif()

#include(CPack)
