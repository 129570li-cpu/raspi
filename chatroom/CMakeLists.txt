find_package(Boost CONFIG REQUIRED)

add_executable(chat_server chat_server.cpp)
add_executable(chat_client chat_client.cpp)


target_link_libraries(chat_server PRIVATE Boost::headers)
target_link_libraries(chat_client PRIVATE Boost::headers)

if(WIN32)
    target_link_libraries(chat_server PRIVATE ws2_32 mswsock)
    target_link_libraries(chat_client PRIVATE ws2_32 mswsock)
endif()