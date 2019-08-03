#ifndef SUBSEQUENCE_CPP
#define SUBSEQUENCE_CPP

#include <algorithm>

template<typename It1, typename It2>
bool Subsequence::isSubsequence(It1 targetFirst, It1 targetLast, It2 masterFirst, It2 masterLast) {
    for (; targetFirst != targetLast; ++targetFirst, ++masterFirst) {
        masterFirst = std::find(masterFirst, masterLast, *targetFirst);
        if (masterFirst == masterLast)
            return false;
    }
    return true;
}

template<typename It1, typename It2>
uint32_t Subsequence::maxStutter(It1 targetFirst, It1 targetLast, It2 masterFirst, It2 masterLast) {
    uint32_t lower = 0, upper = std::distance(masterFirst, masterLast)
                              / std::distance(targetFirst, targetLast);

    while (lower < upper) {
        uint32_t mid = (lower + 1 == upper) ? upper : (lower + upper) / 2;
        StutterIterator<It1> stutterFirst(targetFirst, mid), stutterLast(targetLast, mid);
        if (isSubsequence(stutterFirst, stutterLast, masterFirst, masterLast))
            lower = mid;
        else
            upper = mid - 1;
    }
    return lower;
}

#endif // !SUBSEQUENCE_CPP
