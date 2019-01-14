Short Description:
The executable can parse a netlist containing linear elements such as, voltage sources, current sources, resistances, inductors and capacitors. It creates the Modified Nodal Analysis Matrix and solves the system using direct methods, such as LU or Cholesky, or iterative methods, such as BiConjugate Gradient or Conjugate Gradient. It also supports sparse matrices for large circuits (dropzone.tamu.edu/~pli/PGBench).
It can perform dc analysis, dc sweep, transient analysis, ac analysis (Including graphs for dc sweep, transient analysis, and ac analysis).

Dependencies:
Download the "libblas" package.
Browse into Sparse/CXSparse directory and use the following commands to build the sparse library.

make clean
make install
make

Compile:
gcc -O3 *.c -o circuit_simulation -lblas -lm -L ./Sparse/CXSparse/Lib -lcxsparse

or use compile.sh

Third party applications for graphs:
sudo apt-get install gnuplot
sudo apt-get install xdot

Help:
./circuit_simulation -help
