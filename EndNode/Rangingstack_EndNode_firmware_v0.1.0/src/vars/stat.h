#ifndef STAT_H
#define STAT_H

#include "vektor.h"
#include <math.h>

// Swap function for sorting
template <typename T>
void swap(T &a, T &b)
{
    T temp = a;
    a = b;
    b = temp;
}

// QuickSort for sorting the Vektor
template <typename T>
void quickSort(Vektor<T> &arr, int left, int right)
{
    if (left >= right)
        return;

    T pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; j++)
    {
        if (arr[j] <= pivot)
        {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    swap(arr[i + 1], arr[right]);
    int pivotIndex = i + 1;

    quickSort(arr, left, pivotIndex - 1);
    quickSort(arr, pivotIndex + 1, right);
}

// Function to calculate median for a given range
template <typename T>
T calculateMedian(Vektor<T> &arr, size_t start, size_t end)
{
    size_t len = end - start + 1;
    if (len % 2 == 0)
    {
        return (arr[start + len / 2 - 1] + arr[start + len / 2]) / 2.0;
    }
    else
    {
        return arr[start + len / 2];
    }
}

template <typename T>
void computeQuartiles(Vektor<T> &arr, T &q1, T &q2, T &q3)
{
    size_t size = arr.getSize();

    // Sort the array
    quickSort(arr, 0, size - 1);

    // Compute median (Q2)
    q2 = calculateMedian(arr, 0, size - 1);

    // Compute Q1 (Lower quartile)
    size_t mid = size / 2;
    q1 = calculateMedian(arr, 0, mid - 1);

    // Compute Q3 (Upper quartile)
    q3 = calculateMedian(arr, (size % 2 == 0) ? mid : mid + 1, size - 1);
}

#endif // STAT_H
