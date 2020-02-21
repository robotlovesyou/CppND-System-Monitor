#include <algorithm>
#include "processor.h"


// Calculates Current CPU Utilization from the system aggregate.
// Calculations made according to this Stack Overflow answer https://stackoverflow.com/a/23376195
float Processor::Utilization() {
    CPUValues values;
    CpuUtilization(values);
    long idle = CPUIdle(values);
    long prev_idle = CPUIdle(prev_values_);

    long busy = CPUBusy(values);
    long prev_busy = CPUBusy(prev_values_);

    long total = idle + busy;
    long prev_total = prev_idle + prev_busy;

    long total_delta = total - prev_total;
    long idle_delta = idle - prev_idle;

    prev_values_ = values;
    return (float)(total_delta - idle_delta) / std::max((float)total_delta, 1.0f);
}

long Processor::CPUIdle(CPUValues &values) {
    return values.idle + values.io_wait;
}

long Processor::CPUBusy(CPUValues &values) {
    return values.user + values.nice + values.system + values.irq + values.soft_irq + values.steal;
}
