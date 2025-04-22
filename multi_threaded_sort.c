#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*
 * This program implements a multi-threaded sorting application using POSIX threads.
 * It divides an array into two equal parts, sorts each part in parallel using
 * separate threads (insertion sort algorithm), and then merges the sorted halves
 * using a third thread.
 */

// Global arrays for storing the original and sorted lists
// These are global to allow easy access by all threads
int *original_array;  // Holds the input array and will contain the two sorted subarrays
int *sorted_array;    // Will hold the final merged and sorted array
int array_size;       // Size of the array to be sorted

// Structure to pass arguments to the sorting threads
// This allows us to specify which portion of the array each thread should sort
typedef struct {
    int start_index;  // Starting index of the subarray to sort
    int end_index;    // Ending index of the subarray to sort
} thread_args;

// Mutex for thread synchronization
// Protects access to shared data (sort_threads_completed counter)
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Condition variables for signaling between threads
// Used to notify the merge thread when both sorting threads have completed
pthread_cond_t sort_done = PTHREAD_COND_INITIALIZER;
// Counter to track how many sorting threads have completed their work
int sort_threads_completed = 0;

// Function to perform insertion sort on a subarray
void *insertion_sort(void *args) {
    thread_args *targs = (thread_args *)args;
    int start = targs->start_index;
    int end = targs->end_index;
    
    // Perform insertion sort on the subarray
    for (int i = start + 1; i <= end; i++) {
        int key = original_array[i];
        int j = i - 1;
        
        while (j >= start && original_array[j] > key) {
            original_array[j + 1] = original_array[j];
            j--;
        }
        
        original_array[j + 1] = key;
    }
    
    // Signal that this sorting thread has completed
    pthread_mutex_lock(&mutex);
    sort_threads_completed++;
    
    // If both sorting threads are done, signal the merge thread
    if (sort_threads_completed == 2) {
        pthread_cond_signal(&sort_done);
    }
    pthread_mutex_unlock(&mutex);
    
    pthread_exit(NULL);
}

// Function to merge two sorted subarrays
void *merge_arrays(void *arg) {
    // Wait until both sorting threads have completed
    pthread_mutex_lock(&mutex);
    while (sort_threads_completed < 2) {
        pthread_cond_wait(&sort_done, &mutex);
    }
    pthread_mutex_unlock(&mutex);
    
    // Calculate the midpoint of the array
    int mid = array_size / 2 - 1;
    
    // Merge the two sorted subarrays
    int i = 0;      // Index for first subarray
    int j = mid + 1; // Index for second subarray
    int k = 0;      // Index for merged array
    
    while (i <= mid && j < array_size) {
        if (original_array[i] <= original_array[j]) {
            sorted_array[k++] = original_array[i++];
        } else {
            sorted_array[k++] = original_array[j++];
        }
    }
    
    // Copy remaining elements from first subarray, if any
    while (i <= mid) {
        sorted_array[k++] = original_array[i++];
    }
    
    // Copy remaining elements from second subarray, if any
    while (j < array_size) {
        sorted_array[k++] = original_array[j++];
    }
    
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    // Define a hardcoded array
    int hardcoded_array[] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    
    // Set array size from the hardcoded array
    array_size = sizeof(hardcoded_array) / sizeof(hardcoded_array[0]);
    
    // Allow optional command line override of array size
    if (argc >= 2) {
        int requested_size = atoi(argv[1]);
        if (requested_size > 0 && requested_size <= array_size) {
            array_size = requested_size;
        } else {
            printf("Using default array size: %d\n", array_size);
        }
    }
    
    // Allocate memory for arrays
    original_array = (int *)malloc(array_size * sizeof(int));
    sorted_array = (int *)malloc(array_size * sizeof(int));
    
    if (original_array == NULL || sorted_array == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }
    
    // Copy values from hardcoded array
    for (int i = 0; i < array_size; i++) {
        original_array[i] = hardcoded_array[i];
    }

    
    // Print the original array
    printf("Original array: ");
    for (int i = 0; i < array_size; i++) {
        printf("%d ", original_array[i]);
    }
    printf("\n");
    
    // Create thread arguments
    thread_args first_half_args, second_half_args;
    first_half_args.start_index = 0;
    first_half_args.end_index = array_size / 2 - 1;
    
    second_half_args.start_index = array_size / 2;
    second_half_args.end_index = array_size - 1;
    
    // Create threads
    pthread_t sort_thread1, sort_thread2, merge_thread;
    
    // Create sorting threads
    if (pthread_create(&sort_thread1, NULL, insertion_sort, &first_half_args) != 0) {
        perror("Failed to create first sorting thread");
        return 1;
    }
    
    if (pthread_create(&sort_thread2, NULL, insertion_sort, &second_half_args) != 0) {
        perror("Failed to create second sorting thread");
        return 1;
    }
    
    // Create merge thread
    if (pthread_create(&merge_thread, NULL, merge_arrays, NULL) != 0) {
        perror("Failed to create merge thread");
        return 1;
    }
    
    // Wait for all threads to complete
    pthread_join(sort_thread1, NULL);
    pthread_join(sort_thread2, NULL);
    pthread_join(merge_thread, NULL);
    
    // Print the sorted array
    printf("Sorted array: ");
    for (int i = 0; i < array_size; i++) {
        printf("%d ", sorted_array[i]);
    }
    printf("\n");
    
    // Clean up
    free(original_array);
    free(sorted_array);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&sort_done);
    
    return 0;
}