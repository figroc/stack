cmake_minimum_required(VERSION 2.8)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CONFIGURATION_TYPES Debug)

project(mon)

set(MON_VERSION_MAJOR 1)
set(MON_VERSION_MINOR 0)

set(MON_BUILD_FLAGS "-fPIC")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MON_BUILD_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MON_BUILD_FLAGS}")

configure_file (
  "${PROJECT_SOURCE_DIR}/src/mon.h.in"
  "${PROJECT_BINARY_DIR}/mon.h"
)
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_BINARY_DIR}")
link_directories(/usr/local/lib)

add_library(util OBJECT
  "src/util/String.cpp"
  "src/util/Network.cpp"
)

add_library(conf OBJECT
  "src/conf/Conf.cpp"
  "src/conf/SvcRole.cpp"
  "src/conf/CfgLoader.cpp" 
  "src/conf/SvcLocator.cpp"
  "src/conf/SvcSetting.cpp"
)

add_library(perf OBJECT
  "src/perf/PerfCounter.cpp"
  "src/perf/PerfManager.cpp"
  "src/perf/PerfMonitor.cpp"
)

add_library(trace OBJECT 
  "src/trace/TraceFormat.cpp"
  "src/trace/TraceClient.cpp"
  "src/trace/TraceFacet.cpp"
)

add_library(msvc SHARED
  "src/mon.cpp"
  $<TARGET_OBJECTS:util>
  $<TARGET_OBJECTS:conf>
  $<TARGET_OBJECTS:perf>
  $<TARGET_OBJECTS:trace>
)

target_link_libraries(msvc
  boost_system
  boost_chrono
  boost_thread
  boost_program_options
  cppnetlib-uri
  cppnetlib-server-parsers
)

add_library(util_test OBJECT
  "test/util/StringUtilTest.cpp" 
  "test/util/NetworkTest.cpp"
)

add_library(conf_test OBJECT 
  "test/conf/SvcLocatorTest.cpp"
  "test/conf/SvcSettingTest.cpp"
)

add_executable(msvc_test 
  "test/TestSuite.cpp" 
  $<TARGET_OBJECTS:util_test> 
  $<TARGET_OBJECTS:conf_test>
)

target_link_libraries(msvc_test
  msvc 
  boost_unit_test_framework
)

