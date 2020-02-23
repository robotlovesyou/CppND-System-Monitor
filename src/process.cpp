#include "process.h"
#include <unistd.h>
#include <string>
#include <utility>

using std::string;
using std::to_string;
using std::vector;

/**
 * convert the provided ticks into seconds
 * @param utime
 * @param stime
 * @return
 */
float process_secs(long utime, long stime) {
  return (float)(utime + stime) / (float)sysconf(_SC_CLK_TCK);
}

int Process::Pid() const { return process_values_.pid; }

float Process::CpuUtilization() const { return utilization_; }

string Process::Command() { return process_values_.command; }

string Process::Ram() {
  if ((unsigned long)process_values_.vm_size < MB_KB) {
    return to_string(process_values_.vm_size) + " KB";
  }
  return to_string(process_values_.vm_size / MB_KB) + " MB";
}

string Process::User() { return process_values_.user; }

long int Process::UpTime() const {
  return uptime_ - (process_values_.starttime_ticks / sysconf(_SC_CLK_TCK));
}

bool Process::operator<(Process const& a) const {
  return a.CpuUtilization() < this->CpuUtilization();
}

Process::Process(long uptime, ProcessValues process_values)
    : uptime_(uptime),
      prev_uptime_{},
      process_values_(std::move(process_values)),
      prev_process_values_{} {
  // This first call will calculate the utilization since
  // System start since the time delta will be 1.0
  UpdateUtilization();
}
void Process::Update(long uptime, ProcessValues process_values) {
  prev_uptime_ = uptime_;
  uptime_ = uptime;
  prev_process_values_ = process_values_;
  process_values_ = std::move(process_values);
  // This will calculate the utilization since the last update
  UpdateUtilization();
}

void Process::UpdateUtilization() {
  float total_time =
      process_secs(process_values_.utime_ticks, process_values_.stime_ticks);
  float prev_time = process_secs(prev_process_values_.utime_ticks,
                                 prev_process_values_.stime_ticks);

  float time_delta = std::max((float)uptime_ - (float)prev_uptime_, 1.0f);

  float process_delta = total_time - prev_time;
  utilization_ = process_delta / time_delta;
}
