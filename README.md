# CICIND damping parameter

This repository holds the damping parameter as mentioned in the CICIND commentaries and appendices for model code for steel chimneys.
The table can be found in KA_mean.csv.

![image-aerodynamic-damping-paraketer](https://github.com/jimmyberg/CICIND-Ka/blob/0f396a4453081dfe2880cbf47d19114f26d4d5d5/Aerodynamic%20damping%20parameter.svg)

## Example C++ code

An example is included to read and interpolate values in the table using 3d cubic interpolation.

## Run code

The code is written in a Linux environment. Make sure you have build essentials installed.
```
sudo apt-get install build-essentials
```

Then compile and run the code by executing:
```
./compileAndRun
```

You can also run this code in any other environment.
