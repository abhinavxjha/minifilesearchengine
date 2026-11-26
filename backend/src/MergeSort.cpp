// MergeSort.cpp — Recursive merge sort for ranking SearchResult items.

#include "MergeSort.h"

using namespace std;

// Primary: score desc; tie-breakers: matchedTermsCount desc, docId asc.
bool MergeSort::compare(const SearchResult& a, const SearchResult& b) {
    if (a.score != b.score) {
        return a.score > b.score;
    }
    if (a.matchedTermsCount != b.matchedTermsCount) {
        return a.matchedTermsCount > b.matchedTermsCount;
    }
    return a.docId < b.docId;
}

void MergeSort::sort(vector<SearchResult>& results) {
    if (results.size() <= 1) return;
    mergeSort(results, 0, static_cast<int>(results.size()) - 1);
}

void MergeSort::mergeSort(vector<SearchResult>& arr, int left, int right) {
    if (left >= right) return;

    int mid = left + (right - left) / 2;

    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

void MergeSort::merge(vector<SearchResult>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<SearchResult> L(n1);
    vector<SearchResult> R(n2);

    for (int i = 0; i < n1; i++) {
        L[i] = arr[left + i];
    }
    for (int j = 0; j < n2; j++) {
        R[j] = arr[mid + 1 + j];
    }

    int i = 0;
    int j = 0;
    int k = left;

    while (i < n1 && j < n2) {
        if (compare(L[i], R[j])) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }

    while (i < n1) {
        arr[k++] = L[i++];
    }

    while (j < n2) {
        arr[k++] = R[j++];
    }
}
