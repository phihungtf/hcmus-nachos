#include "syscall.h"

int main() {
  int pingID;
  int pongID;

  pingID = Exec("./test/ping");
  PrintInt(pingID);
  pongID = Exec("./test/pong");
  PrintInt(pongID);
}
