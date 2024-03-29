cmake_minimum_required(VERSION 3.10)
project(Hint
		VERSION 0.4.0
	DESCRIPTION "A type safe compatibility layer between major HLS tools"
	LANGUAGES CXX
	)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

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
	EXPORT ${PROJECT_NAME}Targets
	PUBLIC_HEADER DESTINATION "include/hint"
	)
install(FILES ${PROJECT_NAME}Config.cmake DESTINATION lib/cmake/${PROJECT_NAME})
install(EXPORT ${PROJECT_NAME}Targets FILE ${PROJECT_NAME}Targets.cmake DESTINATION lib/cmake/${PROJECT_NAME})

set(VIVADO_INCLUDES ${VIVADO_INCLUDES} CACHE PATH "Path to vivado ap_int includes")

if(EXISTS ${VIVADO_INCLUDES}/ap_int.h)
    set(VIVADO_BACKEND ON CACHE BOOL "Is vivado backend available" FORCE)
    add_library(HINT_VIVADO_BACKEND INTERFACE IMPORTED GLOBAL)
    target_include_directories(HINT_VIVADO_BACKEND INTERFACE ${VIVADO_INCLUDES})
    target_compile_definitions(HINT_VIVADO_BACKEND INTERFACE VIVADO_BACKEND)
    message(STATUS "Using ${VIVADO_INCLUDES} as vivado include directory")
else()
    set(VIVADO_BACKEND OFF CACHE BOOL "Is vivado backend available" FORCE)
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

OPTION(BUILD_HINT_UNIT_TEST "Build the operators unit tests")
if(BUILD_HINT_UNIT_TEST)
	find_package(Boost 1.55 REQUIRED COMPONENTS unit_test_framework)

	enable_testing()

	if(VIVADO_BACKEND)
		add_executable(testVivadoBackend_exe tests/backend/vivado_main.cpp tests/backend/vivado_table.cpp)
		target_link_libraries(testVivadoBackend_exe hint ${Boost_LIBRARIES} HINT_VIVADO_BACKEND)
		target_include_directories(testVivadoBackend_exe PRIVATE ${Boost_INCLUDE_DIR})
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
        target_link_libraries(testPrimitives_exe PRIVATE hint ${Boost_LIBRARIES})
        target_include_directories(testPrimitives_exe PRIVATE ${Boost_INCLUDE_DIR})
        if(VIVADO_BACKEND)
            target_link_libraries(testPrimitives_exe PRIVATE HINT_VIVADO_BACKEND)
        endif()
        if(INTEL_BACKEND)
            target_include_directories(testPrimitives_exe PRIVATE ${INTEL_INCLUDES})
            target_compile_definitions(testPrimitives_exe PRIVATE INTEL_BACKEND)
        endif()
            add_test(testPrimitives testPrimitives_exe)
	endif()

	if(EXTINT_BACKEND)
		add_executable(testExtIntBackend_exe tests/backend/extint_main tests/backend/extint_arithmetic tests/backend/extint_logic)
		target_link_libraries(testExtIntBackend_exe hint ${Boost_LIBRARIES})
		target_include_directories(testExtIntBackend_exe PRIVATE ${Boost_INCLUDE_DIR})
		target_compile_definitions(testExtIntBackend_exe PRIVATE EXTINT_BACKEND)
		add_test(testExtIntBackend testExtIntBackend_exe)
	endif()

	add_executable(testArithmetic_exe tests/arithmetic/arithmetic_main.cpp tests/arithmetic/tiling.cpp)
	target_link_libraries(testArithmetic_exe hint ${Boost_LIBRARIES})
	add_test(testArithmetic testArithmetic_exe)
endif()

#include(CPack)
