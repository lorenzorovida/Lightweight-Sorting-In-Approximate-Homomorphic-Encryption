# Somewhat efficient blind recommender system for the MovieLens dataset
>Custom branch of "Lightweight sorting in approximate homomorphic encryption" which is used as an implementation of this [extended abstract](lorenzorovida.github.io/>abstracts/2026_recommendersystem.pdf), presented at AICrypt '26.

[![CMake on multiple platforms](https://github.com/lorenzorovida/Lightweight-Sorting-In-Approximate-Homomorphic-Encryption/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/lorenzorovida/Lightweight-Sorting-In-Approximate-Homomorphic-Encryption/actions/workflows/cmake-single-platform.yml)
<br>
<a href="https://eprint.iacr.org/2025/1150"><img src="imgs/preprint_icon.svg" alt="Console presentation image" ></a>
<br>

---
This branch contains a toy implementations of [this idea](lorenzorovida.github.io/>abstracts/2026_recommendersystem.pdf). After installing OpenFHE, it can be executed by

```
mkdir build
cd build
cmake ..
make
./Sort
```

This will run an example of finding the top-2 elements in an array of size 256 (it can be tweaked). For any question please open an issue (so everyone can see it).

N.b. this is a proof-of-concept just to play around, it is far from being optimized and the parameters do not even satisfy 128-bits of security.
