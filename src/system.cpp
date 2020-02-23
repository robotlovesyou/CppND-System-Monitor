#include "system.h"

#include <algorithm>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using LinuxParser::MemoryValues;
using LinuxParser::ProcessValues;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  processes_.clear();
  vector<ProcessValues> process_list = LinuxParser::ProcessValuesList();

  long system_uptime = LinuxParser::UpTime();

  for (auto const& pv : process_list) {
    if (process_by_pid_.find(pv.pid) != process_by_pid_.end()) {
      Process process = process_by_pid_.find(pv.pid)->second;
      process.Update(system_uptime, pv);
      processes_.push_back(process);
    } else {
      Process process(system_uptime, pv);
      processes_.push_back(process);
    }
  }

  // Clear the process map and re-add to prevent a slow memory leak
  // due to exited processes never being cleaned up
  process_by_pid_.clear();
  for (auto const& process : processes_) {
    process_by_pid_.emplace(process.Pid(), process);
  }

  std::sort(processes_.begin(), processes_.end());
  return processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() {
  // calculation based on answer given at https://stackoverflow.com/a/41251290
  MemoryValues values{};
  LinuxParser::MemoryUtilization(values);
  return (float)(values.total - values.free) /
         std::max((float)values.total, 1.0f);
}

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }
