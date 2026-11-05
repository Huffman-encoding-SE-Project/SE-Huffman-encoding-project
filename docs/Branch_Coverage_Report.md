# Branch Coverage Report
## Huffman Encoding and Compression Tool

### 1. Objective
Branch coverage testing evaluates whether the different decision outcomes in the program have been exercised during testing.

### 2. Testing Environment
- Operating System: Ubuntu 24.04 (WSL)
- Compiler: GCC 13.3.0
- Coverage Tool: gcov
- Compiler flags: `-O0 -g --coverage`

### 3. Branch Coverage Results

The following results were collected from CLI integration testing.

| Source File | Total Branches | Branches Executed | Branch Outcomes Taken at Least Once |
|---|---:|---:|---:|
| main.c | 74 | 72 | 60 |
| file_io.c | 20 | 20 | 13 |
| huffman.c | 132 | 128 | 96 |
| **Total** | **226** | **220** | **169** |

### 4. Overall Results

- Branches executed: 220/226 (97.35%)
- Branch outcomes taken at least once: 169/226 (74.78%)

The two measurements are different: a branch can be reached without both of its possible outcomes being observed.

### 5. Additional Test Scenarios

The following scenarios were executed to exercise decision paths:

| Test Scenario | Expected Behaviour |
|---|---|
| Existing output with `n` confirmation | Operation cancelled; existing file preserved |
| Existing output with `y` confirmation | Output overwritten |
| Invalid overwrite response | Invalid choice displayed; prompt repeated |
| Invalid command | Usage/error message displayed |
| Identical input and output paths | Operation rejected |
| Invalid compression output extension | Operation rejected |
| Invalid decompression input extension | Operation rejected |
| Invalid output directory | File write error handled |

### 6. Unit Test Branch Coverage

Separate unit-test profiles were also collected.

| Unit Test | huffman.c Branches Executed | huffman.c Branch Outcomes Taken at Least Once |
|---|---:|---:|
| Frequency | 1.52% | 1.52% |
| Tree | 19.70% | 17.42% |
| Codes | 25.76% | 21.97% |
| Format | 51.52% | 40.91% |

These figures are independent profiles and are not combined with the CLI integration results.

### 7. Uncovered Branches and Limitations

- Some branch outcomes were not exercised by the available test cases.
- GUI code (`gui.c`) was not included.
- The individual unit-test coverage profiles were not merged into the CLI integration profile.
- Further edge-case testing may improve branch outcome coverage.

### 8. Conclusion

The CLI integration testing exercised 97.35% of the recorded branch points, with 74.78% of branch outcomes taken at least once. Manual testing of overwrite confirmation, invalid commands, file extensions, path validation, and file-writing failures helped improve decision-path coverage.
