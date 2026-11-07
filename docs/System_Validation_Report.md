# System Validation Report
## Basic ZIP Compression Tool Using Huffman Coding

### 1. Objective
This report documents system-level validation of the Huffman compression tool. The objective is to verify that the complete command-line application performs compression and decompression correctly, preserves input data, and handles invalid inputs and error conditions safely.

### 2. Validation Environment
- Operating System: Ubuntu 24.04 (WSL)
- Compiler: GCC 13.3.0
- Implementation Language: C
- Automated Test Runner: Python
- Coverage and test execution environment: WSL

### 3. Validation Scope
The following system behaviours were validated:

- End-to-end file compression and decompression.
- Byte-for-byte comparison of original and restored files.
- Empty, single-byte, text, and binary input handling.
- Single-symbol and skewed-frequency input handling.
- Handling of malformed and truncated compressed files.
- Missing input files and invalid command-line arguments.
- Output overwrite confirmation and path validation.

### 4. System Validation Results

The latest WSL integration test execution completed with **90/90 checks passed**.

The test suite covered the following input categories:

| Input Category | Validation Purpose | Result |
|---|---|---|
| Empty file | Zero-length input handling | PASS |
| Single-byte file | Minimum non-empty input | PASS |
| Repeated single-symbol file | Single-symbol encoding | PASS |
| Single null symbol | Binary zero-byte handling | PASS |
| Single high-byte symbol | High-bit byte handling | PASS |
| Odd-length single-symbol input | Single-symbol edge case | PASS |
| Two-symbol input | Small alphabet handling | PASS |
| Skewed binary input | Uneven byte frequencies | PASS |
| Short text | Basic text compression | PASS |
| English text | Larger text input | PASS |
| All byte values | Full byte-range handling | PASS |
| Random binary data | Binary data with limited redundancy | PASS |

### 5. Negative and Error-Handling Validation

| Scenario | Expected Behaviour | Result |
|---|---|---|
| Nonexistent input file | Error handled with nonzero exit code | PASS |
| Corrupted compressed header | Invalid input rejected | PASS |
| Truncated compressed header | Invalid input rejected | PASS |
| Truncated compressed payload | Invalid input rejected | PASS |
| Extra trailing data | Invalid input rejected | PASS |
| Missing command-line arguments | Usage information displayed | PASS |
| Invalid command | Error/usage message displayed | PASS |
| Identical input and output paths | Operation rejected | PASS |
| Existing output with overwrite declined | Existing file preserved | PASS |
| Existing output with overwrite accepted | Existing file overwritten | PASS |
| Invalid file extension | Operation rejected | PASS |
| Invalid output directory | File-writing error handled | PASS |

### 6. Data Integrity Validation

Compression and decompression were validated using automated byte-for-byte comparisons across the integration test inputs.

A separate manual demonstration using a 15-byte text file also confirmed that the original and decompressed files were identical.

A 25 MB file was additionally compressed and decompressed. Both operations completed, but a separate byte-for-byte comparison was not recorded. Therefore, exact recovery for this large-file test is not claimed.

### 7. Validation Limitations

- GUI implementation (`gui.c`) was not included in the coverage run.
- The 25 MB test did not have a recorded byte-for-byte comparison.
- Testing demonstrates correctness for the executed scenarios, not every possible input.
- The latest WSL integration run recorded 90/90 checks, while the earlier QA report records a previous 78/78 run.

### 8. Conclusion

The system validation results demonstrate successful end-to-end compression and decompression for the tested input categories. The latest WSL integration run passed all 90 checks. Negative-input, malformed-file, overwrite, and path-validation scenarios were also exercised.

The recorded manual comparison confirmed exact recovery for the 15-byte demonstration file. The report distinguishes this verified result from the 25 MB operation, for which a separate byte comparison was not recorded.
