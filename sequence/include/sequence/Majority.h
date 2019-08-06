#ifndef SEQUENCE_MAJORITY_H
#define SEQUENCE_MAJORITY_H
#include <iterator>

namespace Sequence {
    template <typename It>
    typename std::iterator_traits<It>::value_type findMajority(It first, It last);
}

#include <src/sequence/Majority.cpp>

#endif // SEQUENCE_MAJORITY_H