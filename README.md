# Huffman Encoding Project

A C-based, lossless file compression tool using Huffman coding.

## Features

- Command-line file compression and decompression
- Byte-frequency analysis
- Huffman tree construction
- Huffman code generation
- Bit-level encoding and decoding
- Compressed-file header creation and validation
- Compression ratio and file-size reporting
- Input validation and error handling
- Support for text and binary files

## Project Structure

```text
Huffman-encoding-project-SE/
├── src/
│   ├── main.c
│   ├── file_io.c
│   ├── file_io.h
│   ├── huffman.c
│   └── huffman.h
├── tests/
│   ├── test_plan.md
│   └── run_tests.py
├── README.md
```

## Requirements

- GCC C compiler
- Python 3 for running the test script
- Windows, Linux, or another compatible environment

## Build Instructions

Open a terminal in the project root directory and compile the C source files:

```bash
gcc src/main.c src/file_io.c src/huffman.c -o hzip
```

On Windows, the output executable can be named `hzip.exe`:

```powershell
gcc src/main.c src/file_io.c src/huffman.c -o hzip.exe
```

## Usage

The program accepts the following command formats:

### Compress a file

```bash
hzip compress input_file output_file
```

### Decompress a file

```bash
hzip decompress compressed_file output_file
```

- `compress`: Compresses the input file and writes the compressed data to the output file.
- `decompress`: Decompresses the input file and restores the original data to the output file.

On Windows, use `hzip.exe` instead of `hzip` when running the executable directly.

**Note:** These commands describe the implemented command-line interface. Successful execution and compression/decompression results are still pending verification.


## Testing

The project includes a test plan and a Python test runner under the `tests/` directory.

To run the test script from the project root:

```bash
python tests/run_tests.py
```

### Test Results

| Check | Status |
|---|---|
| C compilation | Passed — compiled without errors |
| Executable generation | Passed — `hzip.exe` was created |
| Program execution | Blocked by Windows Code Integrity policy |
| Compression functionality | Pending verification |
| Decompression functionality | Pending verification |
| Text and binary file tests | Pending verification |
| Empty-file and single-symbol tests | Pending verification |
| Invalid or corrupted input tests | Pending verification |

Tests will be marked as passed only after their actual results have been verified.

## Team 9

- Ritesh P — PES2UG24CS340
- Pavithra N — PES2UG24CS347
- Nikshep V A — PES2UG24CS320
- Pranav Balebail — PES2UG24CS362
