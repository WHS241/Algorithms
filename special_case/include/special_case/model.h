#ifndef MODEL_BASED_SPECIAL_CASE
#define MODEL_BASED_SPECIAL_CASE

#include <iterator>
#include <vector>

#include <sequence/compare_sort.h>
#include <sequence/subsequence.h>

#include <structures/van_Emde_Boas_map.h>

// Special cases for mostly NP-complete problems
// This file contains model-based algorithms (needs proof the input is an instance of special case)
namespace special_case {
/*
 * Max clique in a circle graph
 *
 * Input: A set of chords in a circle, inputted as a pair of coordinates representing the endpoints
 * (example: vertical line is (0, 180), horizontal (90, 270))
 * Output: The maximum set of mutually intersecting chords, or rather the index of each
 * (example: if the first and eighth chords given are in the set, 0 and 7 are in the output)
 *
 * Preconditions:
 * No chords share endpoints (undefined behavior)
 * Coordinates are based on first revolution around circle (i.e. if inputted as degrees, must be <
 * 360, if as radians, < 2pi, if as gradians, < 400)
 *
 * O(n^2 log log n)
 */
template<typename It> std::vector<std::size_t> model_max_clique_circle_graph(It first, It last) {
    typedef typename std::iterator_traits<It>::value_type::first_type coord;

    struct chord {
        coord first;
        coord second;
        std::size_t pos;

        bool operator==(const chord& rhs) const {
            return first == rhs.first && second == rhs.second;
        }
    };

    std::vector<chord> doubled_chords;
    std::vector<std::size_t> matcher;
    std::vector<std::vector<std::size_t>> subresults;

    std::size_t num_chords = std::distance(first, last);
    doubled_chords.reserve(2 * num_chords);
    matcher.reserve(2 * num_chords);
    subresults.reserve(2 * num_chords);

    auto chord_compare = [](const chord& x, const chord& y) {
        return x.first < y.first;
    };

    // Double count all edges, as we need to track endpoints separately
    for (std::size_t i = 0; first != last; ++first) {
        doubled_chords.push_back({first->first, first->second, i});
        doubled_chords.push_back({first->second, first->first, i++});
    }

    // Order the endpoints, then match each endpoint to the index of its partner
    sequence::mergesort(doubled_chords.begin(), doubled_chords.end(), chord_compare);
    for (const chord& c : doubled_chords) {
        chord complement = {c.second, c.first, c.pos};
        auto complement_it = sequence::binary_search(doubled_chords.begin(), doubled_chords.end(),
                                                     complement, chord_compare);
        matcher.push_back(complement_it - doubled_chords.begin());
    }

    // For each chord-vertex, find the maximum clique that contains that vertex
    // Process on the first chord in matcher, then perform cyclic shifts
    for (std::size_t i = 0; i < doubled_chords.size(); ++i) {
        std::vector<std::size_t> filtered_matches;
        std::size_t limit = matcher.front();

        // Filter to get only intersecting chords: index of current are (0, x), and other is (y, z),
        // then they intersect iff 0 < y < x < z
        std::copy_if(matcher.begin(), matcher.begin() + limit, std::back_inserter(filtered_matches),
                     [&limit](std::size_t x) { return x >= limit; });

        // Generalizing the above to multiple chords (y1, z1), (y2, z2), ..., 0 < y_1 < ... < y_n <
        // x < z_1 < ... < z_n y-values are just the index in matcher, so just need to find the
        // longest increasing sequence of z-values (values stored in matcher)
        auto clique_on_current_gen = sequence::longest_increasing_integer_subsequence(
          filtered_matches.begin(), filtered_matches.end());
        std::vector<std::size_t> subresult(clique_on_current_gen.size());
        std::transform(clique_on_current_gen.cbegin(), clique_on_current_gen.cend(),
                       subresult.begin(),
                       [&doubled_chords, &i](const std::vector<std::size_t>::const_iterator& it) {
                           // need to account for cyclic shift
                           return doubled_chords[(*it + i) % doubled_chords.size()].pos;
                       });
        subresults.push_back(subresult);

        // Cyclic shift: need to account for shift affecting index of other endpoint: change stored values
        auto replace = [&matcher](std::size_t x) {
            return (x == 0) ? (matcher.size() - 1) : (x - 1);
        };
        std::size_t temp = matcher.front();
        std::transform(matcher.begin() + 1, matcher.end(), matcher.begin(), replace);
        matcher.back() = replace(temp);
    }

    return *std::max_element(subresults.begin(), subresults.end(),
                             [](const std::vector<std::size_t>& x,
                                const std::vector<std::size_t>& y) { return x.size() < y.size(); });
}
} // namespace special_case

#endif // MODEL_BASED_SPECIAL_CASE
