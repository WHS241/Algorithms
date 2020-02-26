#ifndef STRING_COMP_H
#define STRING_COMP_H
#include <string>
#include <vector>

namespace sequence {
/*
Determine if target is a substring of input
Returns iterator to start if yes, end of input if no

Donald Knuth, James Morris Jr., Vaughan Pratt
Fast pattern matching in strings
(1977) doi:10.1137/0206024

O(m+n)
*/
template <typename CharT>
typename std::basic_string<CharT>::const_iterator find_substring(
    const std::basic_string<CharT>& input, const std::basic_string<CharT>& target);

struct instruction {
    enum category { insert_char, delete_char, replace_char };

    category directive;
    uint32_t src_index; // Delete or Replace
    uint32_t target_index; // Insert or Replace
};

/*
Returns the Levenshtein distance from src to target
(edit distance, where Insert, Delete, and Replace are unit-cost operations)
Taras Vintsyuk
Speech discrimination by dynamic programming
(1968) Cybernetics
Robert Wagner, Michael Fischer (1974)
O(mn)
*/
template <typename CharT>
std::vector<instruction> Levenshtein_distance(
    const std::basic_string<CharT>& src, const std::basic_string<CharT>& target);
} // namespace Sequence

#include "src/sequence/sequence_comparison.cpp"

#endif // !STRING_COMP_H
