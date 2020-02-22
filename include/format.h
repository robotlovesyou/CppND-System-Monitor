#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
    /**
     * Formats the provided time in seconds as an HH:MM:SS string
     * @param times
     * @return
     */
    std::string ElapsedTime(long times);
};                                    // namespace Format

#endif