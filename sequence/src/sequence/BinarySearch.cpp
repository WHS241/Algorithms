#ifndef BINARY_SEARCH_CPP
#define BINARY_SEARCH_CPP

template <typename RandomIt, typename T, typename Compare>
RandomIt Sequence::binarySearch(RandomIt first, RandomIt last, const T &item,
                            Compare comp) {
    if (first == last)
        return first;

    RandomIt noMatch = last;
    while (last - first > 1) {
        RandomIt mid = first + (last - first) / 2;
        if (*mid == item)
            return mid;

        if (comp(item, *mid))
            last = mid;
        else
            first = mid;
    }

    return (*first == item) ? first : noMatch;
}

template <typename RandomIt, typename Function>
RandomIt Sequence::findCutoff(RandomIt first, RandomIt last, Function pred) {
    if (first == last || !pred(*first))
        return first;
    if (pred(*(last - 1)))
        return last;


    while (last - first > 2) {
        RandomIt mid = first + (last - first) / 2;

        if (pred(*(mid++)))
            first = mid;
        else
            last = mid;
    }

    return pred(*first) ? first + 1 : first;
}

template <typename RandomIt, typename Compare>
RandomIt Sequence::cyclicFindMin(RandomIt first, RandomIt last,
                                     Compare comp) {
    if (first == last)
        return first;

    while (last - first > 2) {
        RandomIt mid = first + (last - first) / 2;
        if (comp(*(mid++), *(last - 1)))
            last = mid;
        else
            first = mid;
    }
    --last;
    return comp(*first, *last) ? first : last;
}

template <typename RandomIt>
uint32_t Sequence::specialIndex(RandomIt first, RandomIt last) {
    RandomIt noMatch = last, begin = first;
    while (last - first > 1) {
        RandomIt mid = first + (last - first) / 2;
        int midValue = mid - begin;
        if (*mid == midValue)
            return midValue;
        if (*mid < midValue) {
            first = mid + 1;
        } else {
            last = mid;
        }
    }
    if (first == last || *first != first - begin)
        return -1;
    return first - begin;
}

#endif // !BINARY_SEARCH_CPP
