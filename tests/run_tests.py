"""Run Huffman integration tests, optionally against a user-supplied file.

Usage:
  python tests/run_tests.py [input_file]

With input_file, the runner checks frequency counting, tree/code generation,
compression, decompression, and byte-for-byte integrity for that file.
Without input_file, it also runs a built-in edge-case test suite.
Results are written to tests/results.txt.
"""
import filecmp
import os
import random
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TESTS = os.path.join(ROOT, "tests")
EXE = os.path.join(ROOT, "hzip.exe" if os.name == "nt" else "hzip")
SRC = [os.path.join(ROOT, "src", f) for f in ("main.c", "huffman.c", "file_io.c")]
C_TESTS = {
    "frequency": os.path.join(TESTS, "test_frequency.c"),
    "tree": os.path.join(TESTS, "test_tree.c"),
    "codes": os.path.join(TESTS, "test_codes.c"),
}


def build(cc, sources, output):
    cmd = [cc, "-std=c99", "-Wall", "-Wextra", "-O2", "-o", output] + sources
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode:
        raise RuntimeError("Build failed:\n" + r.stderr)


def run(exe, *args):
    return subprocess.run([exe, *map(str, args)], capture_output=True, text=True)


def make_inputs(directory):
    rnd = random.Random(42)
    cases = {
        "empty.bin": b"",
        "single_byte.bin": b"A",
        "single_symbol_repeated.bin": b"z" * 10000,
        "two_symbols.bin": b"ab" * 5000,
        "short_text.txt": b"hello huffman world",
        "english.txt": b"the quick brown fox jumps over the lazy dog. " * 500,
        "all_bytes.bin": bytes(range(256)) * 40,
        "random.bin": bytes(rnd.getrandbits(8) for _ in range(30000)),
    }
    paths = []
    for name, data in cases.items():
        path = os.path.join(directory, name)
        with open(path, "wb") as f:
            f.write(data)
        paths.append(path)
    return paths


def test_one_file(path, results, c_test_exes):
    if not os.path.isfile(path):
        results.append((f"input exists: {path}", False, "File not found"))
        return
    path = os.path.abspath(path)
    with open(path, "rb") as f:
        data = f.read()
    results.append((f"input readable: {os.path.basename(path)}", True,
                    f"{len(data):,} bytes"))

    for label, exe in c_test_exes.items():
        r = run(exe, path)
        results.append((f"C {label} test on {os.path.basename(path)}",
                        r.returncode == 0,
                        (r.stdout + r.stderr).strip() or f"exit code {r.returncode}"))

    compressed, restored = path + ".huf-test", path + ".restored-test"
    c = run(EXE, "compress", path, compressed)
    if c.returncode != 0:
        results.append((f"compress {os.path.basename(path)}", False, c.stderr.strip()))
        return
    results.append((f"compress {os.path.basename(path)}", True, c.stdout.strip()))
    d = run(EXE, "decompress", compressed, restored)
    ok = d.returncode == 0 and os.path.isfile(restored) and filecmp.cmp(path, restored, shallow=False)
    results.append((f"decompress and byte-compare {os.path.basename(path)}", ok,
                    d.stdout.strip() if d.returncode == 0 else d.stderr.strip()))
    for p in (compressed, restored):
        try: os.remove(p)
        except OSError: pass


def main():
    cc = shutil.which("gcc") or shutil.which("cc")
    if not cc:
        sys.exit("No C compiler found. Install GCC/MinGW and ensure it is on PATH.")
    try:
        build(cc, SRC, EXE)
        c_test_exes = {}
        with tempfile.TemporaryDirectory(prefix="huffman-tests-") as build_dir:
            for label, source in C_TESTS.items():
                output = os.path.join(build_dir, "test_" + label + (".exe" if os.name == "nt" else ""))
                build(cc, [source, os.path.join(ROOT, "src", "huffman.c"),
                           os.path.join(ROOT, "src", "file_io.c")], output)
                c_test_exes[label] = output
            results = []
            if len(sys.argv) > 1:
                if len(sys.argv) > 2:
                    sys.exit("Usage: python tests/run_tests.py [input_file]")
                test_one_file(sys.argv[1], results, c_test_exes)
            else:
                with tempfile.TemporaryDirectory(prefix="huffman-inputs-") as inputs_dir:
                    for path in make_inputs(inputs_dir):
                        test_one_file(path, results, c_test_exes)
            failed = sum(not ok for _, ok, _ in results)
            lines = [f"[{'PASS' if ok else 'FAIL'}] {name} -- {info}" for name, ok, info in results]
            lines.append(f"\n{len(results) - failed}/{len(results)} checks passed")
            report = "\n".join(lines)
            print(report)
            with open(os.path.join(TESTS, "results.txt"), "w", encoding="utf-8") as f:
                f.write(report + "\n")
            sys.exit(1 if failed else 0)
    except RuntimeError as e:
        sys.exit(str(e))


if __name__ == "__main__":
    main()
