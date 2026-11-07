#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
"""Check imported files against the pinned local py32f0-template checkout."""
import argparse
import subprocess
from pathlib import Path

SNAPSHOT = "009bdec7ed77c9b782a258c064c227e15ec19498"
MODULE = Path(__file__).resolve().parents[1]
INTEGRATION = {
    ".editorconfig", "LICENSE", "README.md", "zephyr/module.yml",
    "tests/Makefile", "tests/test_registers.c", "tools/check_provenance.py",
}


def normalize(path):
    return "\n".join(line.rstrip() for line in path.read_text().splitlines()).rstrip()



def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("template", type=Path)
    args = parser.parse_args()
    template = args.template.resolve()
    revision = subprocess.check_output(
        ["git", "-C", str(template), "rev-parse", "HEAD"], text=True
    ).strip()
    if revision != SNAPSHOT:
        raise SystemExit(f"Expected template {SNAPSHOT}, found {revision}")
    if subprocess.check_output(
        ["git", "-C", str(template), "status", "--porcelain", "--untracked-files=all"],
        text=True,
    ).strip():
        raise SystemExit("Template checkout must be clean")
    mappings = {}
    for source, dest in (
        ("PY32F0xx_HAL_Driver/Inc", "drivers/include"),
        ("PY32F0xx_LL_Driver/Inc", "drivers/include"),
        ("CMSIS/Device/PY32F0xx/Include", "drivers/include"),
        ("PY32F0xx_HAL_Driver/Src", "drivers/src"),
        ("PY32F0xx_LL_Driver/Src", "drivers/src"),
    ):
        for path in (template / "Libraries" / source).iterdir():
            if path.is_file():
                mappings[f"py32/py32f0xx/{dest}/{path.name}"] = path
    mappings["py32/py32f0xx/drivers/include/py32f0xx_hal_conf.h"] = (
        template / "User/py32f0xx_hal_conf.h"
    )
    system = "py32/py32f0xx/soc/system_py32f0xx.c"
    mappings[system] = template / "Libraries/CMSIS/Device/PY32F0xx/Source/system_py32f0xx.c"
    errors = []
    vendor_count = integration_count = 0
    for path in sorted(MODULE.rglob("*")):
        if not path.is_file() or ".git" in path.relative_to(MODULE).parts:
            continue
        relative = path.relative_to(MODULE).as_posix()
        if relative in mappings:
            vendor_count += 1
            if normalize(path) != normalize(mappings[relative]):
                errors.append(f"Vendor mismatch: {relative}")
        elif relative in INTEGRATION:
            integration_count += 1
        else:
            errors.append(f"Unclassified file: {relative}")
    if vendor_count != 125 or integration_count != len(INTEGRATION) or not (MODULE / system).is_file():
        errors.append("Imported or integration files are missing")
    if errors:
        raise SystemExit("\n".join(errors))
    print(f"PASS: {vendor_count} normalized template matches, "
          f"{integration_count} explicitly identified metadata/test/tool files; template {revision}")


if __name__ == "__main__":
    main()
