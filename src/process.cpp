#include <unistd.h>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <processor.h>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() const { return process_info_.values.pid; }

float process_secs(long utime, long stime) {
    return (float)(utime+stime) / (float)sysconf(_SC_CLK_TCK);
}

// Return this process's CPU utilization
float Process::CpuUtilization() const
{
    float total_time = process_secs(process_info_.values.utime_ticks, process_info_.values.stime_ticks);
    float prev_time = process_secs(process_info_.prev_values.utime_ticks, process_info_.prev_values.stime_ticks);

    float time_delta = (float)system_info_.uptime - (float)system_info_.prev_uptime;
    float process_delta = total_time - prev_time;
    float utilization = process_delta / time_delta;
    return utilization;
}

// Return the command that generated this process
string Process::Command() { return process_info_.values.command; }

// Return this process's memory utilization
string Process::Ram()
{
    if ((unsigned long) process_info_.values.vm_size<MB_KB) {
        return to_string(process_info_.values.vm_size)+" KB";
    }
    return to_string(process_info_.values.vm_size/MB_KB)+" MB";
}

// Return the user (name) that generated this process
string Process::User() { return process_info_.values.user; }

// Return the age of this process (in seconds)
long int Process::UpTime() const
{
    return system_info_.uptime-
            (process_info_.values.starttime_ticks/sysconf(_SC_CLK_TCK));
}

// the provided process as being less than this if it has lower cpu utilization
bool Process::operator<(Process const& a) const
{
    return a.CpuUtilization()<this->CpuUtilization();
}

Process::Process(Process::SystemInfo system_info,
        Process::ProcessInfo process_info)
        :system_info_(system_info), process_info_(std::move(process_info)) { }
