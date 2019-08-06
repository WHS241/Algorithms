#ifndef GRAPHIC_CPP
#define GRAPHIC_CPP

#include <algorithm>
#include <iterator>
#include <typeinfo>

template<typename It>
Induction::Skyline Induction::generateSkyline(It first, It last) {
    Skyline result;
    if (first == last) {
        return result;
    }
    auto temp(first);
    if (++temp == last) {
        result.leftEnd = first->leftIndex;
        result.section.push_back(std::make_pair(first->height, first->rightIndex));
        return result;
    }

    It mid;
    if (typeid(typename std::iterator_traits<It>::iterator_category) == typeid(std::random_access_iterator_tag)) {
        mid = first + (last - first) / 2;
    }
    else {
        uint32_t dist = std::distance(first, last);
        mid = first;
        std::advance(mid, dist / 2);
    }

    Skyline subproblemA = generateSkyline(first, mid);
    Skyline subproblemB = generateSkyline(mid, last);

    if (subproblemB.leftEnd < subproblemA.leftEnd)
        std::swap(subproblemA, subproblemB);

    auto it = std::find_if(subproblemA.section.begin(), subproblemA.section.end(), [&subproblemB](auto section) {
        return section.second >= subproblemB.leftEnd;
    });

    if (it == subproblemA.section.end()) {
        subproblemA.section.push_back(std::make_pair(0, subproblemB.leftEnd));
        subproblemA.section.splice(subproblemA.section.end(), subproblemB.section);
        return subproblemA;
    }

    result.leftEnd = subproblemA.leftEnd;
    result.section.splice(result.section.end(), subproblemA.section, subproblemA.section.begin(), it);
    auto it2 = subproblemB.section.begin();
    double currentLeft = subproblemB.leftEnd;
    result.section.push_back(std::make_pair(it->first, currentLeft));

    while (it != subproblemA.section.end() && it2 != subproblemB.section.end()) {
        if (it->first < it2->first) {
            if (it->second < it2->second) {
                result.section.push_back(std::make_pair(it2->first, it->second));
                currentLeft = it->second;
                ++it;
            }
            else {
                auto temp = it2;
                ++temp;
                result.section.splice(result.section.end(), subproblemB.section, it2, temp);
                currentLeft = it2->second;
                it2 = temp;
            }
        }
        else {
            if (it->second < it2->second) {
                auto temp = it;
                ++temp;
                result.section.splice(result.section.end(), subproblemA.section, it, temp);
                currentLeft = it->second;
                it = temp;
            }
            else {
                result.section.push_back(std::make_pair(it->first, it2->second));
                currentLeft = it2->second;
                ++it2;
            }
        }
    }

    if (it != subproblemA.section.end()) {
        result.section.splice(result.section.end(), subproblemA.section, it);
    }
    else if (it2 != subproblemB.section.end()) {
        result.section.splice(result.section.end(), subproblemB.section, it2);
    }

    // one more pass to consolidate
    for (auto it = result.section.begin(); it != result.section.end();) {
        auto it2(it);
        ++it2;
        if (it2 == result.section.end()) {
            if (it->first == 0)
                result.section.erase(it);

            break;
        }

        if (it->second == it2->second)
            result.section.erase(it2);
        else if (it->first == it2->first)
            it = result.section.erase(it);
        else
            ++it;
    }

    return result;
}

#endif // GRAPHIC_CPP