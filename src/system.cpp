#include <unistd.h>
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using LinuxParser::MemoryValues;
using LinuxParser::ProcessValues;
using std::set;
using std::size_t;
using std::string;
using std::vector;

// Accessor function for cpu member
Processor& System::Cpu() { return cpu_; }

// container composed of the system's processes
vector<Process>& System::Processes()
{
    processes_.clear();
    vector<ProcessValues> process_list = LinuxParser::ProcessValuesList();

    long system_uptime = LinuxParser::UpTime();

    Process::SystemInfo system_info{system_uptime, prev_uptime_};
    for (auto const& pv : process_list) {
        ProcessValues last;
        if (last_process_values_.find(pv.pid)!=last_process_values_.end()) {
            last = last_process_values_[pv.pid];
        }
        else {
            last = pv;
        }

        Process::ProcessInfo process_info{pv, last};
        Process process(system_info, process_info);
        processes_.push_back(process);
        last_process_values_[pv.pid] = pv;
    }

    prev_uptime_ = system_uptime;
    std::sort(processes_.begin(), processes_.end());
    return processes_;
}

// Returns the name of the kernel
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Calculates current memory utilization.
// calculation based on answer given at https://stackoverflow.com/a/41251290
float System::MemoryUtilization()
{
    MemoryValues values{};
    LinuxParser::MemoryUtilization(values);
    return (float) (values.total-values.free)/
            std::max((float) values.total, 1.0f);
}

// Returns the name of the operating system.
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }
