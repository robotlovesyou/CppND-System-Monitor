#ifndef PROCESS_H
#define PROCESS_H

#include <string>

#include "linux_parser.h"

using LinuxParser::ProcessValues;

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  struct SystemInfo {
   public:
    long uptime{};
    long prev_uptime{};
  };

  struct ProcessInfo {
   public:
    ProcessValues values{};
    ProcessValues prev_values{};
  };

  Process(SystemInfo system_info, ProcessInfo process_info);

  int Pid() const;
  std::string User();
  std::string Command();
  float CpuUtilization() const;
  std::string Ram();
  long int UpTime() const;
  bool operator<(Process const& a) const;
  static const unsigned long MB_KB = 0x1ul << 10ul;

 private:
  SystemInfo system_info_;
  ProcessInfo process_info_;
};

#endif