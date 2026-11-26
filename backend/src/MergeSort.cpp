/*
 * MergeSort.cpp — Implementation of recursive Merge Sort algorithm.
 */

#include "MergeSort.h"

using namespace std;

/*
 * compare — Ranking comparator.
 *
 * Returns true if 'a' should appear BEFORE 'b' in final results.
 * Strict descending order with two consistent tie-breakers:
 *   1. Score (higher first)
 *   2. Matched terms count (higher first)
 *   3. Document ID (lower first for stability)
 */
bool MergeSort::compare(const SearchResult& a, const SearchResult& b) {
    if (a.score != b.score) {
        return a.score > b.score; // Higher score ranks first
    }
    if (a.matchedTermsCount != b.matchedTermsCount) {
        return a.matchedTermsCount > b.matchedTermsCount; // Better query coverage
    }
    return a.docId < b.docId; // Deterministic tie-breaker
}

/*
 * sort — Public entry point.
 */
void MergeSort::sort(vector<SearchResult>& results) {
    if (results.size() <= 1) return;
    mergeSort(results, 0, static_cast<int>(results.size()) - 1);
}

/*
 * mergeSort — Recursive Divide and Conquer.
 *
 * Recurrence relation:
 *   T(N) = 2 * T(N / 2) + O(N)
 * By Master Theorem: T(N) = O(N log N)
 */
void MergeSort::mergeSort(vector<SearchResult>& arr, int left, int right) {
    if (left >= right) return; // Base case: 1 or 0 elements

    int mid = left + (right - left) / 2;

    // Divide & Conquer
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);

    // Combine
    merge(arr, left, mid, right);
}

/*
 * merge — Merges two sorted subarrays into one.
 *
 * Left subarray:  arr[left .. mid]
 * Right subarray: arr[mid + 1 .. right]
 *
 * Space Complexity: O(N) for temp arrays
 * Time Complexity:  O(N) single-pass comparison
 */
void MergeSort::merge(vector<SearchResult>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Temporary storage for both halves
    vector<SearchResult> L(n1);
    vector<SearchResult> R(n2);

    for (int i = 0; i < n1; i++) {
        L[i] = arr[left + i];
    }
    for (int j = 0; j < n2; j++) {
        R[j] = arr[mid + 1 + j];
    }

    int i = 0;    // Initial index of first subarray
    int j = 0;    // Initial index of second subarray
    int k = left; // Initial index of merged array

    // Pick highest ranked candidate from L or R
    while (i < n1 && j < n2) {
        if (compare(L[i], R[j])) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }

    // Copy remaining elements of L[] if any
    while (i < n1) {
        arr[k++] = L[i++];
    }

    // Copy remaining elements of R[] if any
    while (j < n2) {
        arr[k++] = R[j++];
    }
}
