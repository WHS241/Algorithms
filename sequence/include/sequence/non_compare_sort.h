#ifndef NON_COMPARE_SORT_H
#define NON_COMPARE_SORT_H

#include <algorithm>
#include <cstdint>
#include <functional>
#include <list>
#include <vector>

namespace sequence {
/*
 Radix sort, customizable base
 Herman Hollerth (1887)
 Time: O(n log_base(max)) where n is distance(first, last), max is largest
 element Space: O(n + base)
 */
template<typename It> void radix_sort(It first, It last, uint32_t base) {
    std::list<uint32_t> processor(first, last);
    std::vector<std::list<uint32_t>> bins(base);

    uint32_t max = *std::max_element(first, last);
    for (uint32_t place = 1; place <= max; place *= base) {
        // value / place % base gives the least significant unprocessed digit in
        // value in the base
        while (!processor.empty()) {
            uint32_t current = *processor.begin();
            auto& destList = bins[(current / place) % base];
            destList.splice(destList.end(), processor, processor.begin(),
                            ++processor.begin()); // process one at a time
        }

        for (auto& fullBin : bins) {
            processor.splice(processor.end(), fullBin);
        }
    }

    std::copy(processor.begin(), processor.end(), first);
}
} // namespace sequence

#endif // NON_COMPARE_SORT_H
