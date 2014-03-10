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
  "src/util/Uri.cpp"
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
  "src/trace/TraceFile.cpp"
)

add_library(log OBJECT 
  "src/log/LogClient.cpp"
)

add_library(db OBJECT 
  "src/db/Database.cpp"
  "src/db/spec/OprSpec.cpp"
  "src/db/util/DbPerfC.cpp"
  "src/db/impl/mongo/MongoDbUri.cpp"
  "src/db/impl/mongo/MongoDbHelper.cpp"
  "src/db/impl/mongo/MongoDbConnPool.cpp"
  "src/db/impl/mongo/MongoDbClient.cpp"
  "src/db/impl/mysql/MySqlDbUri.cpp"
  "src/db/impl/mysql/MySqlDbHelper.cpp"
  "src/db/impl/mysql/MySqlDbConnPool.cpp"
  "src/db/impl/mysql/MySqlDbClient.cpp"
)

add_library(cache OBJECT 
  "src/cache/Cache.cpp"
  "src/cache/util/CachePerfC.cpp"
  "src/cache/impl/redis/RedisUri.cpp"
  "src/cache/impl/redis/RedisConnPool.cpp"
  "src/cache/impl/redis/RedisClient.cpp"
)

add_library(sock OBJECT
  "src/sock/ThreadPool.cpp"
  "src/sock/EPollWatcher.cpp"
  "src/sock/EPollManager.cpp"
  "src/sock/Socket.cpp"
)

add_library(msvc STATIC
  "src/mon.cpp"
  $<TARGET_OBJECTS:util>
  $<TARGET_OBJECTS:conf>
  $<TARGET_OBJECTS:perf>
  $<TARGET_OBJECTS:trace>
  $<TARGET_OBJECTS:log>
  $<TARGET_OBJECTS:db>
  $<TARGET_OBJECTS:cache>
  $<TARGET_OBJECTS:sock>
)

target_link_libraries(msvc
  boost_system
  boost_chrono
  boost_thread
  boost_date_time
  boost_program_options
  boost_filesystem
  cppnetlib-uri
  cppnetlib-server-parsers
  mongoclient
  mysqlcppconn
  hiredis
)

add_executable(test 
  "test/TestSuite.cpp" 
  "test/util/StringTest.cpp" 
  "test/util/NetworkTest.cpp"
  "test/util/UriTest.cpp"
  "test/conf/SvcLocatorTest.cpp"
  "test/conf/SvcSettingTest.cpp"
  "test/log/LogTest.cpp"
  "test/db/DatabaseTest.cpp"
  "test/cache/CacheTest.cpp"
)

target_link_libraries(test
  msvc boost_unit_test_framework
)

