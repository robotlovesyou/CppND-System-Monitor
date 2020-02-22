#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <map>
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

/**
 * Fills out the provided MemoryValues with values parsed from /proc/meminfo;
 * @param values
 */
void MemoryUtilization(MemoryValues &values);

/**
 * Read and return the system uptime
 * @return
 */
long UpTime();

/**
 * Return a vector of integers, one for each running process
 * @return
 */
std::vector<int> Pids();

int TotalProcesses();

int RunningProcesses();

/**
 * Returns a string describing the operating system
 * @return
 */
std::string OperatingSystem();

/**
 * Returns a string describing the kernel
 * @return
 */
std::string Kernel();

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

/**
 * Fills out the provided CPUValues structure with values parsed from /proc/stat
 * @param values
 */
void CpuUtilization(CPUValues &values);

long Jiffies();

long ActiveJiffies();

long ActiveJiffies(int pid);

long IdleJiffies();

// Processes
enum PasswdParts {
  kName = 0,
  kPassword,
  kUserId,
  kGroupId,
  kGecos,
  kHomeDir,
  kShell
};

struct ProcessValues {
 public:
  int pid{};
  std::string user_id{};
  std::string user{};
  long vm_size{};
  long utime_ticks;
  long stime_ticks;
  long cutime_ticks;
  long cstime_ticks;
  long starttime_ticks;
  std::string command{};
};

/**
 * Create a vector of filled out process values. One for each process.
 * @return
 */
std::vector<ProcessValues> ProcessValuesList();


std::string User(int pid);

/**
 * Return a map of user names indexed by user id
 * @return
 */
std::map<std::string, std::string> NameById();

long int UpTime(int pid);
};  // namespace LinuxParser

#endif