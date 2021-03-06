cmake_minimum_required(VERSION 2.4)

project(val)

set( CMAKE_MODULE_PATH    ${CMAKE_MODULE_PATH}
                          ${CMAKE_CURRENT_SOURCE_DIR}
                          ${CMAKE_CURRENT_SOURCE_DIR}/..
                          ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules)

set(DOXYFILE_TARGET_SUFFIX val)
include(UseDoxygen OPTIONAL)

IF("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
#set(CMAKE_OSX_DEPLOYMENT_TARGET "10.7")
#FIND_LIBRARY(COREFOUNDATION_FRAMEWORK CoreFoundation)
#FIND_LIBRARY(IOKIT_FRAMEWORK IOKit)
#SET (MAC_LIBRARIES ${COREFOUNDATION_FRAMEWORK} ${IOKIT_FRAMEWORK})
set(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -DIL_STD -I")
ENDIF()
add_subdirectory(parsing)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${ParsePDDL_SOURCE_DIR})

set(libInst_SRCS
      SimpleEval.cpp
      FastEnvironment.cpp instantiation.cpp
      typecheck.cpp TypedAnalyser.cpp FuncAnalysis.cpp
      TIM.cpp TimSupport.cpp TypeStripWC.cpp
)

add_library(Inst STATIC ${libInst_SRCS})

