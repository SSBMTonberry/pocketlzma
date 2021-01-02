# pocketlzma
A singleheader LZMA library for C++11

# Benchmarks

**Specs:**
- **CPU**: Intel Core i7-6700 @ 8x 4GHz
- **GPU**: GeForce GTX 1080
- **RAM**: 15969MiB
- **OS**: Linux (Manjaro 20.2 Nibia) - **Kernel**: x86_64 Linux 4.14.209-1-MANJARO

## .json compression benchmark (20 runs)
| Preset  | Size before (bytes) | Size after (bytes) | Average time | Min. time |Max. time |
| :-----: |:------------------: | :-----------------:| :---------------: |:------------:|:------------:| 
| Fastest |70230                | 3364               | 2.25587 ms        | 1.99016 ms   | 3.17568 ms   | 
| Fast    |70230                | 3283               |                   |              |              | 


// C O M P R E S S I O N   B E N C H M A R K  
// -------------------------------------------

Number of runs: 20
Total time: 2035.62ms

Fastest - time: average=2.25587ms min=1.99016ms max=3.17568ms - Size (bytes): 70230->3364
Fast - time: average=2.29653ms min=2.02276ms max=3.54542ms - Size (bytes): 70230->3283
Default - time: average=20.7517ms min=19.3443ms max=25.8784ms - Size (bytes): 70230->2693
Good - time: average=30.6193ms min=28.5697ms max=38.3485ms - Size (bytes): 70230->2485
Best - time: average=45.8248ms min=43.4147ms max=57.934ms - Size (bytes): 70230->2451

// D E C O M P R E S S I O N   B E N C H M A R K  
// -----------------------------------------------

Number of runs: 20
Total time: 44.7639ms

Fastest - time: average=0.502977ms min=0.497585ms max=0.536352ms
Fast - time: average=0.492965ms min=0.488002ms max=0.50396ms
Default - time: average=0.42965ms min=0.423914ms max=0.448674ms
Good - time: average=0.405111ms min=0.402765ms max=0.419498ms
Best - time: average=0.401403ms min=0.400253ms max=0.406537ms

// C O M P R E S S I O N ( . S L P )   B E N C H M A R K  
// -------------------------------------------

Number of runs: 5
Total time: 42639.5ms

Fastest - time: average=253.102ms min=232.259ms max=330.66ms - Size (bytes): 4145823->702789
Fast - time: average=355.459ms min=321.867ms max=455.138ms - Size (bytes): 4145823->677754
Default - time: average=1889.8ms min=1852.6ms max=1993.94ms - Size (bytes): 4145823->572742
Good - time: average=2888.82ms min=2879.99ms max=2915.07ms - Size (bytes): 4145823->521168
Best - time: average=3140.49ms min=3107.47ms max=3168.36ms - Size (bytes): 4145823->520358

// D E C O M P R E S S I O N ( . S L P )   B E N C H M A R K  
// -----------------------------------------------

Number of runs: 5
Total time: 1972.63ms

Fastest - time: average=85.5723ms min=84.871ms max=87.2708ms
Fast - time: average=81.7042ms min=81.4525ms max=82.052ms
Default - time: average=78.0439ms min=77.7491ms max=78.7667ms
Good - time: average=74.8225ms min=74.1976ms max=76.2949ms
Best - time: average=74.3353ms min=74.1313ms max=74.5187ms
