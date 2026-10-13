add_executable(device_server
        main.cpp
)

target_compile_options(device_server PRIVATE
        -Wall -Wextra -Wpedantic
)