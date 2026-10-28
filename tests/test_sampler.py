import random

file_size = 25_000_000  # 25 MB
filename = "ABC_25MB.txt"
characters = "ABC"
chunk_size = 1_000_000

with open(filename, "w", encoding="ascii") as file:
    for written in range(0, file_size, chunk_size):
        size = min(chunk_size, file_size - written)
        data = "".join(random.choices(characters, k=size))
        file.write(data)

        progress = (written + size) / file_size * 100
        print(f"\rGenerating file: {progress:.1f}%", end="")

print(f"\nDone! Created {filename} ({file_size:,} bytes).")
