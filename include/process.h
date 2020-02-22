#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include "linux_parser.h"

using LinuxParser::ProcessValues;

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
public:
    struct SystemInfo {
    public:
        long uptime{};
        long prev_uptime{};
    };

    struct ProcessInfo {
    public:
        ProcessValues values{};
        ProcessValues prev_values{};
    };

    Process(SystemInfo system_info, ProcessInfo process_info);

    int Pid() const;                         // TODO: See src/process.cpp
    std::string User();                      // TODO: See src/process.cpp
    std::string Command();                   // TODO: See src/process.cpp
    float CpuUtilization() const;            // TODO: See src/process.cpp
    std::string Ram();                       // TODO: See src/process.cpp
    long int UpTime() const;                 // TODO: See src/process.cpp
    bool operator<(Process const& a) const;  // TODO: See src/process.cpp
    static const unsigned long MB_KB = 0x1ul << 10ul;

    // TODO: Declare any necessary private members
private:
    SystemInfo system_info_;
    ProcessInfo process_info_;
};

#endif