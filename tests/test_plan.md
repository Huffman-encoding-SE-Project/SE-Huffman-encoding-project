# Huffman Compression Tool — Test Plan

## 1. Objective

To verify that the Huffman compression tool compresses files and decompresses them correctly without losing the original data.

## 2. Testing Environment

- Programming language: C
- Compiler: GCC
- Test runner: Python 3
- Operating system: Windows and WSL Ubuntu
- Project: Team 9 — Huffman Encoding Project

## 3. Test Cases

| Test ID | Test Scenario | Expected Result |
|---|---|---|
| TC01 | Compress and decompress an empty file | The restored file matches the empty original |
| TC02 | Compress and decompress a single-byte file | The restored file matches the original |
| TC03 | Compress and decompress a repeated single-symbol file | The restored file matches the original |
| TC04 | Compress and decompress a two-symbol file | The restored file matches the original |
| TC05 | Compress and decompress a short text file | The restored file matches the original |
| TC06 | Compress and decompress a larger English text file | The restored file matches the original |
| TC07 | Compress and decompress skewed-frequency binary data | The restored file matches the original |
| TC08 | Compress and decompress a file containing all byte values | The restored file matches the original |
| TC09 | Compress and decompress random binary data | The restored file matches the original |
| TC10 | Attempt compression with a nonexistent input file | The program returns a nonzero exit code |
| TC11 | Attempt decompression of an invalid compressed file | The program returns a nonzero exit code |
| TC12 | Attempt decompression of a truncated compressed file | The program returns a nonzero exit code |
| TC13 | Run the program without arguments | The program returns exit code 2 and prints usage information |

These tests are automated by `tests/run_tests.py`.

## 4. Test Execution Status

The component tests and automated program tests were executed successfully. The automated test suite was run using `python3 tests/run_tests.py`.

| Test ID | Actual Result | Status |
|---|---|---|
| CT01 | Frequency counts verified: A = 3, B = 1, C = 1 | PASS |
| CT02 | Huffman tree constructed successfully; root frequency = 5 | PASS |
| CT03 | Huffman code lengths verified: A = 1, B = 2, C = 2 | PASS |
| TC01 | Empty file compression and decompression verified | PASS |
| TC02 | Single-byte file compression and decompression verified | PASS |
| TC03 | Repeated single-symbol file compression and decompression verified | PASS |
| TC04 | Two-symbol file compression and decompression verified | PASS |
| TC05 | Short text file compression and decompression verified | PASS |
| TC06 | Larger English text file compression and decompression verified | PASS |
| TC07 | Skewed-frequency binary data compressed and decompressed successfully; output matched the original | PASS |
| TC08 | File containing all byte values compressed and decompressed successfully | PASS |
| TC09 | Random binary file compressed and decompressed successfully; output matched the original | PASS |
| TC10 | Nonexistent input file was rejected with a nonzero exit code | PASS |
| TC11 | Invalid compressed file was rejected | PASS |
| TC12 | Truncated compressed file was rejected | PASS |
| TC13 | Program returned exit code 2 and displayed usage information when run without arguments | PASS |

**Automated Test Summary:** 78/78 checks passed. Decompressed files were verified against their original files using byte-by-byte comparison.

## 5. Acceptance Criteria

- Compression and decompression complete successfully for supported inputs.
- Decompressed output matches the original file byte for byte.
- Empty files and single-symbol files are handled correctly.
- Invalid, corrupted, and truncated inputs are handled safely.
- Memory and file resources are managed correctly.
- The program displays appropriate usage information when run without arguments.

## 6. Notes

- The source files compiled successfully using GCC, and the executable was generated.
- Windows native test execution was blocked by the system's Code Integrity policy.
- The project was compiled successfully using GCC in WSL Ubuntu.
- The automated test suite was executed using `python3 tests/run_tests.py`.
- All 78 checks passed.