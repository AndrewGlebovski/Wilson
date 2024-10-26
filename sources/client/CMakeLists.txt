# Executables
add_executable(TcpClient tcp_client.cpp)
add_executable(UdpClient udp_client.cpp)

# Link
target_link_libraries(TcpClient PRIVATE Utils)
target_link_libraries(UdpClient PRIVATE Utils)
