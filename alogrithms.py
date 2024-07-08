import csv
import random

reads = 0
writes = 0

writeCsv = True
filename = 'quicksort_requests.csv'

array = [random.randint(1, 1000) for _ in range(100)]
print("Original array is:", array)

firstAddress = random.randint(0x10000000, 0xFFFFFFF0)
print("First address:", hex(firstAddress))

def getAdress(index):
    return hex(firstAddress + index*4)

def partition(arr, low, high):
    global reads, writes
    pivot = arr[high]
    i = low - 1
    for j in range(low, high):
        reads += 1
        if writeCsv:
            with open(filename, mode='a', newline='') as file:
                writer = csv.writer(file)
                writer.writerow(['R', getAdress(j)])  # Logging read operation
        if arr[j] < pivot:
            i += 1
            arr[i], arr[j] = arr[j], arr[i]
            writes += 2
            if writeCsv:
                with open(filename, mode='a', newline='') as file:
                    writer = csv.writer(file)
                    writer.writerow(['W', getAdress(i), arr[i]])  # Logging write operation
                    writer.writerow(['W', getAdress(j), arr[j]])  # Logging write operation
    arr[i + 1], arr[high] = arr[high], arr[i + 1]
    writes += 2
    if writeCsv:
        with open(filename, mode='a', newline='') as file:
            writer = csv.writer(file)
            writer.writerow(['W', getAdress(i + 1), arr[i + 1]])  # Logging write operation
            writer.writerow(['W', getAdress(high), arr[high]])  # Logging write operation
    return i + 1

def quick_sort(arr, low, high):
    if low < high:
        pi = partition(arr, low, high)
        print("Array after partitioning at index", pi, ":", arr)
        quick_sort(arr, low, pi - 1)
        quick_sort(arr, pi + 1, high)

with open(filename, mode='w', newline='') as file:
    writer = csv.writer(file)

quick_sort(array, 0, len(array) - 1)

print("Sorted array is:", array)
print("Reads:", reads)
print("Writes:", writes)
