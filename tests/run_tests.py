"""Integration tests: compress -> decompress round trips plus error handling.

Usage:  python tests/run_tests.py
Writes a summary to tests/results.txt
"""
import filecmp
import os
import random
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE = os.path.join(ROOT, "hzip.exe" if os.name == "nt" else "hzip")
SRC = [os.path.join(ROOT, "src", f) for f in ("main.c", "huffman.c", "file_io.c")]


def build():
    cc = shutil.which("gcc") or shutil.which("cc")
    if not cc:
        sys.exit("No C compiler found (install gcc / MinGW).")
    cmd = [cc, "-std=c99", "-Wall", "-Wextra", "-O2", "-o", EXE] + SRC
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode != 0:
        sys.exit("Build failed:\n" + r.stderr)


def run(*args):
    return subprocess.run([EXE, *args], capture_output=True, text=True)


def make_inputs(d):
    rnd = random.Random(42)
    cases = {
        "empty.bin": b"",
        "single_byte.bin": b"A",
        "single_symbol_repeated.bin": b"z" * 10000,
        "two_symbols.bin": b"ab" * 5000,
        "short_text.txt": b"hello huffman world",
        "english.txt": (b"the quick brown fox jumps over the lazy dog. " * 500),
        "skewed.bin": bytes(rnd.choices(range(256), weights=[2 ** (i % 12) for i in range(256)], k=50000)),
        "all_bytes.bin": bytes(range(256)) * 40,
        "random.bin": bytes(rnd.getrandbits(8) for _ in range(30000)),
    }
    paths = {}
    for name, data in cases.items():
        p = os.path.join(d, name)
        with open(p, "wb") as f:
            f.write(data)
        paths[name] = p
    return paths


def main():
    build()
    results, failed = [], 0
    with tempfile.TemporaryDirectory() as d:
        for name, src in make_inputs(d).items():
            comp, back = src + ".huf", src + ".out"
            ok = (run("compress", src, comp).returncode == 0
                  and run("decompress", comp, back).returncode == 0
                  and filecmp.cmp(src, back, shallow=False))
            sizes = f"{os.path.getsize(src)} -> {os.path.getsize(comp)}" if os.path.exists(comp) else "n/a"
            results.append((f"round-trip {name}", ok, sizes))

        src = os.path.join(d, "english.txt")
        comp = src + ".huf"

        r = run("compress", os.path.join(d, "does_not_exist"), os.path.join(d, "x"))
        results.append(("missing input file is rejected", r.returncode != 0, r.stderr.strip()))

        junk = os.path.join(d, "junk.huf")
        with open(junk, "wb") as f:
            f.write(b"this is not a huffman file" * 100)
        r = run("decompress", junk, os.path.join(d, "x"))
        results.append(("invalid compressed file is rejected", r.returncode != 0, r.stderr.strip()))

        trunc = os.path.join(d, "trunc.huf")
        with open(comp, "rb") as f, open(trunc, "wb") as g:
            data = f.read()
            g.write(data[:-50])
        r = run("decompress", trunc, os.path.join(d, "x"))
        results.append(("truncated compressed file is rejected", r.returncode != 0, r.stderr.strip()))

        r = run("compress")
        results.append(("no arguments prints usage", r.returncode == 2, "exit code %d" % r.returncode))

    lines = []
    for name, ok, info in results:
        failed += not ok
        lines.append(f"[{'PASS' if ok else 'FAIL'}] {name}  {info}")
    lines.append(f"\n{len(results) - failed}/{len(results)} tests passed")
    report = "\n".join(lines)
    print(report)
    with open(os.path.join(ROOT, "tests", "results.txt"), "w") as f:
        f.write(report + "\n")
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()
