cmake_minimum_required(VERSION 3.5)
# DT10: No idea if cmake 3.10 is actually needed. I only happened to have 3.5
# locally, and it seemed to provide all needed features.
project(Hint CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined -fsanitize-undefined-trap-on-error  -O0")

option(VIVADO_BACKEND "Enable Vivado Backend")
option(INTEL_BACKEND "Enable Intel Backend")
option(BITSET_BACKEND "Enable bitset backend" ON)
option(MASKED_WORD_BACKEND "Enable masked word backend" ON)
option(WORD_ARRAY_BACKEND "Enable word array backend")
option(BOOST_CPP_INT_BACKEND "Enable boost cpp int backend" ON)

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
if(INTEL_BACKEND)
	if(NOT DEFINED INTEL_INCLUDES)
		Message(FATAL_ERROR "Please provide path to INTEL ac_int include in INTEL_INCLUDES")
	endif()	
	set(INTEL_INCLUDES ${INTEL_INCLUDES} CACHE PATH "Path to INTEL ac_int includes" FORCE)
	add_library(INTEL_AC_INT INTERFACE IMPORTED)
	target_include_directories(INTEL_AC_INT INTERFACE ${INTEL_INCLUDES})
	target_link_libraries(${LIBNAME} INTERFACE INTEL_AC_INT)
	target_compile_definitions(${LIBNAME} INTERFACE INTEL_BACKEND)
endif()

if(BITSET_BACKEND)
	target_compile_definitions(${LIBNAME} INTERFACE BITSET_BACKEND)
endif()

if(MASKED_WORD_BACKEND)
    target_compile_definitions(${LIBNAME} INTERFACE MASKED_WORD_BACKEND)
endif()

if(WORD_ARRAY_BACKEND)
	target_compile_definitions(${LIBNAME} INTERFACE WORD_ARRAY_BACKEND)
endif()

if(BOOST_CPP_INT_BACKEND)
	target_compile_definitions(${LIBNAME} INTERFACE BOOST_CPP_INT_BACKEND)
endif()

OPTION(BUILD_UNIT_TEST "Build the operators unit tests") 
if(BUILD_UNIT_TEST)
	find_package(Boost 1.55 REQUIRED COMPONENTS unit_test_framework)
	enable_testing()

if(VIVADO_BACKEND)
	add_executable(testVivadoBackend_exe tests/back_end/vivado_main.cpp)
	target_link_libraries(testVivadoBackend_exe ${LIBNAME} ${Boost_LIBRARIES}) 
	add_test(testVivadoBackend testVivadoBackend_exe)
endif(VIVADO_BACKEND)

if(INTEL_BACKEND)
	add_executable(testIntelBackend_exe tests/back_end/intel_main.cpp)
	target_link_libraries(testIntelBackend_exe ${LIBNAME} ${Boost_LIBRARIES}) 
	add_test(testIntelBackend testIntelBackend_exe)
endif(INTEL_BACKEND)

if(MASKED_WORD_BACKEND)
	add_executable(testMaskedWordBackend_exe tests/back_end/masked_word_main.cpp)
	target_link_libraries(testMaskedWordBackend_exe ${LIBNAME} ${Boost_LIBRARIES})
	add_test(testMaskedWordBackend testMaskedWordBackend_exe)
endif(MASKED_WORD_BACKEND)

if(WORD_ARRAY_BACKEND)
	add_executable(testWordArrayBackend_exe tests/back_end/word_array_main.cpp)
	target_link_libraries(testWordArrayBackend_exe ${LIBNAME} ${Boost_LIBRARIES})
	add_test(testWordArrayBackend testWordArrayBackend_exe)
endif(WORD_ARRAY_BACKEND)

if(BOOST_CPP_INT_BACKEND)
	add_executable(testBoostCppIntBackend_exe tests/back_end/boost_cpp_int_main.cpp)
	target_link_libraries(testBoostCppIntBackend_exe ${LIBNAME} ${Boost_LIBRARIES})
	add_test(testWordArrayBackend testBoostCppIntBackend_exe)
endif(BOOST_CPP_INT_BACKEND)

endif()
