Short Description:
The executable can parse a netlist containing linear elements such as, voltage sources, current sources, resistances, inductors and capacitors. It creates the Modified Nodal Analysis Matrix and solves the system using direct methods, such as LU or Cholesky, or iterative methods, such as BiConjugate Gradient or Conjugate Gradient. It also supports sparse matrices for large circuits (dropzone.tamu.edu/~pli/PGBench).
It can perform dc analysis, dc sweep and transient analysis (Including graphs for dc sweep and transient).

Compile:
gcc *.c -o circuit_simulation -lm -fopenmp -lblas

Help:
./circuit_simulation -help