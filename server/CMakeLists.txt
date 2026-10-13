add_executable(device_server
        main.cpp
        network/TcpServer.cpp
        network/TcpServer.h
)

target_compile_options(device_server PRIVATE
        -Wall -Wextra -Wpedantic
)