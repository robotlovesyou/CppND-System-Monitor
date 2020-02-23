#include "format.h"
#include <string>

using std::string;
using std::to_string;

string FormatPart(long part) {
  if (part < 10) {
    return "0" + to_string(part);
  }
  return to_string(part);
}

string Format::ElapsedTime(long seconds) {
  long hours = seconds / 3600;
  seconds -= hours * 3600;
  long minutes = seconds / 60;
  seconds -= minutes * 60;
  return FormatPart(hours) + ":" + FormatPart(minutes) + ":" +
         FormatPart(seconds);
}