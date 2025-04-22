# Multi-Threaded Sorting Application

This program demonstrates the use of POSIX threads (pthreads) in C to implement a parallel sorting algorithm. The application divides an array into two equal parts, sorts each part in parallel using separate threads, and then merges the sorted halves using a third thread.

## Implementation Details

### Overview

The program follows these steps:

1. The main thread initializes an array with values from a hardcoded array `{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}`.
2. The array is divided into two equal halves.
3. Two separate threads sort each half using insertion sort.
4. A third thread merges the two sorted halves into a final sorted array.
5. The main thread prints the final sorted array.

### Thread Synchronization

The program uses the following synchronization mechanisms:

- A mutex (`pthread_mutex_t`) to protect shared data
- A condition variable (`pthread_cond_t`) to signal when both sorting threads have completed
- A counter to track the number of completed sorting threads

### Sorting Algorithm

The program uses insertion sort for the sorting threads. Insertion sort is a simple sorting algorithm that builds the final sorted array one item at a time. It's efficient for small data sets and is more efficient than other simple quadratic algorithms like bubble sort.

## Building and Running

### Prerequisites

- GCC compiler
- POSIX threads library (pthread)

### Build Instructions

Compile the program using GCC with pthread support:

```bash
gcc -o multi_threaded_sort multi_threaded_sort.c -pthread
```

### Running the Program

Run the program with the default array size (10):

```bash
./multi_threaded_sort
```

This will use the hardcoded array `{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}`.

Alternatively, you can specify a smaller array size (must be less than or equal to 10):

```bash
./multi_threaded_sort <array_size>
```

### Examples

Sort the full hardcoded array (10 elements):
```bash
./multi_threaded_sort
```

Sort only the first 5 elements of the hardcoded array:
```bash
./multi_threaded_sort 5
```

## Code Explanation

### Global Variables

- `original_array`: Stores the input array to be sorted
- `sorted_array`: Stores the final sorted array after merging
- `array_size`: Size of the array

### Thread Functions

1. `insertion_sort`: Sorts a subarray using insertion sort algorithm
2. `merge_arrays`: Merges two sorted subarrays into a single sorted array

### Synchronization Mechanisms

- `mutex`: Protects access to shared data
- `sort_done`: Condition variable to signal when sorting is complete
- `sort_threads_completed`: Counter to track completed sorting threads

## Notes

- The program handles memory allocation and deallocation properly
- Error handling is implemented for thread creation and memory allocation
- The program demonstrates proper use of thread synchronization primitives
- The current implementation uses a hardcoded array rather than random numbers
- The program only allows specifying the array size, not the array elements themselves