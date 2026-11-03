# Huffman Encoding Project

A C-based, lossless file compression tool using Huffman coding. The program compresses files by assigning variable-length codes to bytes based on their frequencies and supports decompression to recover the original data.

## 1. Project Overview and Features

### Features
- Command-line file compression and decompression
- Byte-frequency analysis
- Huffman tree construction
- Huffman code generation
- Bit-level encoding and decoding
- Compressed-file header creation and validation
- Compression ratio and file-size reporting
- Input validation and error handling
- Support for text and binary files

## 2. Project Structure

```text
Huffman-encoding-project-SE/
├── src/
│   ├── main.c
│   ├── file_io.c
│   ├── file_io.h
│   ├── huffman.c
│   └── huffman.h
├── tests/
│   ├── test_frequency.c
│   ├── test_tree.c
│   ├── test_codes.c
│   ├── test_plan.md
│   └── run_tests.py
└── README.md
```

## 3. Requirements and Build Instructions

### Requirements
- GCC C compiler
- Python 3 for running the Python test runner
- Windows, Linux, or another compatible environment

### Build

Open a terminal in the project root directory and compile the C source files.

**Linux:**
```bash
gcc src/main.c src/file_io.c src/huffman.c -o hzip
```

**Windows:**
```powershell
gcc src/main.c src/file_io.c src/huffman.c -o hzip.exe
```

## 4. How to Use the Program

### Compress a file

**Linux:**
```bash
./hzip compress input_file output_file
```

**Windows:**
```powershell
.\hzip.exe compress input_file output_file
```

### Decompress a file

**Linux:**
```bash
./hzip decompress compressed_file output_file
```

**Windows:**
```powershell
.\hzip.exe decompress compressed_file output_file
```

Replace `input_file`, `compressed_file`, and `output_file` with the required file paths.

## 5. Testing and Verified Results

### Component and Functional Tests

| Test | Result |
|---|---|
| C compilation | Passed |
| Byte-frequency counting | Passed — A = 3, B = 1, C = 1 |
| Huffman tree construction | Passed — root frequency = 5 |
| Huffman code generation | Passed — A length = 1, B length = 2, C length = 2 |
| Basic text-file compression | Passed |
| Basic text-file decompression | Passed |
| Original/restored file comparison | Passed — no differences found using byte-by-byte comparison |

### Automated Test Results

The automated test runner was executed successfully in WSL Ubuntu.

- **90/90 checks passed** in the latest recorded run.
- Tested empty files, single-byte files, repeated symbols, two-symbol files, short and larger text files, skewed-frequency binary data, all byte values, and random binary data.
- Invalid-input handling was tested, including nonexistent input files, invalid compressed files, truncated headers and payloads, corrupted magic values, extra trailing bytes, and running without arguments.
- Decompressed files were verified against their originals using byte-by-byte comparisons.

### Large-File Testing

A separate large-file test was also performed.

- Created `ABC_25MB.txt` containing generated data using `test_sampler.py`.
- Compression and decompression completed successfully.

### Run the Test Script

From the project root, run:

```bash
python3 tests/run_tests.py
```

To run the test runner on a specific input file:

```bash
python3 tests/run_tests.py path/to/input_file.txt
```

The runner rebuilds the program and C tests, performs frequency/tree/code checks, verifies compression and decompression, and compares the restored file byte-for-byte. It also writes the test report to `tests/results.txt`. Paths containing spaces should be quoted.

**Note:** Compression may increase the size of small files or random data because of compressed-file header overhead. This does not necessarily indicate a failure of the compression or decompression process.

## Team 9

- Ritesh P — PES2UG24CS340
- Pavithra N — PES2UG24CS347
- Nikshep V A — PES2UG24CS320
- Pranav Balebail — PES2UG24CS362