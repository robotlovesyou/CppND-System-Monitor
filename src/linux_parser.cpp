#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::map;
using std::stof;
using std::string;
using std::to_string;
using std::vector;

/**
 * Offsets of various desired values in a process stat file
 */
static const unsigned int kUtime = 13;
static const unsigned int kStime = 14;
static const unsigned int kCutime = 15;
static const unsigned int kCstime = 16;
static const unsigned int kStartTime = 21;

/**
 * Module private method declarations
 */
vector<string> SplitString(const string &str, char delim);

vector<string> SplitLine(const string &str);

int ProcessCount(const string &file_path, const string &desired_key);

void ProcessFileLines(const string &path, std::function<bool(string &)> f);

long StoLSafe(const string &from);

string ReadCommandFile(const string &path);

void ParseProcStatFile(const string &path, LinuxParser::ProcessValues &values);

void ParseProcStatusFile(const string &path,
                         LinuxParser::ProcessValues &values);

static inline void rtrim(std::string &s);

int StoISafe(const string &from);

/**
 * Split the provided string into parts delimited by a given delimiter
 * @param str
 * @param delim
 * @return
 */
vector<string> SplitString(const string &str, char delim) {
  vector<string> result{};
  string part;
  std::stringstream source(str);
  while (std::getline(source, part, delim)) {
    result.push_back(part);
  }
  return result;
}

/**
 * Split the provided string into parts
 * @param str
 * @return
 */
vector<string> SplitLine(const string &str) {
  vector<string> result{};
  string part;
  std::stringstream source(str);
  while (source >> part) {
    result.push_back(part);
  }
  return result;
}

/**
 * Collect the desired process count value
 * @param file_path
 * @param desired_key
 * @return
 */
int ProcessCount(const string &file_path, const string &desired_key) {
  int value;
  auto line_processor = [&](string &line) -> bool {
    std::istringstream line_stream(line);
    string key;
    return !((line_stream >> key >> value) && key == desired_key);
  };
  ProcessFileLines(file_path, line_processor);
  return value;
}

/**
 * Open the required file and then read each line in turn
 * and pass it to the provided lambda until said lambda returns false
 * or the file ends.
 * @param path
 * @param f
 */
void ProcessFileLines(const string &path, std::function<bool(string &)> f) {
  string line;
  std::ifstream file_stream(path);
  if (!file_stream.is_open()) {
    return;
  }
  while (std::getline(file_stream, line)) {
    if (!f(line)) {
      break;
    }
  }
  file_stream.close();
}

/**
 * Return a map of user names by user id
 * @return
 */
map<string, string> LinuxParser::NameById() {
  map<string, string> result{};
  auto line_processor = [&](string &line) -> bool {
    vector<string> parts = SplitString(line, ':');
    if (parts.size() >= kUserId) {
      result[parts[kUserId]] = parts[kName];
    }
    return true;
  };
  ProcessFileLines(kPasswordPath, line_processor);
  return result;
}

/**
 * Returns a string describing the operating system
 * @return
 */
string LinuxParser::OperatingSystem() {
  string key;
  string value;
  auto line_processor = [&](string &line) -> bool {
    std::replace(line.begin(), line.end(), ' ', '_');
    std::replace(line.begin(), line.end(), '=', ' ');
    std::replace(line.begin(), line.end(), '"', ' ');
    std::istringstream linestream(line);
    while (linestream >> key >> value) {
      if (key == "PRETTY_NAME") {
        std::replace(value.begin(), value.end(), '_', ' ');
        return false;  // we're done so stop processing
      }
    }
    return true;
  };
  ProcessFileLines(kOSPath, line_processor);
  return value;
}

/**
 * Returns a string describing the kernel
 * @return
 */
string LinuxParser::Kernel() {
  string os, kernel, version;
  auto line_processor = [&](string &line) -> bool {
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
    return false;  // we only need to read one line
  };
  ProcessFileLines(kProcDirectory + kVersionFilename, line_processor);
  return kernel;
}

/**
 * Return a vector of integers, one for each running process
 * @return
 */
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR *directory = opendir(kProcDirectory.c_str());
  struct dirent *file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

/**
 * Fills out the provided MemoryValues with values parsed from /proc/meminfo;
 * @param values
 */
void LinuxParser::MemoryUtilization(MemoryValues &values) {
  auto line_processor = [&](string &line) -> bool {
    std::istringstream line_stream(line);
    string key;
    long val;
    line_stream >> key >> val;
    if (key == kMemFree + ":") {
      values.free = val;
    } else if (key == kMemTotal + ":") {
      values.total = val;
    }
    return true;
  };
  ProcessFileLines(kProcDirectory + kMeminfoFilename, line_processor);
}

/**
 * Read and return the system uptime
 * @return
 */
