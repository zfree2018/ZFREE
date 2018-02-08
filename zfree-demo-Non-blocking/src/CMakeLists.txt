
if(CMAKE_BUILD_TYPE MATCHES Debug)
  message("Debug build.")
  message("Debug output enabled.")
  add_definitions(-DDEBUG_MODE)
else()
endif()

add_library(sniffer sniffer.cpp)
add_library(pkt_hash packet_hash.cpp)


target_link_libraries(sniffer tins)

add_executable(proxy2 proxy2.cpp)
target_link_libraries (proxy2
  tins
  pthread
  ssl
  sniffer
  crypto
  pkt_hash)


add_executable (proxy1 proxy1.cpp)
target_link_libraries (proxy1
  tins
  crypto
  ssl
  pthread
  sniffer
  pkt_hash
        net_static)


