import csv
import random

# Generate direct_mapped_better.csv with unique addresses
with open('direct_mapped_better.csv', 'w', newline='') as file:
    writer = csv.writer(file)
    base_address = 0x10000000
    for i in range(2500):  # 2500 iterations * 2 entries per iteration = 5000 entries
        writer.writerow(['W', f'0x{base_address + i*0x1000:08X}', random.randint(10, 100)])
        writer.writerow(['R', f'0x{base_address + i*0x1000:08X}'])
