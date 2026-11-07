# Puya PY32 HAL

This repository contains Puya HAL/LL drivers and CMSIS device headers imported
from [IOsetting/py32f0-template](https://github.com/IOsetting/py32f0-template).
The vendor snapshot is commit `009bdec7ed77c9b782a258c064c227e15ec19498`
(2026-02-08).

The import maps these upstream directories into `py32/py32f0xx`:

| Upstream directory under `Libraries/` | Module directory |
| --- | --- |
| `PY32F0xx_HAL_Driver/Inc`, `PY32F0xx_LL_Driver/Inc` | `drivers/include` |
| `PY32F0xx_HAL_Driver/Src`, `PY32F0xx_LL_Driver/Src` | `drivers/src` |
| `CMSIS/Device/PY32F0xx/Include` | `drivers/include` |
| `CMSIS/Device/PY32F0xx/Source/system_py32f0xx.c` | `soc/system_py32f0xx.c` |

The shared drivers target PY32F002A, PY32F003 and PY32F030. The complete
upstream CMSIS device-header set is retained, including PY32F002B and
PY32F071/072 headers; those parts need their separate driver/system packages.
The repository does not bundle CMSIS core headers, standalone startup assembly,
linker scripts, BSPs or examples.

Vendor files retain their notices and implementation, including `SystemInit()`.
Imported text uses LF line endings, has no trailing whitespace and ends with a
newline. `drivers/include/py32f0xx_hal_conf.h` comes from the template's
`User/py32f0xx_hal_conf.h`.

Provenance checking uses a clean checkout of the pinned template:

```sh
python3 tools/check_provenance.py ../py32f0-template
```

The checker compares 125 imported files with their explicit template paths,
normalizing line endings and trailing whitespace. All remaining files are
explicitly listed as metadata, tests or tooling; unknown files fail the check.
This verifies the template origin, not its own chain of authorship or licensing.

`zephyr/module.yml` delegates CMake and Kconfig integration to the Zephyr tree's
`modules/hal_py32` directory. No Zephyr-specific CMake or startup implementation
is kept in this repository. The module has no STM32 compatibility headers,
peripheral-name aliases or CMSIS core fork.

Known upstream limits: IRDA uses register definitions absent from the shared
device headers and requires DMA types. LL flash redeclares HAL flash types and
cannot be compiled with `USE_HAL_DRIVER` and HAL flash enabled.

Some template files carry both Puya and STMicroelectronics copyright notices.
Those notices are retained. [LICENSE](LICENSE) inventories the licenses; file
notices take precedence. Cortex headers must come from a CMSIS core package.

Run the RAM-backed clock and EXTI/peripheral-clock regression tests with CMSIS
core headers available locally:

```sh
make -C tests run CMSIS_INCLUDE=/path/to/CMSIS/Core/Include
```
