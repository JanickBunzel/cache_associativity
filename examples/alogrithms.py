import csv
import random


# region Config --------------------------------------------------------------------------------------------------

WRITE_CSV = True

PRINT_ARRAYS = True

ARRAY_SIZE = 10
ARRAY_MIN = 1
ARRAY_MAX = 1000

QUICKSORT_ENABLED = False
QUICKSORT_OUTPUT = 'examples/quicksort_requests.csv'

MERGESORT_ENABLED = False
MERGESORT_OUTPUT = 'examples/mergesort_requests.csv'

ITERATE_ENABLED = True
ITERATE_OUTPUT = 'examples/iterate_requests.csv'

# endregion Config -----------------------------------------------------------------------------------------------



# region Main
def main():
    global reads, writes
    reads = 0
    writes = 0

    global firstAddress
    firstAddress = random.randint(0x10000000, 0xFFFFFFF0)
    firstAddress &= ~0xF  # Align to 16 bytes
    print("\nFirst address:", hex(firstAddress), "\n")

    array = [random.randint(ARRAY_MIN, ARRAY_MAX) for _ in range(ARRAY_SIZE)]

    # Quick Sort
    if QUICKSORT_ENABLED:
        reads = 0
        writes = 0
        arraySorted = array.copy()
        quick_sort(arraySorted, QUICKSORT_OUTPUT)
        if PRINT_ARRAYS:
            print("Original array is:", array, "\n")
            print("Quick Sort array is:", arraySorted, "\n")
        print("Quick Sort Reads:", reads)
        print("Quick Sort Writes:", writes, "\n")


    # Merge Sort
    if MERGESORT_ENABLED:
        reads = 0
        writes = 0
        arraySorted = array.copy()
        merge_sort(array, MERGESORT_OUTPUT)
        if PRINT_ARRAYS:
            print("Original array is:", array, "\n")
            print("Merge Sort array is:", arraySorted, "\n")
        print("Merge Sort Reads:", reads)
        print("Merge Sort Writes:", writes, "\n")
    
    # Iterate
    if ITERATE_ENABLED:
        reads = 0
        writes = 0
        iterate_array(array, ITERATE_OUTPUT)
        if PRINT_ARRAYS:
            print("Iterated array is:", array, "\n")
        print("Iterate Reads:", reads)
        print("Iterate Writes:", writes, "\n")

def getAddress(index):
    return hex(firstAddress + index * 4)

def writeLine(filename, data):
    with open(filename, 'a', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(data)
# endregion Main



# region Quick Sort
def partition(arr, low, high, filename):
    global reads, writes
    global WRITE_CSV
    pivot = arr[high]
    i = low - 1
    for j in range(low, high):
        reads += 1
        if WRITE_CSV:
            writeLine(filename, ['R', getAddress(j)])  # Logging read operation
        if arr[j] < pivot:
            i += 1
            arr[i], arr[j] = arr[j], arr[i]
            writes += 2
            if WRITE_CSV:
                writeLine(filename, ['W', getAddress(i), arr[i]])  # Logging write operation
                writeLine(filename, ['W', getAddress(j), arr[j]])  # Logging write operation
    arr[i + 1], arr[high] = arr[high], arr[i + 1]
    writes += 2
    if WRITE_CSV:
        writeLine(filename, ['W', getAddress(i + 1), arr[i + 1]])  # Logging write operation
        writeLine(filename, ['W', getAddress(high), arr[high]])  # Logging write operation
    return i + 1

def quick_sort_helper(arr, low, high, filename):
    global reads, writes
    if low < high:
        pi = partition(arr, low, high, filename)
        quick_sort_helper(arr, low, pi - 1, filename)
        quick_sort_helper(arr, pi + 1, high, filename)

def quick_sort(arr, filename):
    with open(filename, mode='w', newline='') as file:
        file.write('')  # Truncate the file to make sure it starts empty
    quick_sort_helper(arr, 0, len(arr) - 1, filename)
# endregion Quick Sort



# region Merge Sort
def merge(arr, l, m, r, filename):
    global reads, writes
    n1 = m - l + 1
    n2 = r - m
    L = [0] * n1
    R = [0] * n2

    for i in range(0, n1):
        L[i] = arr[l + i]
        reads += 1
    for j in range(0, n2):
        R[j] = arr[m + 1 + j]
        reads += 1

    i = 0
    j = 0
    k = l
    while i < n1 and j < n2:
        if L[i] <= R[j]:
            arr[k] = L[i]
            i += 1
        else:
            arr[k] = R[j]
            j += 1
        writes += 1
        if WRITE_CSV:
            writeLine(filename, ['W', getAddress(k), arr[k]])
        k += 1

    while i < n1:
        arr[k] = L[i]
        i += 1
        k += 1
        writes += 1
        if WRITE_CSV:
            writeLine(filename, ['W', getAddress(k - 1), arr[k - 1]])

    while j < n2:
        arr[k] = R[j]
        j += 1
        k += 1
        writes += 1
        if WRITE_CSV:
            writeLine(filename, ['W', getAddress(k - 1), arr[k - 1]])

def merge_sort_helper(arr, l, r, filename):
    if l < r:
        m = l + (r - l) // 2
        merge_sort_helper(arr, l, m, filename)
        merge_sort_helper(arr, m + 1, r, filename)
        merge(arr, l, m, r, filename)

def merge_sort(arr, filename):
    with open(filename, mode='w', newline='') as file:
        file.write('')  # Truncate the file to make sure it starts empty
    merge_sort_helper(arr, 0, len(arr) - 1, filename)
# endregion Merge Sort



# region Iterate
def iterate_array(arr, filename):
    global reads
    with open(filename, mode='w', newline='') as file:
        file.write('')  # Truncate the file to make sure it starts empty
    for index, value in enumerate(arr):
        reads += 1
        if WRITE_CSV:
            writeLine(filename, ['R', getAddress(index)])
# endregion Iterate



# Call to main function
if __name__ == "__main__":
    main()
