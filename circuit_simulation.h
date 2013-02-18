/* Project for class, Algorithms for Circuit Simulation (2012 - 2013) */

#ifndef _CIRCUIT_SIMULATION_
  
#define _CIRCUIT_SIMULATION_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <cblas.h>
#include "csparse.h"


#define DEBUG 0

#define VOLTAGE_SOURCE 0
#define CURRENT_SOURCE 1
#define RESISTANCE 2
#define CAPACITOR 3
#define INDUCTOR 4
#define DIODE 5
#define BJT 6
#define MOS 7

#define SWEEP_VOLTAGE_SOURCE 1
#define SWEEP_CURRENT_SOURCE 2

#define NORMAL 0
#define SPARSE 1

#define NONSPD 0
#define SPD 1

#define DIRECT 0
#define ITERATIVE 1

#define EULER 0
#define TRAPEZOIDAL 1

#define EXP 0
#define SIN 1
#define PULSE 2
#define PWL 3

#define LINES_POINTS 1
#define LINES 2

#define SIZE_OF_LINE 3000
#define ELEMENTS 8
#define HASH_TABLE_SIZE 1024

#define ZERO 1e-12


struct ExpImpulse { // spice format exp inputs
    double i1;
    double i2;
    double td1;
    double tc1;
    double td2;
    double tc2;
};


struct SinImpulse { // spice format sin inputs
    double i1;
    double ia;
    double fr;
    double td;
    double df;
    double ph;
};

struct PulseImpulse { // spice format pulse inputs
    double i1;
    double i2;
    double td;
    double tr;
    double tf;
    double pw;
    double per;
};

struct PWLImpulse { // spice format piece wise linear inputs
   int length;
   double *t;
   double *i;
};

struct transientComponent {
    int impulse; // type of impulse, exp, sin, pulse, or piece wise linear
    struct ExpImpulse *exp;
    struct SinImpulse *sin;
    struct PulseImpulse *pulse;
    struct PWLImpulse *pwl;
    double *vals; // precomputed values for the transient analysis
};

struct twoTerminalsElement {
    int type; // V, I, R, C, L, (Diode just parsed, not used)
    int name; // Element name
    char string_name[256];  // Original given name
    int positive_terminal; // positive terminal
    int negative_terminal; // negative terminal
    double value; // Element value, according to type, SI
    struct transientComponent *transient; // transient part, for V, I, null if DC, or not V, I
    struct twoTerminalsElement *next;
};

struct threeTerminalsElement { // Just for parsing, not used
    int type;
    int name;
    char string_name[256];
    int c_terminal;
    int b_terminal;
    int e_terminal;
    double value;
    struct threeTerminalsElement *next;
};

struct fourTerminalsElement { // Just for parsing, not used
    int type;
    int name;
    char string_name[256];
    int d_terminal;
    int g_terminal;
    int s_terminal;
    int b_terminal;
    double length;
    double width;
    struct fourTerminalsElement *next;
};

struct hash_node {  // Node hashing
    char name[256];
    int mapping;
    struct hash_node *next;
};

int number_of_elements[ELEMENTS]; // helper array for counting each element type

int number_of_nodes; // number of nodes, not counting ground
int group1_elements, group2_elements; // Group 1: R, C, I  / Group 2: V, L

int matrix_sparsity; // Sparse or Normal Matrices
int matrix_type; // General Purpose, or Symmetric Positive Definite Matrices
int method; // Direct or Iterative Methods for solving Ax=b

int operating_point; // print currents at DC_Analysis.txt

int gnuplot; // plot the results using gnuplot (needs to be installed)

int transient_analysis; // transient analysis toggle
int diff_method; // differential equation solving method, Trapezoidal or Backward Euler

int non_zeroes_G;
int non_zeroes_C;

double time_step; // time step for the transient analysis
double fin_time; // end time for the transinet analysis (start time = 0)


