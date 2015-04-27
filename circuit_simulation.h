/* Project for class, Algorithms for Circuit Simulation (2012 - 2013) */

#ifndef _CIRCUIT_SIMULATION_
  
#define _CIRCUIT_SIMULATION_

#include "sparse_helpers.h"

#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <cblas.h>
#include "types.h"
#include "avl_tree_string_mapping.h"


int number_of_elements[ELEMENTS]; // helper array for counting each element type

int group1_elements, group2_elements; // Group 1: R, C, I  / Group 2: V, L

int non_zeroes_G;
int non_zeroes_C;
int non_zeroes_Gcomplex;

extern twoTerminalsElement *head_twoTerminalsElement_list, *last_twoTerminalsElement_list;
extern threeTerminalsElement *head_threeTerminalsElement_list, *last_threeTerminalsElement_list;
extern fourTerminalsElement *head_fourTerminalsElement_list, *last_fourTerminalsElement_list;

extern twoTerminalsElement** sources;

/* Double routines */
extern double *matrix_G;
extern double *matrix_C;
extern double *temp_matrix;
extern double *temp_matrix2;
extern double *vector_b;
extern double *vector_y;
extern double *vector_x;
extern double *inv_preconditioner;

extern cs_di *G2;
extern cs_di *C2;
extern cs_di *temp_mat;
extern cs_di *temp_mat2;
extern cs_dis *S;
extern cs_din *N;
/******************/

extern int *transposition;



/* Complex routines */
extern double complex *matrix_Gcomplex;

extern cs_ci *G2complex;
extern cs_cis *Scomplex;
extern cs_cin *Ncomplex;

extern double complex *vector_b_complex;
extern double complex *vector_x_complex;
extern double complex *vector_y_complex;

extern double complex *inv_preconditioner_complex;
/**********************/

/*This function initializes the simulation */
void init(void);
/*This function parses the input file and creates 3 linked-lists (for two,three and four terminal elements
that hold the circuit elements.Also,checks whether the node 0 found in the input file */
void parseInput(FILE *p_file);
/* Checks if the string is a number */
double invalid_number_checker(char *);
/* Creates the DC Matrix */
void createMnaSystemDC(void);
/* Creates the Transient Matrix */
void createMnaSystemTransient(void);
/* Creates the AC Matrix */
void createMnaSystemAC(double f, long int run);
/*This function inserts a new two terminals element to the list*/
void insertTwoTerminalsElement(twoTerminalsElement *current);
/*This function inserts a new three terminals element to the list*/
void insertThreeTerminalsElement(threeTerminalsElement *current);
/*This function inserts a new four terminals element to the list*/
void insertFourTerminalsElement(fourTerminalsElement *current);
/* Clean up */
void freeAllocations1(void);
void freeAllocations2(void);
/* LU Decomposition for linear systems */
void LUDecomposition(void);
void LUDecompositionComplex(long int run);
/* line interchange for LU */
void interchange(int from, int to);
void interchangeComplex(int from, int to);
/* Cholesky Decomposition for linear systems (Symmetric(Hermitian) Positive Definite Matrices) */
void CholeskyDecomposition(void);
void CholeskyDecompositionComplex(long int run);
/* Solves a linear system, Ax=b, after LU or Cholesky has taken place */
void solve_direct_methods();
void solve_direct_methods_Complex();
/* forward substitution for linear systems */
void forward_substitution(void);
void forward_substitution_Complex(void);
/* backward substitution for linear systems */
void backward_substitution(void);
void backward_substitution_Complex(void);
/* Creates the inverse preconditioner for the iterative solving methods, M = 1 /diag(G) */
void createInversePreconditioner(int length);
void createInversePreconditionerComplex(int length);
/* An Iterative method for solving the system Ax=b, (General Purpose Matrices) */
void BiConjugateGradient(int zero_x);
void BiConjugateGradientComplex(int zero_x);
/* An Iterative method for solving the system Ax=b, (Symmetric(Hermitian) Positive Definite Matrices) */
void ConjugateGradient(int zero_x);
void ConjugateGradientComplex(int zero_x);
/* x = inv(A) * b, when A is diagonal */
void solve_diagonal_system(double *vector1, double *vector2, double *result, int length);
void solve_diagonal_system_Complex(double complex *vector1, double complex *vector2, double complex *result, int length, int hermitian);
/* Creates the values for every non dc source, for every time step */
void createTransientSteps(transientComponent*  trans);
/* Trapezoidal method for solving differential equations */
void Trapezoidal(double *vector_e, double *vector_e_1);
/* Backward Euler method for solving differential equations */
void BackwardEuler(double *vector_e);
/* Forward Euler method for solving differential equations */
//void ForwardEuler(double *vector_e, double *temp_vec);
/* DC Analysis of the circuit */
void DCAnalysis(void);
/* Transient Analysis of the circuit */
void TransientAnalysis(void);
/* DC Sweep of the circuit */
void DCSweep(void);
/* AC Analysis of the circuit */
void ACAnalysis(void);
/* Prints the help */
void Help(void);
/* Displays the graph */
void Graph(void);
/* Displays the progress bar */
void DrawProgressBar(int len, double percent);
/* Helper function for the logspace points */
double calculate_next_decade(double decade);


#endif
