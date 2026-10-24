find_package(Boost CONFIG REQUIRED)

add_executable(chat_server chat_server.cpp)
add_executable(chat_client chat_client.cpp)
add_executable(my_chat_server my_chat_server.cpp)

target_link_libraries(chat_server PRIVATE Boost::headers)
target_link_libraries(chat_client PRIVATE Boost::headers)
target_link_libraries(my_chat_server PRIVATE Boost::headers)
if(WIN32)
    target_link_libraries(chat_server PRIVATE ws2_32 mswsock)
    target_link_libraries(chat_client PRIVATE ws2_32 mswsock)
    target_link_libraries(my_chat_server PRIVATE ws2_32 mswsock)
    target_compile_options(chat_client PRIVATE
            -finput-charset=UTF-8
            -fexec-charset=UTF-8
    )
endif()