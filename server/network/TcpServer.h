//
// Created by 李玉辉 on 2026/10/13.
//

#ifndef RASPI_TCPSERVER_H
#define RASPI_TCPSERVER_H
#pragma once

#include <cstdint>


class TcpServer {
public:
  explicit TcpServer(std::uint16_t port);
  ~TcpServer();

  TcpServer(const TcpServer&) = delete;
  TcpServer& operator=(const TcpServer&) = delete;

  bool start();
  void run();
  void stop();

private:
  std::uint16_t port_;
  int listenFd_ = -1;
};



#endif //RASPI_TCPSERVER_H
