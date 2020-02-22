#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"

using LinuxParser::CPUValues;

class Processor {
public:
    float Utilization();

    static long CPUIdle(CPUValues& values);

    static long CPUBusy(CPUValues& values);

private:
    CPUValues prev_values_{};

};

#endif