#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() const { return process_vals_.pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return 0; }

// TODO: Return the command that generated this process
string Process::Command() { return string(); }

// TODO: Return this process's memory utilization
string Process::Ram() { return string(); }

// TODO: Return the user (name) that generated this process
string Process::User() { return process_vals_.user; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return 0; }

// TODO: Overload the "less than" comparison operator for Process objects in a useful way!
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { return a.Pid() < process_vals_.pid; }

Process::Process(ProcessValues pv): process_vals_(std::move(pv)) {}
