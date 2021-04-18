
# zxzerodiag - Raspberry Pi Zero Testing Software

# Using zxzerodiag

Full instructions for ```zxzerodiag``` can be found [here](https://github.com/hermitretro/hermitretro-full/raw/main/src/zxzero/doc/diagnostics/Hermit%20Retro%20ZXZero%20-%20Diagnostics%20User%20Guide%20-%20V1.0.pdf)

# Building zxzerodiag

```zxzerodiag``` uses [CMake](https://cmake.org) to build. After downloading or cloning, you
should be able to build as follows:

```
% cd zxzerodiag-<commit_or_branch_or_cloned_repo>
% mkdir build
% cd build
% cmake ..
% make
% ./zxzerodiag
```

# Licensing

* `bcm2835.c` and `bcm2835.h` are covered by GNU GPL V3
* Everything else within this package is covered by the
  [GNU General Public License V3](LICENSE.txt).
