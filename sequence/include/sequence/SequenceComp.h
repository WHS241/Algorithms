#ifndef STRING_COMP_H
#define STRING_COMP_H
#include <string>
#include <vector>

namespace Sequence {
    /*
    Determine if target is a substring of input
    Returns iterator to start if yes, end of input if no
    Donald Knuth, James Morris Jr., Vaughan Pratt (1977)
    O(m+n)
    */
    template<typename CharT>
    typename std::basic_string<CharT>::const_iterator findSubstring(const std::basic_string<CharT>& input, const std::basic_string<CharT>& target);

    struct Instruction {
        enum Category {Insert, Delete, Replace};

        Category directive;
        uint32_t srcIndex; // Delete or Replace
        uint32_t targetIndex; // Insert or Replace
    };

    /*
    Returns the shortest edit distance from src to target
    Robert Wagner, Michael Fischer (1974)
    O(mn)
    */
    template<typename CharT>
    std::vector<Instruction> editDistance(const std::basic_string<CharT>& src, const std::basic_string<CharT>& target);
}

#include "../../src/sequence/SequenceComp.cpp"

#endif // !STRING_COMP_H
