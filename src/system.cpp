#include <unistd.h>
#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using LinuxParser::MemoryValues;
using std::set;
using std::size_t;
using std::string;
using std::vector;

// Accessor function for cpu member
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { return processes_; }

// Returns the name of the kernel
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Calculates current memory utilization.
// calculation based on answer given at https://stackoverflow.com/a/41251290
float System::MemoryUtilization() {
  MemoryValues values{};
  LinuxParser::MemoryUtilization(values);
  return (float)(values.total - values.free) /
         std::max((float)values.total, 1.0f);
}

// Returns the name of the operating system.
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() {
    return LinuxParser::UpTime();
}