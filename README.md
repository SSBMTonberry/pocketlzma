# pocketlzma
A singleheader LZMA library for C++11

# Benchmark

Specs:
CPU: Intel Core i7-6700 @ 8x 4GHz
GPU: GeForce GTX 1080
RAM: 15969MiB
OS: Linux (Manjaro 20.2 Nibia) - Kernel: x86_64 Linux 4.14.209-1-MANJARO


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