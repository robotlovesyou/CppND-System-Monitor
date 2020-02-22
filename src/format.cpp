#include <string>
#include "format.h"

using std::string;
using std::to_string;

// Convert the input to a string padded to length 2
string FormatPart(long part) {
    if (part < 10) {
        return "0" + to_string(part);
    }
    return to_string(part);
}

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    long hours = seconds / 3600;
    seconds -= hours * 3600;
    long minutes = seconds / 60;
    seconds -= minutes * 60;
    return FormatPart(hours) + ":" + FormatPart(minutes) + ":" + FormatPart(seconds);
}