long LinuxParser::UpTime() {
  double uptime, idle_time;
  auto line_processor = [&](string &line) -> bool {
    std::istringstream line_stream(line);
    line_stream >> uptime >> idle_time;
    return false;  // done after the first line
  };
  ProcessFileLines(kProcDirectory + kUptimeFilename, line_processor);
  return (long)uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

/**
 * Fills out the provided CPUValues structure with values parsed from /proc/stat
 * @param values
 */
void LinuxParser::CpuUtilization(CPUValues &values) {
  auto line_processor = [&](string &line) -> bool {
    string cpu;
    std::istringstream line_stream(line);
    line_stream >> cpu >> values.user >> values.nice >> values.system >>
        values.idle >> values.io_wait >> values.irq >> values.soft_irq >>
        values.steal >> values.guest >> values.guest_nice;
    return false;  // we only need the first line
  };
  ProcessFileLines(kProcDirectory + kStatFilename, line_processor);
}

/**
 * Read and return the total number of processes
 * @return
 */
int LinuxParser::TotalProcesses() {
  return ProcessCount(kProcDirectory + kStatFilename, "processes");
}

/**
 * Read and return the number of running processes
 * @return
 */
int LinuxParser::RunningProcesses() {
  return ProcessCount(kProcDirectory + kStatFilename, "procs_running");
}

/**
 * Convert from string to long, returning 0 on exception
 * @param from
 * @return
 */
long StoLSafe(const string &from) {
  try {
    return std::stol(from);
  } catch (...) {
    return 0;
  }
}

/**
 * Convert from string to int, returning 0 on exception
 * @param from
 * @return
 */
int StoISafe(const string &from) {
  try {
    return std::stoi(from);
  } catch (...) {
    return 0;
  }
}

/**
 * Read desired values from a processes status file into the provided
 * ProcessValues
 * @param path
 * @param values
 */
void ParseProcStatusFile(const string &path,
                         LinuxParser::ProcessValues &values) {
  auto line_processor = [&](string &line) -> bool {
    string key, value;
    std::istringstream line_stream(line);
    line_stream >> key >> value;
    if (key == "Uid:") {
      values.user_id = value;
    } else if (key == "VmSize:") {
      values.vm_size = StoLSafe(value);
    }
    return true;
  };
  ProcessFileLines(path, line_processor);
}

/**
 * Read and return the command associated with a process
 * @param path
 * @return
 */
string ReadCommandFile(const string &path) {
  string command;
  auto line_processor = [&](string &line) -> bool {
    command = line;
    return false;  // exit after the first line
  };
  ProcessFileLines(path, line_processor);
  return command;
}

/**
 * Parse desired values from a process stat file into the provided ProcessValues
 * @param path
 * @param values
 */
void ParseProcStatFile(const string &path, LinuxParser::ProcessValues &values) {
  auto line_processor = [&](string &line) -> bool {
    vector<string> parts = SplitLine(line);
    if (parts.size() >= kStartTime) {
      values.utime_ticks = StoLSafe(parts[kUtime]);
      values.stime_ticks = StoLSafe(parts[kStime]);
      values.cutime_ticks = StoLSafe(parts[kCutime]);
      values.cstime_ticks = StoLSafe(parts[kCstime]);
      values.starttime_ticks = StoLSafe(parts[kStartTime]);
    }
    return false;  // exit after the first line
  };
  ProcessFileLines(path, line_processor);
}

/**
 * Create a vector of filled out process values. One for each process.
 * @return
 */
vector<LinuxParser::ProcessValues> LinuxParser::ProcessValuesList() {
  vector<int> pids = Pids();
  vector<ProcessValues> values_list{};
  map<string, string> users = NameById();

  for (auto pid : pids) {
    string path_base = kProcDirectory + std::to_string(pid) + "/";
    ProcessValues values{};
    values.pid = pid;

    ParseProcStatusFile(path_base + kStatusFilename, values);
    ParseProcStatFile(path_base + kStatFilename, values);
    values.user = users[values.user_id];

    values.command = ReadCommandFile(path_base + kCmdlineFilename);

    values_list.push_back(values);
  }
  return values_list;
}

/**
 * trim from end (in place). Taken from stack overflow answer:
 * https://stackoverflow.com/a/217605
 * @param s
 */
static inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](int ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

/**
 * Count the total nunber of CPU cores
 * @return
 */
int LinuxParser::CountCores() {
  int count = 0;
  auto line_processor = [&](string &line) -> bool {
    vector<string> parts = SplitString(line, ':');
    if (parts.size() == 2) {
      string key = parts[0];
      rtrim(key);
      if (key == "cpu cores") {
        count += StoISafe(parts[1]);
      }
    }
    return true;
  };
  ProcessFileLines(LinuxParser::kProcDirectory + LinuxParser::kCpuinfoFilename,
                   line_processor);
  return count;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }