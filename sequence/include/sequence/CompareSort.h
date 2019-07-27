#ifndef COMPARE_SORT_H
#define COMPARE_SORT_H

#include <functional>
#include <random>

/*
Comparison sorts (compatible with all data types, including objects)
*/
namespace CompareSort {
    /*
    Mergesort
    Forward iterators are sufficient
    O(n log n) worst case
    O(n) extra space
    */
    template<typename ForwardIt, typename Compare = std::less<>>
    void mergesort(ForwardIt first, ForwardIt last, Compare compare = Compare());

    /*
    Quicksort
    Needs bidirectional iterators
    O(n^2) worst case, O(n log n) expected case
    O(1) extra space
    */
    template<typename BiDirIt, typename Compare = std::less<>>
    void quicksort(BiDirIt first, BiDirIt last, Compare compare = Compare());
    
    /*
    Heapsort with external heap
    Forward iterators
    O(n log n) worst case
    O(n) extra space
    */
    template<typename ForwardIt, typename Compare = std::less<>>
    void heapsortOutPlace(ForwardIt first, ForwardIt last, Compare compare = Compare());

    /*
    In-place heapsort
    Random-access iterators
    O(n log n) worst case
    O(1) extra space
    */
    template<typename RandomIt, typename Compare = std::less<>>
    void heapsortInPlace(RandomIt first, RandomIt last, Compare compare = Compare());

    namespace {
        std::random_device rd;
        std::mt19937_64 engine(rd());
    }
}

#include "../../src/sequence/CompareSort.cpp"

#endif // !COMPARE_SORT_H
