find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

qt_add_executable(Qtclient
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        TcpClient.cpp
        TcpClient.h
)

target_link_libraries(Qtclient PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
)