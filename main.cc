#include <cstdlib>
#include <iostream>

#include "server.h"

int main(void) {

  std::cout << "Hello World" << std::endl;


  Server server(1234);

  server.server_create("","");

  return EXIT_SUCCESS;
}
