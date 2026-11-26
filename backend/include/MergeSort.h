/*
 * MergeSort.h — Recursive Merge Sort for ranking search results.
 *
 * Core Concept:
 *   Merge Sort is a classic Divide-and-Conquer algorithm:
 *     1. Divide: Split the unsorted vector into two halves at the midpoint.
 *     2. Conquer: Recursively sort each half until subarrays have length <= 1.
 *     3. Combine: Merge the two sorted halves into a single sorted sequence.
 *
 * Why Merge Sort for Search Results?
 *   - Guaranteed O(N log N) worst-case time complexity (unlike QuickSort O(N^2)).
 *   - Stable sorting: Preserves relative order of documents with identical relevance scores.
 *   - Deterministic and predictable performance.
 *
 * Ranking Criteria:
 *   1. Primary: Relevance Score (Descending - higher score first).
 *   2. Tie-Breaker 1: Matched Terms Count (Descending - more query terms first).
 *   3. Tie-Breaker 2: Document ID (Ascending - deterministic consistency).
 *
 * Complexity:
 *   - Best Case:   O(N log N)
 *   - Average:     O(N log N)
 *   - Worst Case:  O(N log N)
 *   - Space:       O(N) auxiliary space during merge
 *
 * Note: Implemented manually without sort.
 */

#ifndef MERGESORT_H
#define MERGESORT_H

#include "SearchEngine.h"
#include <vector>

using namespace std;

class MergeSort {
public:
    // Sorts a vector of SearchResult in descending order of relevance.
    static void sort(vector<SearchResult>& results);

    // Comparator determining if 'a' has higher ranking priority than 'b'.
    static bool compare(const SearchResult& a, const SearchResult& b);

private:
    // Recursive divide-and-conquer function
    static void mergeSort(vector<SearchResult>& arr, int left, int right);

    // Combines two sorted subarrays arr[left..mid] and arr[mid+1..right]
    static void merge(vector<SearchResult>& arr, int left, int mid, int right);
};

#endif // MERGESORT_H
