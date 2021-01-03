# pocketlzma
A singleheader LZMA library for C++11

# Benchmarks

### **Specs:**
- **CPU**: Intel Core i7-6700 @ 8x 4GHz
- **GPU**: GeForce GTX 1080
- **RAM**: 15969MiB
- **OS**: Linux (Manjaro 20.2 Nibia) - **Kernel**: x86_64 Linux 4.14.209-1-MANJARO

## .json compression benchmark (20 runs)
| Preset          | Size before | Size after | Average time      | Min. time    |Max. time     |
| :-----:         |:----------: | :---------:| :---------------: |:------------:|:------------:|
| Fastest         |70230 bytes  | 3364 bytes |  2.25587 ms       | 1.99016 ms   | 3.17568 ms   |
| Fast            |70230 bytes  | 3283 bytes |  2.29653 ms       | 2.02276 ms   | 3.54542 ms   |
| Default         |70230 bytes  | 2693 bytes |  20.7517 ms       | 19.3443 ms   | 25.8784 ms   |
| GoodCompression |70230 bytes  | 2485 bytes |  30.6193 ms       | 28.5697 ms   | 38.3485 ms   |
| BestCompression |70230 bytes  | 2451 bytes |  45.8248 ms       | 43.4147 ms   | 57.934  ms   |

## .json decompression benchmark (20 runs)
| Preset (when compressed)| Size before | Size after | Average time      | Min. time    |Max. time     |
| :-----:                 |:----------: | :---------:| :---------------: |:------------:|:------------:|
| Fastest                 |3364 bytes   | 70230 bytes|  0.502977 ms      | 0.497585 ms  | 0.536352 ms  |
| Fast                    |3283 bytes   | 70230 bytes|  0.492965 ms      | 0.488002 ms  | 0.50396 ms   |
| Default                 |2693 bytes   | 70230 bytes|  0.42965 ms       | 0.423914 ms  | 0.448674 ms  |
| GoodCompression         |2485 bytes   | 70230 bytes|  0.405111 ms      | 0.402765 ms  | 0.419498 ms  |
| BestCompression         |2451 bytes   | 70230 bytes|  0.401403 ms      | 0.400253 ms  | 0.406537 ms  |

## .slp (binary file) compression benchmark (5 runs)
| Preset          | Size before | Size after | Average time      | Min. time    |Max. time     |
| :-----:         |:----------: | :---------:| :---------------: |:------------:|:------------:|
| Fastest         |4145823 bytes|702789 bytes|  253.102 ms       | 232.259 ms   | 330.66 ms    |
| Fast            |4145823 bytes|677754 bytes|  355.459 ms       | 321.867 ms   | 455.138 ms   |
| Default         |4145823 bytes|572742 bytes|  1889.8  ms       | 1852.6 ms    | 1993.94 ms   |
| GoodCompression |4145823 bytes|521168 bytes|  2888.82 ms       | 2879.99 ms   | 2915.07 ms   |
| BestCompression |4145823 bytes|520358 bytes|  3140.49 ms       | 3107.47 ms   | 3168.36 ms   |

## .slp (binary file) decompression benchmark (5 runs)
| Preset (when compressed)| Size before | Size after  | Average time      | Min. time    |Max. time     |
| :-----:                 |:----------: | :----------:| :---------------: |:------------:|:------------:|
| Fastest                 |702789 bytes |4145823 bytes|  85.5723 ms       | 84.871 ms    | 87.2708 ms   |
| Fast                    |677754 bytes |4145823 bytes|  81.7042 ms       | 81.4525 ms   | 82.052 ms    |
| Default                 |572742 bytes |4145823 bytes|  78.0439 ms       | 77.7491 ms   | 78.7667 ms   |
| GoodCompression         |521168 bytes |4145823 bytes|  74.8225 ms       | 74.1976 ms   | 76.2949 ms   |
| BestCompression         |520358 bytes |4145823 bytes|  74.3353 ms       | 74.1313 ms   | 74.5187 ms   |

