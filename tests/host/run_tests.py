#!/usr/bin/env python3
"""Compile and run the host-side firmware tests (no Arduino hardware needed).

Needs a C++17 compiler: g++, clang++, or `pip install ziglang` (used automatically as a fallback).
Usage:  python tests/host/run_tests.py
"""
import pathlib
import shutil
import subprocess
import sys
import tempfile

HERE = pathlib.Path(__file__).resolve().parent
TESTS = ["test_prototype", "test_ami_meter"]


def find_cxx():
    for name in ("g++", "clang++", "c++"):
        if shutil.which(name):
            return [name]
    try:
        import ziglang  # noqa: F401
        return [sys.executable, "-m", "ziglang", "c++"]
    except ImportError:
        sys.exit("No C++ compiler found. Install g++/clang++ or run: pip install ziglang")


def main():
    cxx = find_cxx()
    out = pathlib.Path(tempfile.mkdtemp(prefix="ami_tests_"))
    rc = 0
    for t in TESTS:
        exe = out / (t + (".exe" if sys.platform == "win32" else ""))
        cmd = cxx + ["-std=c++17", "-O0", "-w", "-I", str(HERE / "mocks"), "-o", str(exe), str(HERE / (t + ".cpp"))]
        build = subprocess.run(cmd, capture_output=True, text=True)
        if build.returncode:
            print(f"BUILD FAILED: {t}\n{build.stdout}{build.stderr}")
            rc = 1
            continue
        run = subprocess.run([str(exe)], capture_output=True, text=True)
        print(run.stdout, end="")
        if run.returncode:
            print(run.stderr, end="")
            rc = 1
        print()
    sys.exit(rc)


if __name__ == "__main__":
    main()
