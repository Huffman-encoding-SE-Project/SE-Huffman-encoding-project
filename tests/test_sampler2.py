import os
import random

OUTPUT_DIR = "tests/test_inputs"
os.makedirs(OUTPUT_DIR, exist_ok=True)

def write_chunked_file(filepath, total_bytes, char_pool, chunk_size=1_000_000):
    with open(filepath, "w", encoding="ascii") as file:
        if total_bytes == 0:
            return
        for written in range(0, total_bytes, chunk_size):
            size = min(chunk_size, total_bytes - written)
            data = "".join(random.choices(char_pool, k=size))
            file.write(data)
            progress = (written + size) / total_bytes * 100
            print(f"\rGenerating {os.path.basename(filepath)}: {progress:.1f}%", end="")
    print(f"\nDone! Created {filepath} ({total_bytes:,} bytes).")

# Define the 8 test input configurations
test_cases = [
    ("01_empty.txt", 0, "ABC"),
    ("02_single_byte.txt", 1, "A"),
    ("03_short_predictable.txt", None, "AABBCC"),  # Exact literal string
    ("04_single_char_repeated.txt", 1_000, "A"),
    ("05_two_chars_only.txt", 10_000, "AB"),
    ("06_small_random.txt", 1_000, "ABC"),
    ("07_medium_random.txt", 1_000_000, "ABC"),
    ("08_ABC_25MB.txt", 25_000_000, "ABC"),
]

if __name__ == "__main__":
    print(f"Generating 8 test input files in './{OUTPUT_DIR}/'...\n")
    for filename, size, chars in test_cases:
        filepath = os.path.join(OUTPUT_DIR, filename)
        if size is None:
            # Write exact literal content for manual verification
            with open(filepath, "w", encoding="ascii") as f:
                f.write(chars)
            print(f"Done! Created {filepath} ({len(chars)} bytes - exact: '{chars}').")
        else:
            write_chunked_file(filepath, size, chars)
    print("\nAll 8 test input files are ready for automated testing and demo!")
