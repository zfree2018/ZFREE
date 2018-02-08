cmake_minimum_required (VERSION 3.1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)
project (Sniffer2)

add_subdirectory(src bin)
# enable testing first.
enable_testing()

set(prj_config ${CMAKE_SOURCE_DIR}/config)
set(prj_bin ${CMAKE_CURRENT_BINARY_DIR}/bin)

configure_file(${prj_config}/privkey.pem ${prj_bin}/privkey.pem COPYONLY)
configure_file(${prj_config}/cacert.pem ${prj_bin}/cacert.pem COPYONLY)



   