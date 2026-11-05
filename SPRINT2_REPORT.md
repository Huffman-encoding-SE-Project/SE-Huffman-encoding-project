

\# Sprint 2 Report — Huffman Encoding and Decoding Project



\## Project Information



\- \*\*Project:\*\* Huffman Encoding and Decoding

\- \*\*Repository:\*\* \[SE-Huffman-encoding-project](https://github.com/Huffman-encoding-SE-Project/SE-Huffman-encoding-project)

\- \*\*Sprint:\*\* Sprint 2

\- \*\*Sprint 2 contributors:\*\* Nikshep and Pranav Balebail

\- \*\*Integration:\*\* GUI merged through PR #37; file-validation and overwrite-confirmation work integrated through PR #35



\---



\## A. Sprint Overview



Sprint 2 focused on improving the correctness, reliability, usability, and test coverage of the Huffman encoding and decoding application.



The work extended the functionality completed in Sprint 1 and addressed important edge cases in compression and decompression. It also introduced a graphical user interface so users could operate the application without relying exclusively on command-line commands.



The sprint included:



1\. Decompression validation and handling of invalid input.

2\. Portability improvements.

3\. Requirement-based and regression testing.

4\. Bug fixes and release preparation.

5\. A graphical user interface.

6\. Additional file-handling safeguards, including path and extension validation and overwrite confirmation.



The completed work was integrated into the project repository. Branch-testing details are being maintained separately by the designated teammate.



\## B. Sprint 2 Task and Ownership Matrix



| Issue | Task | Owner | Sprint outcome |

|---|---|---|---|

| #11 | Decompression validation | Nikshep | Completed |

| #16 | Portability improvements | Nikshep | Completed |

| #22 | Requirement-based tests | Nikshep | Completed |

| #23 | Bug fixes and regression testing | Nikshep | Completed |

| #24 | Release preparation | Nikshep | Completed |

| #31 | Graphical user interface | Nikshep | Completed |

| #10 | Bit-level encoding and header handling | Pranav Balebail | Completed |

| #12 | Empty-file and single-symbol handling | Pranav Balebail | Completed |

| #13 | Corrupted and truncated input handling | Pranav Balebail | Completed |

| #14 | Lossless and binary-file tests | Pranav Balebail | Completed |

| #15 | Memory and resource safety | Pranav Balebail | Completed |

| #33 | File extension and path validation | Pranav Balebail | Completed |

| #34 | Overwrite confirmation | Pranav Balebail | Completed |



\## C. Implementation Summary



\### C.1 Huffman encoding and decoding



Huffman coding is a lossless compression technique. It assigns shorter bit sequences to more frequent symbols and longer bit sequences to less frequent symbols. The encoder uses symbol frequencies to construct a Huffman tree, then uses the tree to generate codes.



At a high level, the compression process is:



1\. Read the input file as bytes.

2\. Count the frequency of each byte value.

3\. Build a Huffman tree from the frequency table.

4\. Generate a code for each byte.

5\. Encode the input bytes using those codes.

6\. Write the encoded data and the information required for decoding to the compressed output.



The decompression process reverses this sequence:



1\. Read and validate the compressed file.

2\. Recover the information needed to reconstruct the Huffman tree.

3\. Read the encoded bitstream.

4\. Decode the bitstream into the original bytes.

5\. Write the decompressed output.



The implementation is intended to preserve the original file contents, including non-text and binary data.



\### C.2 Bit-level encoding and header handling



Sprint 2 included work on bit-level encoding and the compressed-file header. The encoded symbols are represented as bits rather than being stored as separate text characters.



The header carries information required by the decoder to interpret the encoded data. Correct handling of this information is important because the decoder must reconstruct the same code tree used by the encoder.



The project’s bit-level representation and header handling are part of the compression format. Any future changes to this format should be coordinated with the decoder and regression tests.



\### C.3 Edge-case handling



The sprint addressed inputs that do not behave like ordinary multi-symbol files:



\- Empty input files.

\- Files containing only one distinct symbol.

\- Corrupted compressed input.

\- Truncated compressed input.

\- Binary files and arbitrary byte values.



These cases are important because a compression program should not assume that every input is a non-empty text file containing several different characters.



\### C.4 File validation and overwrite protection



Additional safeguards were implemented for file operations.



The application validates file paths and extensions before proceeding with the relevant operation. It also asks for confirmation before overwriting an existing output file.



These checks help prevent accidental data loss and make invalid input easier to identify.



\### C.5 Graphical user interface



A graphical interface was added in `gui.c`. The GUI provides a user-facing way to select files and invoke compression or decompression.



The GUI is intended to make the application easier to use while retaining the existing Huffman functionality. It does not introduce a separate compression algorithm or format.



\*\*GUI workflow:\*\*



1\. Select the operation: compression or decompression.

2\. Choose the input file.

3\. Choose or confirm the output location.

4\. Start the operation.

5\. Review the status or error message returned by the application.



The GUI was integrated into the project through PR #37.



\## D. Requirement Coverage and Completion Report



| Area | Sprint 2 work | Status |

|---|---|---|

| Compression and decompression | Continued implementation and validation of the Huffman workflow | Completed |

| Decompression validation | Validation of compressed input and decompression behavior | Completed |

| Bit-level encoding | Bitstream and header-related handling | Completed |

| Empty and single-symbol files | Edge-case handling | Completed |

| Corrupted and truncated input | Invalid-input handling | Completed |

| Binary-file support | Lossless behavior for binary input | Completed |

| File operations | Path/extension validation and overwrite confirmation | Completed |

| Regression testing | Requirement-based tests and bug-fix validation | Completed |

| Portability | Portability improvements | Completed |

| GUI | File selection and compression/decompression interface | Completed |

| Branch testing | Separate document being prepared by the designated teammate | Documented separately |



\## E. Testing Report



\### E.1 Automated test suite



The project README records \*\*90/90 automated checks passed\*\*.



This is the test result recorded by the project. It should not be interpreted as a new test run after every subsequent change unless the suite has been rerun on the final commit.



\### E.2 Test areas



The Sprint 2 testing work covered the following areas:



| Test area | Purpose |

|---|---|

| Requirement-based tests | Check the expected behavior of the application |

| Decompression validation | Check that compressed input is validated before or during decoding |

| Empty-file tests | Verify behavior when the input contains no bytes |

| Single-symbol tests | Verify behavior when the input contains only one distinct byte |

| Binary-file tests | Check that compression and decompression preserve non-text data |

| Corrupted-input tests | Check behavior when compressed data is invalid |

| Truncated-input tests | Check behavior when compressed data ends unexpectedly |

| Regression tests | Check that fixes do not break existing functionality |

| File-operation tests | Check path validation and overwrite handling |



\### E.3 Branch testing



Branch-testing results, commands, and coverage details are being prepared in a separate document by the designated teammate.



No branch-coverage percentage is claimed in this report. Only values produced by the actual coverage tool should be included in the separate branch-testing document.



\## F. Illustrative Processing Logic



The following pseudocode summarizes the expected high-level workflow. It is explanatory and is not a verbatim extract from the repository.



\### F.1 Compression workflow



```text

function compress(input\_path, output\_path):

&#x20;   validate\_input\_path(input\_path)

&#x20;   validate\_output\_path(output\_path)

&#x20;   confirm\_overwrite\_if\_required(output\_path)



&#x20;   input\_bytes = read\_file\_as\_bytes(input\_path)

&#x20;   frequencies = count\_byte\_frequencies(input\_bytes)



&#x20;   huffman\_tree = build\_huffman\_tree(frequencies)

&#x20;   code\_table = generate\_codes(huffman\_tree)



&#x20;   encoded\_bits = encode\_bytes(input\_bytes, code\_table)

&#x20;   header = create\_header(frequencies, encoded\_bits)



&#x20;   write\_compressed\_file(output\_path, header, encoded\_bits)

```



\### F.2 Decompression workflow



```text

function decompress(input\_path, output\_path):

&#x20;   validate\_input\_path(input\_path)

&#x20;   validate\_output\_path(output\_path)

&#x20;   confirm\_overwrite\_if\_required(output\_path)



&#x20;   compressed\_data = read\_compressed\_file(input\_path)

&#x20;   validate\_compressed\_data(compressed\_data)



&#x20;   header = parse\_header(compressed\_data)

&#x20;   huffman\_tree = rebuild\_huffman\_tree(header)



&#x20;   decoded\_bytes = decode\_bitstream(

&#x20;       compressed\_data,

&#x20;       huffman\_tree

&#x20;   )



&#x20;   write\_file\_as\_bytes(output\_path, decoded\_bytes)

```



\### F.3 Lossless validation



A basic lossless check compares the original file and the decompressed output byte-for-byte:



```text

original = read\_file\_as\_bytes(original\_path)

restored = read\_file\_as\_bytes(decompressed\_path)



assert original == restored

```



This check is useful for both text and binary files because it compares the underlying bytes rather than relying on text decoding.



\## G. Git and Integration Report



\### G.1 Integration



The GUI was developed on `feature/c-gui` and merged into `main` through PR #37.



The file-validation and overwrite-confirmation work was integrated through PR #35.



\### G.2 Repository workflow



The sprint used issue-based development and separate branches for feature work. Completed changes were integrated through pull requests so that the project’s main branch could contain the reviewed implementation.



\### G.3 Evidence



The repository provides the primary evidence for completed implementation work:



\- Issues #10–#16, #22–#24, #31, #33, and #34.

\- PR #35 for file validation and overwrite confirmation.

\- PR #37 for the GUI.

\- The project README’s recorded automated test result.



\## H. Team Handoff and Remaining Work



The Sprint 2 implementation and testing work listed in this report is complete. Branch-testing documentation is being prepared separately.



The teammate responsible for branch testing should:



1\. Start from the latest `main`.

2\. Run the agreed branch-testing procedure.

3\. Record the actual commands and tool output.

4\. Include only measured coverage values.

5\. Save the results in a separate Markdown document, such as `BRANCH\_TESTING.md`.

6\. Push the document on a separate branch and open a pull request.

7\. Notify the team when the pull request is ready for review.



Once the separate document is merged, this report can be updated with a link to it.



\## I. Sprint Conclusion



Sprint 2 improved the Huffman project’s reliability, validation, testing, portability, and usability. The sprint addressed important edge cases in compression and decompression, introduced additional safeguards for file operations, and integrated a graphical interface.



The completed work is recorded in the project issues and pull requests. Branch-testing results will be maintained separately so that the report does not claim coverage values that have not yet been verified.



