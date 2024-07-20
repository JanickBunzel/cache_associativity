import csv

# Generate fourway.csv
with open('moin.csv', 'w', newline='') as file:
    writer = csv.writer(file)
    for _ in range(625):  # 625 iterations * 8 entries per iteration = 5000 entries
        writer.writerow(['W', '0x00000000', 10])
        writer.writerow(['W', '0x00001000', 20])
        writer.writerow(['W', '0x00002000', 30])
        writer.writerow(['W', '0x00003000', 40])
        writer.writerow(['W', '0x00004000', 50])
        writer.writerow(['R', '0x00000000'])
        writer.writerow(['R', '0x00001000'])
        writer.writerow(['R', '0x00002000'])
        writer.writerow(['R', '0x00003000'])
        writer.writerow(['R', '0x00004000'])

        writer.writerow(['W', '0x00000000', 10])
        writer.writerow(['W', '0x00001000', 20])
        writer.writerow(['W', '0x00002000', 30])
        writer.writerow(['W', '0x00003000', 40])
        writer.writerow(['R', '0x00000000'])
        writer.writerow(['R', '0x00001000'])
        writer.writerow(['R', '0x00002000'])
        writer.writerow(['R', '0x00003000'])
