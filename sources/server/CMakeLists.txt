# Executables
add_executable(TcpServer tcp_server.cpp)
add_executable(UdpServer udp_server.cpp)

# Link
target_link_libraries(TcpServer PRIVATE Utils)
target_link_libraries(UdpServer PRIVATE Utils)
