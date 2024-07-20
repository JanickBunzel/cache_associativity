import csv
import random

# Generate realistic_fourway.csv with more arbitrary addresses
with open('realistic_fourway.csv', 'w', newline='') as file:
    writer = csv.writer(file)
    sequential_addresses = [
        '0x1A2B3C01', '0x1A2B3C05', '0x1A2B3C09', '0x1A2B3C0D',
        '0x2B3C4D11', '0x3C4D5E21', '0x4D5E6F31', '0x5E6F7011'
    ]
    random_addresses = [f'0x{random.randint(0, 0xFFFFFF):06X}' for _ in range(100)]

    for _ in range(312):  # 312 iterations * 16 entries per iteration = 4992 entries (close to 5000)
        # Sequential Writes
        for address in sequential_addresses:
            writer.writerow(['W', address, random.randint(10, 100)])
        # Sequential Reads
        for address in sequential_addresses:
            writer.writerow(['R', address])
        # Random Writes
        for _ in range(4):
            writer.writerow(['W', random.choice(random_addresses), random.randint(10, 100)])
        # Random Reads
        for _ in range(4):
            writer.writerow(['R', random.choice(random_addresses)])
