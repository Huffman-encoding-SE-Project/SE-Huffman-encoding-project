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

### Verified Results

| Test | Result |
|---|---|
| C compilation | Passed |
| Command-line usage output | Passed |
| Byte-frequency counting | Passed — A = 3, B = 1, C = 1 |
| Huffman tree construction | Passed — root frequency = 5 |
| Huffman code generation | Passed — A length = 1, B length = 2, C length = 2 |
| Basic text-file compression | Passed |
| Basic text-file decompression | Passed |
| Original/restored file comparison | Passed — no differences found using `fc.exe /b` |

### Pending Tests

The complete planned test suite has not yet been executed.

- Empty-file and single-symbol cases
- Larger text files and binary files
- All possible byte values and random binary data
- Invalid and truncated compressed files
- Nonexistent input files and error handling
- Complete functional and portability verification

### Run the Test Script

From the project root, run:

```bash
python tests/run_tests.py
```

**Note:** In the basic test, the 15-byte text file became larger after compression because the header overhead exceeded the savings for such a small input. The original and decompressed files nevertheless matched byte-for-byte.

Only tests with verified results are marked as passed. The remaining tests must be executed before claiming complete verification.

## Team 9

- Ritesh P — PES2UG24CS340
- Pavithra N — PES2UG24CS347
- Nikshep V A — PES2UG24CS320
- Pranav Balebail — PES2UG24CS362
