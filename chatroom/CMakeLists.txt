find_package(Boost CONFIG REQUIRED)

if(WIN32)
    # Windows：构建客户端
    add_executable(chat_client chat_client.cpp)

    target_link_libraries(chat_client PRIVATE
            Boost::headers
            ws2_32
            mswsock
    )

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Linux：构建服务端
    add_executable(chat_server chat_server.cpp
            Request_Handler.cpp
            Request_Handler.h)

    target_link_libraries(chat_server PRIVATE
            Boost::headers
    )
endif()