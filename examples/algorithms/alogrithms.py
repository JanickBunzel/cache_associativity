import csv
import random


# region Config --------------------------------------------------------------------------------------------------

WRITE_CSV = False

PRINT_ARRAYS = True

ARRAY_SIZE = 1000
ARRAY_MIN = 1
ARRAY_MAX = 1000

QUICKSORT_ENABLED = True
QUICKSORT_OUTPUT = 'examples/algorithms/quicksort_n.csv'

ITERATE_ENABLED = False
ITERATE_OUTPUT = 'examples/algorithms/iterate_n.csv'

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
    if WRITE_CSV:
        writeLine(filename, ['R', getAddress(high)])  # Logging read operation
    
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
                writeLine(filename, ['R', getAddress(i)])  # Logging read operation
                writeLine(filename, ['R', getAddress(j)])  # Logging read operation
                writeLine(filename, ['W', getAddress(i), arr[j]])  # Logging write operation
                writeLine(filename, ['W', getAddress(j), arr[i]])  # Logging write operation
    arr[i + 1], arr[high] = arr[high], arr[i + 1]
    writes += 2
    if WRITE_CSV:
        writeLine(filename, ['R', getAddress(i + 1)])  # Logging read operation
        writeLine(filename, ['R', getAddress(high)])  # Logging read operation
        writeLine(filename, ['W', getAddress(i + 1), arr[high]])  # Logging write operation
        writeLine(filename, ['W', getAddress(high), arr[i + 1]])  # Logging write operation
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
