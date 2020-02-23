#include "linux_parser.h"

#include <dirent.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::istringstream;
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
static const unsigned int kStartTime = 21;

/**
 * Split the provided string into parts delimited by a given delimiter
 * @param str
 * @param delim
 * @return
 */
vector<string> SplitString(const string &str, char delim);

/**
 * Split the provided string into parts
 * @param str
 * @return
 */
vector<string> SplitLine(const string &str);

/**
 * Collect the desired process count value
 * @param file_path
 * @param desired_key
 * @return
 */
int ProcessCount(const string &file_path, const string &desired_key);

/**
 * Open the required file and then read each line in turn
 * and pass it to the provided lambda until said lambda returns false
 * or the file ends.
 * @param path
 * @param f
 */
void ProcessFileLines(const string &path,
                      const std::function<bool(string &)> &f);
void ProcessFileLines(const string &path,
                      const std::function<bool(istringstream &)> &f);

/**
 * Convert from string to long, returning 0 on exception
 * @param from
 * @return
 */
long StoLSafe(const string &from);

/**
 * Read and return the command associated with a process
 * @param path
 * @return
 */
string ReadCommandFile(const string &path);

/**
 * Parse desired values from a process stat file into the provided ProcessValues
 * @param path
 * @param values
 */
void ParseProcStatFile(const string &path, LinuxParser::ProcessValues &values);

/**
 * Read desired values from a processes status file into the provided
 * ProcessValues
 * @param path
 * @param values
 */
void ParseProcStatusFile(const string &path,
                         LinuxParser::ProcessValues &values);

vector<string> SplitString(const string &str, char delim) {
  vector<string> result{};
  string part;
  std::stringstream source(str);
  while (std::getline(source, part, delim)) {
    result.push_back(part);
  }
  return result;
}

vector<string> SplitLine(const string &str) {
  vector<string> result{};
  string part;
  std::stringstream source(str);
  while (source >> part) {
    result.push_back(part);
  }
  return result;
}

int ProcessCount(const string &file_path, const string &desired_key) {
  int value;
  auto line_processor = [&](istringstream &line_stream) -> bool {
    string key;
    return !((line_stream >> key >> value) && key == desired_key);
  };
  ProcessFileLines(file_path, line_processor);
  return value;
}

void ProcessFileLines(const string &path,
                      const std::function<bool(string &)> &f) {
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

void ProcessFileLines(const string &path,
                      const std::function<bool(istringstream &)> &f) {
  auto line_processor = [&](string &line) -> bool {
    istringstream line_stream(line);
    return f(line_stream);
  };
  ProcessFileLines(path, line_processor);
}

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

string LinuxParser::OperatingSystem() {
  string key;
  string value;
  auto line_processor = [&](string &line) -> bool {
    std::replace(line.begin(), line.end(), ' ', '_');
    std::replace(line.begin(), line.end(), '=', ' ');
    std::replace(line.begin(), line.end(), '"', ' ');
    istringstream linestream(line);
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

string LinuxParser::Kernel() {
  string os, kernel, version;
  auto line_processor = [&](istringstream &line_stream) -> bool {
    line_stream >> os >> version >> kernel;
    return false;  // we only need to read one line
  };
  ProcessFileLines(kProcDirectory + kVersionFilename, line_processor);
  return kernel;
}

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

void LinuxParser::MemoryUtilization(MemoryValues &values) {
  auto line_processor = [&](istringstream &line_stream) -> bool {
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

long LinuxParser::UpTime() {
  double uptime, idle_time;
  auto line_processor = [&](istringstream &line_stream) -> bool {
    line_stream >> uptime >> idle_time;
    return false;  // done after the first line
  };
  ProcessFileLines(kProcDirectory + kUptimeFilename, line_processor);
  return (long)uptime;
}

void LinuxParser::CpuUtilization(CPUValues &values) {
  auto line_processor = [&](istringstream &line_stream) -> bool {
    string cpu;
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

long StoLSafe(const string &from) {
  try {
    return std::stol(from);
  } catch (...) {
    return 0;
  }
}

void ParseProcStatusFile(const string &path,
                         LinuxParser::ProcessValues &values) {
  auto line_processor = [&](istringstream &line_stream) -> bool {
    string key, value;
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

string ReadCommandFile(const string &path) {
  string command;
  auto line_processor = [&](string &line) -> bool {
    command = line;
    return false;  // exit after the first line
  };
  ProcessFileLines(path, line_processor);
  return command;
}

void ParseProcStatFile(const string &path, LinuxParser::ProcessValues &values) {
  auto line_processor = [&](string &line) -> bool {
    vector<string> parts = SplitLine(line);
    if (parts.size() >= kStartTime) {
      values.utime_ticks = StoLSafe(parts[kUtime]);
      values.stime_ticks = StoLSafe(parts[kStime]);
      values.starttime_ticks = StoLSafe(parts[kStartTime]);
    }
    return false;  // exit after the first line
  };
  ProcessFileLines(path, line_processor);
}

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