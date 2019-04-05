cmake_minimum_required(VERSION 3.10)
project(Hint CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(VIVADO_BACKEND "Enable Vivado Backend")
option(QUARTUS_BACKEND "Enable Quartus Backend")
option(BITSET_BACKEND "Enable bitset backend" ON)

set(LIBNAME hint)
set(INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(${LIBNAME} INTERFACE)
target_include_directories(${LIBNAME} INTERFACE ${INCLUDE_ROOT})

if(VIVADO_BACKEND)
	if(NOT DEFINED VIVADO_INCLUDES)
		Message(FATAL_ERROR "Please provide path to Vivado ap_int include in VIVADO_INCLUDES")
	endif()	
	set(VIVADO_INCLUDES ${VIVADO_INCLUDES} CACHE PATH "Path to vivado ap_int includes" FORCE)
	add_library(VIVADO_AP_INT INTERFACE IMPORTED)
	target_include_directories(VIVADO_AP_INT INTERFACE ${VIVADO_INCLUDES})
	target_link_libraries(${LIBNAME} INTERFACE VIVADO_AP_INT)
	target_compile_definitions(${LIBNAME} INTERFACE VIVADO_BACKEND)
endif()
if(QUARTUS_BACKEND)
	if(NOT DEFINED QUARTUS_INCLUDES)
		Message(FATAL_ERROR "Please provide path to quartus ac_int include in QUARTUS_INCLUDES")
	endif()	
	set(QUARTUS_INCLUDES ${QUARTUS_INCLUDES} CACHE PATH "Path to quartus ac_int includes" FORCE)
	add_library(QUARTUS_AP_INT INTERFACE IMPORTED)
	target_include_directories(QUARTUS_AP_INT INTERFACE ${QUARTUS_INCLUDES})
	target_link_libraries(${LIBNAME} INTERFACE QUARTUS_AP_INT)
	target_compile_definitions(${LIBNAME} INTERFACE AC_INT_BACKEND)
endif()

if(BITSET_BACKEND)
	target_compile_definitions(${LIBNAME} INTERFACE BITSET_BACKEND)
endif()

OPTION(BUILD_UNIT_TEST "Build the operators unit tests") 
if(BUILD_UNIT_TEST)
	find_package(Boost 1.55 REQUIRED COMPONENTS unit_test_framework)
	enable_testing()
	add_executable(testBitsetBackend_exe tests/back_end/bitset_main.cpp)
	target_link_libraries(testBitsetBackend_exe ${LIBNAME} ${Boost_LIBRARIES}) 
	add_test(testBitsetBackend testBitsetBackend_exe)
	#		add_executable(testAritOps_exe tests/testArithmeticOp.cpp)
	#		target_include_directories(testAritOps_exe PRIVATE ${SOFTPOSIT_H})
	#		target_compile_options(testAritOps_exe PRIVATE -O3 -fopenmp)
	#		target_link_libraries(testAritOps_exe ${LIBNAME} ${Boost_LIBRARIES} ${SOFTPOSIT_LIB}  -fopenmp) 
	#		add_test(testAritOps testAritOps_exe)
	#	else()
	#		Message(WARNING "Tests on posit value cannot be run")
	#	endif()
	#
	#	find_path(MPFR mpfr.h)
	#
	#	if(MPFR)
	#		MESSAGE(STATUS "MPFR found : ${MPFR}")
	#		add_executable(testKulisch_exe tests/testKulisch.cpp)
	#		target_compile_definitions(testKulisch_exe PRIVATE MPFR)
	#		target_compile_options(testKulisch_exe PRIVATE -O3 -lmpfr)
	#		target_link_libraries(testKulisch_exe PRIVATE ${LIBNAME} ${Boost_LIBRARIES} -lmpfr)
	#		add_test(testKulisch testKulisch_exe)
	#	else()
	#		Message(WARNING "Tests on Kulisch value cannot be run")
	#	endif()
endif()