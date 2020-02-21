#ifndef PROCESSOR_H
#define PROCESSOR_H
#include "linux_parser.h"

using LinuxParser::CPUValues;

class Processor {
 public:
  float Utilization();

 private:
  CPUValues prev_values_{};
  static long CPUIdle(CPUValues &values);
  static long CPUBusy(CPUValues &values);
};

#endif