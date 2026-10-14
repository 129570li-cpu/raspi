//
// Created by 李玉辉 on 2026/10/13.
//
#include <string>
#include <string_view>
#include "TcpServer.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include<iostream>
using std::string;
namespace
{
  bool sendAll(int fd, const char* data, std::size_t length)
  {
    size_t totalSent = 0;
    while (totalSent < length)
    {
      ssize_t n=send(fd, data+totalSent, length-totalSent, 0);
      if (n>0)
      {
        totalSent+=n;
      }
      else if (n < 0 && errno == EINTR)
      {
        continue;
      }
      else
      {
        return false;
      }
    }
    return true;
  }
  std::string processCommand(std::string_view command)
  {
    if (command == "PING")
    {
      return "PONG\n";
    }

    return "UNKNOWN\n";
  }
}
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
  while (true)
  {
    if (listenFd_ < 0) {
      std::cerr << "Server not started\n";
      return;
    }

    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);
    int clientFD=accept(listenFd_,reinterpret_cast<sockaddr*>(&clientAddr),&clientLen);
    if (clientFD < 0)
    {
      if (errno == EINTR)
        continue;

      std::cerr << "accept failed: "
                << std::strerror(errno) << '\n';
      return;
    }
    char buffer[1024];
    string pending;
    while (true)
    {
      ssize_t n=recv(clientFD,buffer,sizeof(buffer),0);
      if (n<0)
      {
        if (errno == EINTR)
          continue;

        std::cerr << "recv failed: "
                  << std::strerror(errno) << '\n';

        break;
      }
      if (n>0)
      {
        pending.append(buffer, static_cast<std::size_t>(n));
        while (true)
        {
          std::size_t pos = pending.find('\n');

          if (pos == std::string::npos)
          {
            // 还没有完整的一行，等待下一次 recv
            break;
          }

          // 3. 提取一条完整命令，不包含 '\n'
          std::string command = pending.substr(0, pos);

          // 4. 从 pending 中删除已经处理的命令和换行符
          pending.erase(0, pos + 1);

          // 兼容 Windows 常见的 \r\n 换行
          if (!command.empty() && command.back() == '\r')
          {
            command.pop_back();
          }

          // 5. 根据命令生成响应
          std::string response = processCommand(command);

          std::cout << "Command: " << command << '\n';

          // 6. 将完整响应发送给客户端
          if (!sendAll(clientFD, response.data(), response.size()))
          {
            std::cerr << "sendAll failed\n";
            break;
          }
        }
      }
      else
      {
        std::cout << "Client disconnected\n";
        break;
      }
    }
    close(clientFD);
  }
}