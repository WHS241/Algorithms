#ifndef NON_COMPARE_SORT_CPP
#define NON_COMPARE_SORT_CPP

#include <algorithm>
#include <cstdint>
#include <list>
#include <typeinfo>
#include <vector>

template <typename It> void sequence::radix_sort(It first, It last, uint32_t base)
{
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

#endif // NON_COMPARE_SORT_CPP