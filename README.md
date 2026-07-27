# PharmaServe

**PharmaServe** is a C++ simulation of automated order fulfillment from an order queue, to be integrated with Automated Storage/Retrieval System.

---

[1. Roadmap](#Roadmap)  
[2. Installation](#Installation)  

---

## Roadmap
- [x] Data is ingested into Order Queue.
- [x] Workers interact with queue atomically.
- [x] Worker Manager dispatches new workers.
- [ ] Order generator uses Order Queue API.

## Installation
<details>
<summary><strong>Docker</strong></summary>

1. **Clone repo and build image**
```sh
git clone https://github.com/OwenM31/PharmaServe

cd PharmaServe                  # Enter repo directory
docker build -t pharmaserve .   # Build the image
```
2. **Run a container**
```sh
docker run --rm -it pharmaserve
```
>Ex. Run with CLI arguments
```sh
docker run --rm -it pharmaserve --workers 3 --max-queue 50
```
</details>
<details>
<summary><strong>CMake</strong></summary>

1. **Install CMake**  
https://cmake.org/download/


2. **Clone repo and build executables**
```sh
git clone https://github.com/OwenM31/PharmaServe

cd PharmaServe      # Enter the repo directory

mkdir build         
cd build
cmake ..            # Configure build

cmake --build .     # Build executables
cd ..
```
**Main**: `build/pharmaserve.exe`  
**Tests**: `build/tests/pharmaserve_tests.exe`  
</details>

