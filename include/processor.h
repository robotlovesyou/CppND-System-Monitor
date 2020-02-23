#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"

using LinuxParser::CPUValues;

/**
 * Class to represent the state of the system's aggregated
 * CPU data
 */
class Processor {
 public:
  /**
   * The system's current aggregated CPU Utilization
   * Calculations made according to this Stack Overflow answer:
   * https://stackoverflow.com/a/23376195
   * @return
   */
  float Utilization();

  /**
   * Calculate the number of idle ticks
   * @param values
   * @return
   */
  static long CPUIdle(CPUValues& values);

  /**
   * Calculate the number of busy ticks
   * @param values
   * @return
   */
  static long CPUBusy(CPUValues& values);

 private:
  /**
   * The previous CPU Values. Used to calculate a delta
   * for the current utilization
   */
  CPUValues prev_values_{};
};

#endif