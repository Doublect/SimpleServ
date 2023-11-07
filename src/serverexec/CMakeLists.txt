add_executable(serverexec main.cpp)

target_link_libraries(
  serverexec
  PRIVATE 
		simpleserv::simpleserv_options
		simpleserv::simpleserv_warnings)


target_link_libraries(serverexec PRIVATE simpleserv::simpleserv)

target_link_system_libraries(serverexec PRIVATE wolfssl::wolfssl Boost::log)