find_package(Qt6 REQUIRED COMPONENTS
        Widgets
        Network
)

add_executable(device_client
        main.cpp
)

target_link_libraries(device_client PRIVATE
        Qt6::Widgets
        Qt6::Network
)