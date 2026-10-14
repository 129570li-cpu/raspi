#include <iostream>
#include <sys/utsname.h>

#include "network/TcpServer.h"
int main() {
  // TIP 当文本光标位于 <b>lang</b> 变量名称处时，按 <shortcut
  // actionId="RenameElement"/> 可以查看 CLion 如何帮助您重命名该变量。

  TcpServer server(9000);

  if (!server.start()) {
    return 1;
  }

  server.run();

  return 0;
}