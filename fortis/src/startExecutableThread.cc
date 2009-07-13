#include <iostream>
#include <string>
#include <stdlib.h>
#include "fortis/FORTIS.hh"

/// Entry point for thread that starts the command-line-program that streams data from FORTIS
void* startExecutableThread(void * args) {
  ExecutableArgs * exeArgs = (ExecutableArgs*)args;
  std::string exeName = (exeArgs->dir) + (exeArgs->filename) + " " + (exeArgs->args) + " &";
  std::string killCommand = "killall " + (exeArgs->filename);

  std::cout << "Executing killall command: " << killCommand << std::endl;
  system(killCommand.c_str());
  
  std::cout << "Executing: " << exeName << std::endl;
  system( exeName.c_str() );

  // usleep(1000);

  return NULL;
}
