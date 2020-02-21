#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// System
// Key constants
const std::string kMemTotal{"MemTotal"};
const std::string kMemFree{"MemFree"};
// Container for Memory Utilization values
struct MemoryValues {
 public:
  long total{};
  long free{};
};

void MemoryUtilization(MemoryValues &values);

long UpTime();

std::vector<int> Pids();

int TotalProcesses();

int RunningProcesses();

std::string OperatingSystem();

std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};

// Container for CPU Utilization values
struct CPUValues {
 public:
  long user{};
  long nice{};
  long system{};
  long idle{};
  long io_wait{};
  long irq{};
  long soft_irq{};
  long steal{};
  long guest{};
  long guest_nice{};
};

void CpuUtilization(CPUValues &values);

long Jiffies();

long ActiveJiffies();

long ActiveJiffies(int pid);

long IdleJiffies();

// Processes
std::string Command(int pid);

std::string Ram(int pid);

std::string Uid(int pid);

std::string User(int pid);

long int UpTime(int pid);
};  // namespace LinuxParser

#endif