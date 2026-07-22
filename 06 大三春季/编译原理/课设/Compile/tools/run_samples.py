#!/usr/bin/env python3

from __future__ import annotations

import datetime
import subprocess
import sys
from pathlib import Path


def main() -> int:
  repo_root = Path(__file__).resolve().parent.parent
  pl0c = repo_root / "build" / "pl0c"
  pl0run = repo_root / "build" / "pl0run"
  samples_dir = repo_root / "tests" / "samples"
  report_path = repo_root / "tests" / "sample_report.txt"

  missing = [cmd for cmd in (pl0c, pl0run) if not cmd.exists()]
  if missing:
    print(f"[run_samples] Missing binaries: {', '.join(str(p) for p in missing)}", file=sys.stderr)
    print("Please build the project (e.g. `cmake --build build`) before running this script.", file=sys.stderr)
    return 1

  sample_files = sorted(samples_dir.glob("*.pl0"))
  if not sample_files:
    print(f"[run_samples] No .pl0 files found in {samples_dir}", file=sys.stderr)
    return 1

  lines: list[str] = []
  now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
  lines.append("PL/0 Sample Batch Report")
  lines.append(f"Generated: {now}")
  lines.append("")

  for sample in sample_files:
    name = sample.name
    expects_failure = name.startswith("error_")
    pcode_path = sample.with_suffix(".pcode")

    lines.append(f"=== {name} ===")
    lines.append("-- Source --")
    lines.append(sample.read_text().rstrip())
    lines.append("")

    compile_cmd = [str(pl0c), str(sample), "-o", str(pcode_path)]
    compile_proc = subprocess.run(compile_cmd, capture_output=True, text=True)

    stderr = compile_proc.stderr.strip()
    if stderr:
      lines.append("-- Compile Diagnostics --")
      lines.append(stderr)
      lines.append("")

    if compile_proc.returncode != 0:
      outcome = "expected failure" if expects_failure else "compilation failed"
      lines.append(f"-- Result --\n{outcome}")
      lines.append("")
      continue

    if pcode_path.exists():
      lines.append("-- P-Code --")
      lines.append(pcode_path.read_text().rstrip())
      lines.append("")

    run_proc = subprocess.run([str(pl0run), str(pcode_path)],
                              capture_output=True, text=True)
    output_chunks = []
    if run_proc.stdout.strip():
      output_chunks.append(run_proc.stdout.strip())
    if run_proc.stderr.strip():
      output_chunks.append(run_proc.stderr.strip())

    lines.append("-- Output --")
    lines.append("\n".join(output_chunks) if output_chunks else "(no output)")
    lines.append("")

  report_path.write_text("\n".join(lines).rstrip() + "\n")
  print(f"[run_samples] Report written to {report_path}")
  return 0


if __name__ == "__main__":
  sys.exit(main())
