cmake_minimum_required(VERSION 3.10)
project(Hint
        VERSION 0.4.0
	DESCRIPTION "A type safe compatibility layer between major HLS tools"
	HOMEPAGE_URL https://github.com/yuguen/hint
	LANGUAGES CXX
    )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CPACK_PACKAGE_NAME "libhint")
#set(CPACK_PACKAGE_VENDOR "(Florent de Dinechin, Luc Forget, Yohann Uguen)@[Citi Lab - INSA Lyon - INRIA]  David Thomas@Imperial College London")

set(INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(hint INTERFACE)
target_include_directories(
    hint INTERFACE
    $<BUILD_INTERFACE:${INCLUDE_ROOT}>
    $<INSTALL_INTERFACE:include/hint>
)

set(VIVADO_INCLUDES ${VIVADO_INCLUDES} CACHE PATH "Path to vivado ap_int includes")

if(EXISTS ${VIVADO_INCLUDES}/ap_int.h)
    set(VIVADO_BACKEND ON)
    message(STATUS "Using ${VIVADO_INCLUDES} as vivado include directory")
    #add_library(VIVADO_AP_INT INTERFACE IMPORTED)
    #target_include_directories(VIVADO_AP_INT INTERFACE ${VIVADO_INCLUDES})
    # Direct inclusion is less elegant but most simpler for export
    target_include_directories(hint INTERFACE ${VIVADO_INCLUDES})
    target_compile_definitions(hint INTERFACE VIVADO_BACKEND)
else()
	message(STATUS "Vivado include path unset please set VIVADO_INCLUDES if you inten to use Vivado HLS backend" )
endif()

set(INTEL_INCLUDES ${INTEL_INCLUDES} CACHE PATH "Path to INTEL ac_int includes")
if(EXISTS ${INTEL_INCLUDES}/ref/ac_int.h)
    set(INTEL_BACKEND ON)
    message(STATUS "Using ${INTEL_INCLUDES} as intelHLS include directory")
    #add_library(INTEL_AC_INT INTERFACE IMPORTED)
    # Direct inclusion is less elegant but most simpler for export
    target_include_directories(hint INTERFACE ${INTEL_INCLUDES})
    #target_link_libraries(hint INTERFACE INTEL_AC_INT)
    target_compile_definitions(hint INTERFACE INTEL_BACKEND)
else()
	message(STATUS "IntelHLS include path unset please set INTEL_INCLUDES if you inten to use Intel HLS backend" )
endif()

if(BITSET_BACKEND)
        target_compile_definitions(hint INTERFACE BITSET_BACKEND)
endif()


install(DIRECTORY ${INCLUDE_ROOT}/ DESTINATION include/hint FILES_MATCHING PATTERN "*.hpp" PATTERN "*.ipp")
install(
    TARGETS hint
    EXPORT libhint
    PUBLIC_HEADER DESTINATION "include/hint"
    )
install(EXPORT libhint DESTINATION ${CMAKE_INSTALL_PREFIX}/include/hint/)
export(EXPORT libhint FILE libhint-config.cmake)
export(PACKAGE libhint)

OPTION(BUILD_UNIT_TEST "Build the operators unit tests") 
if(BUILD_UNIT_TEST)
	find_package(Boost 1.55 REQUIRED COMPONENTS unit_test_framework)
	enable_testing()

	if(VIVADO_BACKEND)
		add_executable(testVivadoBackend_exe tests/backend/vivado_main.cpp)
		target_link_libraries(testVivadoBackend_exe hint ${Boost_LIBRARIES})
		target_include_directories(testVivadoBackend_exe PRIVATE ${Boost_INCLUDE_DIR})
		add_test(testVivadoBackend testVivadoBackend_exe)
	endif()
	if(INTEL_BACKEND)
		add_executable(testIntelBackend_exe tests/backend/intel_main.cpp)
		target_link_libraries(testIntelBackend_exe hint ${Boost_LIBRARIES})
		target_include_directories(testIntelBackend_exe PRIVATE ${Boost_INCLUDE_DIR})
		add_test(testIntelBackend testIntelBackend_exe)
	endif()

    if(VIVADO_BACKEND OR INTEL_BACKEND)
        add_executable(testPrimitives_exe tests/primitives/lzoc_shifter_and_shifter_sticky_main.cpp)
	target_link_libraries(testPrimitives_exe hint ${Boost_LIBRARIES})
	target_include_directories(testPrimitives_exe PRIVATE ${Boost_INCLUDE_DIR})
        add_test(testPrimitives testPrimitives_exe)
	endif()
endif()

#include(CPack)
