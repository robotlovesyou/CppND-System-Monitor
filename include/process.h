#ifndef PROCESS_H
#define PROCESS_H

#include <string>

#include "linux_parser.h"

using LinuxParser::ProcessValues;

/**
 * Class to represent a process on the system
 */
class Process {
 public:
  /**
   * Construct a new process
   * @param uptime The current system uptime in seconds
   * @param process_values process values for this process
   */
  Process(long uptime, ProcessValues process_values);

  /**
   * Update this process with new values. Will re-calculate the
   * process cpu utilization
   * @param uptime
   * @param process_values
   */
  void Update(long uptime, ProcessValues process_values);

  /**
   * The process id of this process
   * @return
   */
  int Pid() const;

  /**
   * The name of the user which owns this process
   * @return
   */
  std::string User();

  /**
   * The command line used to start this process
   * @return
   */
  std::string Command();

  /**
   * The current CPU Utilization of this process
   * @return
   */
  float CpuUtilization() const;

  /**
   * The current RAM used by this process
   * @return
   */
  std::string Ram();

  /**
   * The uptime of this process in seconds
   * @return
   */
  long int UpTime() const;

  /**
   * Does this process have a lower cpu utilization than the
   * process a?
   * @param a
   * @return
   */
  bool operator<(Process const& a) const;

  /**
   * Ratio between MB and KB
   */
  static const unsigned long MB_KB = 0x1ul << 10ul;

 private:
  /**
   * Update the cpu utilization.
   * As CPU Utilization is used by the < method during sorting
   * it is requested a lot and so is pre-calculated.
   */
  void UpdateUtilization();

  /**
   * The current uptime of the processs
   */
  long uptime_{};

  /**
   * The previous uptime of the process.
   */
  long prev_uptime_{};

  /**
   * The current process values of the process
   */
  ProcessValues process_values_{};

  /**
   * The previous process values of the process
   */
  ProcessValues prev_process_values_{};

  /**
   * The current CPU Utilization of the process
   */
  float utilization_{};
};

#endif