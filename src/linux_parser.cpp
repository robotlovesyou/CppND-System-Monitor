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

// Split the provided string into parts delimited by a given character
vector<string> SplitString(const string &str, char delim) {
    vector<string> result{};
    string part;
    std::stringstream source(str);
    while (std::getline(source, part, delim)) {
        result.push_back(part);
    }
    return result;
}

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

// Return a map of user names by user id
map<string, string> LinuxParser::NameById() {
    map<string, string> result{};
    auto line_processor = [&](string &line) -> bool {
        vector<string> parts = SplitString(line, ':');
        if (parts.size() >= 3) {
            result[parts[0]] = parts[1];
        }
        return true;
    };
    ProcessFileLines(kPasswordPath, line_processor);
    return result;
}


// Returns a string describing the operating system
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
                return false; // we're done so stop processing
            }
        }
        return true;
    };
    ProcessFileLines(kOSPath, line_processor);
    return value;
}

// Returns a string describing the kernel
string LinuxParser::Kernel() {
    string os, kernel, version;
    auto line_processor = [&](string &line) -> bool {
        std::istringstream linestream(line);
        linestream >> os >> version >> kernel;
        return false; // we only need to read one line
    };
    ProcessFileLines(kProcDirectory + kVersionFilename, line_processor);
    return kernel;
}

// BONUS: Update this to use std::filesystem
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


// Fills out the provided MemoryValues with values parsed from /proc/meminfo;
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

// Read and return the system uptime
long LinuxParser::UpTime() {
    double uptime, idle_time;
    auto line_processor = [&](string &line) -> bool {
        std::istringstream line_stream(line);
        line_stream >> uptime >> idle_time;
        std::cout << uptime << std::endl;
        return false; // done after the first line
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

// Fills out the provided CPUValues structure with values parsed from /proc/stat
void LinuxParser::CpuUtilization(CPUValues &values) {
    auto line_processor = [&](string &line) -> bool {
        string cpu;
        std::istringstream line_stream(line);
        line_stream >> cpu >> values.user >> values.nice >> values.system >>
                    values.idle >> values.io_wait >> values.irq >> values.soft_irq >>
                    values.steal >> values.guest >> values.guest_nice;
        return false; // we only need the first line
    };
    ProcessFileLines(kProcDirectory + kStatFilename, line_processor);
}

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

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
    return ProcessCount(kProcDirectory + kStatFilename, "processes");
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
    return ProcessCount(kProcDirectory + kStatFilename, "procs_running");
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }