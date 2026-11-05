# Code Coverage Report
## Huffman Encoding and Compression Tool

### 1. Objective
The objective of code coverage testing is to measure how much of the source code is executed during functional and unit testing.

### 2. Testing Environment
- Operating System: Ubuntu 24.04 (WSL)
- Compiler: GCC 13.3.0
- Coverage Tool: gcov
- Coverage flags: `-O0 -g --coverage`
- Language: C

### 3. Scope
Coverage was collected for:
- `src/main.c`
- `src/huffman.c`
- `src/file_io.c`

The GUI implementation (`gui.c`) was not included in this coverage run.

### 4. CLI Integration Coverage

The following results were collected after executing the CLI integration tests and additional manual test cases.

| Source File | Lines Executed | Branches Executed | Branches Taken at Least Once |
|---|---:|---:|---:|
| main.c | 97.44% | 97.30% | 81.08% |
| file_io.c | 100.00% | 100.00% | 65.00% |
| huffman.c | 84.93% | 96.97% | 72.73% |

**Combined results:**
- Line coverage: 88.96% (282 of 317 lines)
- Branches executed: 97.35% (220 of 226)
- Branch outcomes taken at least once: 74.78% (169 of 226)

### 5. Unit Test Coverage

Each C unit test was separately compiled with coverage instrumentation and executed.

| Unit Test | huffman.c Lines | huffman.c Branches Taken at Least Once | file_io.c Lines | file_io.c Branches Taken at Least Once |
|---|---:|---:|---:|---:|
| Frequency | 2.28% | 1.52% | 70.00% | 30.00% |
| Tree | 21.46% | 17.42% | 70.00% | 30.00% |
| Codes | 28.77% | 21.97% | 70.00% | 30.00% |
| Format | 51.14% | 40.91% | 70.00% | 30.00% |

These are separate test profiles. They are not added to the CLI integration results.

### 6. Observations
- The CLI integration tests exercised most of the main program's executable lines.
- File I/O achieved 100% line coverage during CLI integration testing.
- The format test exercised the largest portion of Huffman implementation code among the four individual unit tests.
- Additional test cases covered overwrite confirmation, invalid commands, invalid file extensions, identical input/output paths, and output write failures.
- Some branches and lines remain uncovered and may require additional test cases.

### 7. Limitations
- GUI source code was not included.
- Unit-test coverage profiles were collected separately and have not been merged into a single coverage profile.
- Coverage percentages indicate execution, not the correctness of every possible input or output.

### 8. Conclusion
GCC gcov was used to measure line and branch execution for the Huffman compression tool. The CLI integration tests achieved 88.96% line coverage, while 74.78% of branch outcomes were taken at least once. Separate unit-test profiles provided additional information about the execution of the Huffman and file I/O modules.
