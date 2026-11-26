// MergeSort.h — Recursive O(N log N) merge sort for ranking search results.

#ifndef MERGESORT_H
#define MERGESORT_H

#include "SearchEngine.h"
#include <vector>

using namespace std;

class MergeSort {
public:
    static void sort(vector<SearchResult>& results);
    static bool compare(const SearchResult& a, const SearchResult& b);

private:
    static void mergeSort(vector<SearchResult>& arr, int left, int right);
    static void merge(vector<SearchResult>& arr, int left, int mid, int right);
};

#endif // MERGESORT_H
