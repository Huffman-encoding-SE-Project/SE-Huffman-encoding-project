# Huffman Compression Tool — Test Plan

## 1. Objective

To verify that the Huffman compression tool compresses files and decompresses them correctly without losing the original data.

## 2. Testing Environment

- Programming language: C
- Compiler: GCC
- Test runner: Python 3
- Operating system: Windows
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

These tests are automated by `tests/run_tests.py`. Their actual pass/fail status will be recorded after the test script runs successfully.



## 4. Test Execution Status

The following component tests were executed successfully. The full program test suite is still pending.

| Test ID | Actual Result | Status |
|---|---|---|
| CT01 | Frequency counts verified: A = 3, B = 1, C = 1 | PASS |
| CT02 | Huffman tree constructed successfully; root frequency = 5 | PASS |
| CT03 | Huffman code lengths verified: A = 1, B = 2, C = 2 | PASS |
| TC01–TC13 | Full program tests not yet executed | Pending |


## 5. Acceptance Criteria

- Compression and decompression complete successfully for supported inputs.
- Decompressed output matches the original file byte for byte.
- Empty files and single-symbol files are handled correctly.
- Invalid, corrupted, and truncated inputs are handled safely.
- Memory and file resources are managed correctly.
- The program displays appropriate usage information when run without arguments.
## 6. Notes

The source files compiled successfully using GCC, and the executable was generated. However, Windows Code Integrity blocked execution of the executable. Functional test results are therefore pending until execution is possible in an approved environment.