double dc_sweep_start; // dc sweep start value
double dc_sweep_end ; // dc sweep end value
double dc_sweep_step; // dc sweep step value
int dc_sweep_index1; // helper indexes for dc sweep
int dc_sweep_index2; // helper indexes for dc sweep
int dc_sweep; // dc sweep type, disabled = 0, voltage source = 1, current source = 2

double itol; // iterative threshold

extern struct twoTerminalsElement *head_twoTerminalsElement_list, *last_twoTerminalsElement_list;
extern struct threeTerminalsElement *head_threeTerminalsElement_list, *last_threeTerminalsElement_list;
extern struct fourTerminalsElement *head_fourTerminalsElement_list, *last_fourTerminalsElement_list;
extern struct hash_node * head_hash_node_list[HASH_TABLE_SIZE];


extern struct twoTerminalsElement** sources;

extern double *matrix_G;
extern double *matrix_C;
extern double *temp_matrix;
extern double *temp_matrix2;
extern double *vector_b;
extern int *transposition;
extern double *vector_y;
extern double *vector_x;

extern char *node_real_names;
extern int *node_dc_plot_vector; //Which nodes to plot on dc sweep
extern int *node_transient_plot_vector; //Which nodes to plot on transient analysis
extern char *sweep_name;

char file_name[256];

extern cs *G2;
extern cs *C2;
extern cs *temp_mat;
extern cs *temp_mat2;
extern css *S;
extern csn *N;

/*This function initializes the simulation */
void init(void);
/*This function maps the name string to a unique integer identifier.
Checks whether the same name already exists (making non case-sensitive comparison) otherwise creates
a new mapping.Returns the integer identifier.*/
int hash(char *string);
/* helper function, pretty much like hash */
int search_for_node(char *string);
/*This function parses the input file and creates 3 linked-lists (for two,three and four terminal elements
that hold the circuit elements.Also,checks whether the node 0 found in the input file */
void parseInput(FILE *p_file);
/* Creates the DC Matrix */
void createMnaSystemDC(void);
/* Creates the Transient Matrix */
void createMnaSystemTransient(void);
/*This function inserts a new two terminals element to the list*/
void insertTwoTerminalsElement(struct twoTerminalsElement *current);
/*This function inserts a new three terminals element to the list*/
void insertThreeTerminalsElement(struct threeTerminalsElement *current);
/*This function inserts a new four terminals element to the list*/
void insertFourTerminalsElement(struct fourTerminalsElement *current);
/* Clean up */
void freeAllocations(void);
/* LU Decomposition for linear systems */
void LUDecomposition(void);
/* line interchange for LU */
void interchange(int from, int to);
/* Cholesky Decomposition for linear systems (Symmetric Positive Definite Matrices) */
void CholeskyDecomposition(void);
/* Solves a linear system, Ax=b, after LU or Cholesky has taken place */
void solve_direct_methods(void);
/* forward substitution for linear systems */
void forward_substitution(void);
/* backward substitution for linear systems */
void backward_substitution(void);
/* Creates the inverse preconditioner for the iterative solving methods, M = 1 /diag(G) */
void createInversePreconditioner(double *inv_preconditioner, int length);
/* An Iterative method for solving the system Ax=b, (General Purpose Matrices) */
void BiConjugateGradient(void);
/* An Iterative method for solving the system Ax=b, (Symmetric Positive Definite Matrices) */
void ConjugateGradient(void);
/* x = inv(A) * b, when A is diagonal */
void solve_diagonal_system(double *vector1, double *vector2, double *result, int length);
/* Creates the values for every non dc source, for every time step */
void createTransientSteps(struct transientComponent*  trans);
/* Trapezoidal method for solving differential equations */
void Trapezoidal(double *vector_e, double *vector_e_1, double *temp_vec);
/* Backward Euler method for solving differential equations */
void BackwardEuler(double *vector_e, double *temp_vec);
/* DC Analysis of the circuit */
void DCAnalysis(void);
/* Transient Analysis of the circuit */
void TransientAnalysis(void);
/* DC Sweep of the circuit */
void DCSweep(void);


#endif