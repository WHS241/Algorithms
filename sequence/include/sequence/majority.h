#ifndef SEQUENCE_MAJORITY_H
#define SEQUENCE_MAJORITY_H
#include <iterator>

namespace sequence {
template <typename It>
typename std::iterator_traits<It>::value_type find_majority(It first, It last);
}

#include <src/sequence/majority.cpp>

#endif // SEQUENCE_MAJORITY_H