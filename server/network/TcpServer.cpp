//
// Created by 李玉辉 on 2026/10/13.
//

#include "TcpServer.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include<iostream>
TcpServer::TcpServer(std::uint16_t port)
    : port_(port)
{
}

TcpServer::~TcpServer()
{
  stop();
}

void TcpServer::stop()
{
  if (listenFd_ >= 0) {
    close(listenFd_);
    listenFd_ = -1;
  }
}

bool TcpServer::start()
{
  if (listenFd_ >= 0) {
    return false;
  }

  // 1. 创建 TCP Socket
  listenFd_=socket(AF_INET,SOCK_STREAM,0);

  if (listenFd_ < 0) {
    std::cerr << "socket failed: "
              << std::strerror(errno) << '\n';
    return false;
  }

  // 2. 允许快速重新绑定端口
  int opt = 1;

  if (setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR,
                 &opt, sizeof(opt)) < 0) {
    std::cerr << "setsockopt failed: "
              << std::strerror(errno) << '\n';
    stop();
    return false;
                 }

  // 3. 设置监听地址
sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr=htons(INADDR_ANY);
  serverAddr.sin_port=htons(port_);

  //4 bind 绑定端口和ip
  if (bind(listenFd_,reinterpret_cast<sockaddr*>(&serverAddr),sizeof(serverAddr))<0)
  {
    std::cerr << "bind failed: "
              << std::strerror(errno) << '\n';
    stop();
    return false;
  }
  // 5. 开始监听
  if (listen(listenFd_, SOMAXCONN) < 0) {
    std::cerr << "listen failed: "
              << std::strerror(errno) << '\n';
    stop();
    return false;
  }

  std::cout << "Server listening on port "
            << port_ << std::endl;

  return true;
}

void TcpServer::run()
{
  if (listenFd_ < 0) {
    std::cerr << "Server not started\n";
    return;
  }

  sockaddr_in clientAddr{};
  socklen_t clientLen = sizeof(clientAddr);

  // 等待客户端连接
  int clientFd = accept(
      listenFd_,
      reinterpret_cast<sockaddr*>(&clientAddr),
      &clientLen
  );

  if (clientFd < 0) {
    std::cerr << "accept failed: "
              << std::strerror(errno) << '\n';
    return;
  }

  std::cout << "Client connected\n";

  char buffer[1024]{};

  // 接收一次客户端数据
  ssize_t n = recv(clientFd, buffer,
                   sizeof(buffer), 0);

  if (n > 0) {
    std::cout << "Received: ";
    std::cout.write(buffer, n);
    std::cout << '\n';

    const char response[] = "PONG";

    ssize_t sent = send(
        clientFd,
        response,
        sizeof(response) - 1,
        0
    );

    if (sent < 0) {
      std::cerr << "send failed: "
                << std::strerror(errno) << '\n';
    }
  } else if (n < 0) {
    std::cerr << "recv failed: "
              << std::strerror(errno) << '\n';
  }

  close(clientFd);
}