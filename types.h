#ifndef _TYPES_
  
#define _TYPES_

#define NAME_SIZE 256

#define ELEMENTS 11

#define SIZE_OF_LINE 3000

#define ZERO 1e-12

#define DEBUG 0


typedef enum {VOLTAGE_SOURCE, CURRENT_SOURCE, RESISTANCE, CAPACITOR, INDUCTOR, DIODE, BJT, MOS, VOLTAGE_CONTROLLED_VOLTAGE_SOURCE, VOLTAGE_CONTROLLED_CURRENT_SOURCE, SHORT_CIRCUIT} ELEMENT_TYPE;

typedef enum {SWEEP_OFF, SWEEP_VOLTAGE_SOURCE, SWEEP_CURRENT_SOURCE} SWEEP_OPT;

typedef enum {NORMAL, SPARSE} SPARSITY;

typedef enum {NONSPD, SPD} TYPE;

typedef enum {DIRECT,ITERATIVE} LINEAR_SOLVING_METHOD;

typedef enum {BACKWARD_EULER,TRAPEZOIDAL} DIFFERENTIAL_EQUATION_SOLVING_METHOD;

typedef enum {EXP, SIN, PULSE, PWL, SFFM} IMPULSE;

typedef enum {GNUPLOT_OFF, LINES_POINTS, LINES} GNUPLOT_OPT;

typedef enum {DOT_GRAPH_OFF, DOT_GRAPH_ON} DOT_GRAPH_OPT;

typedef enum {DISABLED, ENABLED} OPTION;

typedef enum {LIN, LOG} SCALE;

typedef enum {LINEAR, DB} AC_PLOT_SCALE;


typedef struct { // spice format exp inputs
    double i1;
    double i2;
    double td1;
    double tc1;
    double td2;
    double tc2;
}  ExpImpulse;


typedef struct { // spice format sin inputs
    double i1;
    double ia;
    double fr;
    double td;
    double df;
    double ph;
} SinImpulse;

typedef struct { // spice format pulse inputs
    double i1;
    double i2;
    double td;
    double tr;
    double tf;
    double pw;
    double per;
} PulseImpulse;

typedef struct { // spice format piece wise linear inputs
   int length;
   double *t;
   double *i;
} PWLImpulse;

typedef struct { // spice format single frequency fm
    double v0;
    double va;
    double fc;
    double mdi;
    double fs;
} SFFMImpulse;

typedef struct {
    IMPULSE impulse; // type of impulse, exp, sin, pulse, or piece wise linear
    ExpImpulse *exp;
    SinImpulse *sin;
    PulseImpulse *pulse;
    PWLImpulse *pwl;
    SFFMImpulse *sffm;
    double *vals; // precomputed values for the transient analysis
} transientComponent;

typedef struct {
    double mag;
    double phase;
} acComponent;

typedef struct twoTerminalsElement {
    ELEMENT_TYPE type; // V, I, R, C, L, (Diode just parsed, not used)
    unsigned int name; // Element name
    char string_name[NAME_SIZE];  // Original given name
    unsigned int positive_terminal; // positive terminal
    unsigned int negative_terminal; // negative terminal
    
    unsigned int in_positive_terminal;
    unsigned int in_negative_terminal;
    double value; // Element value, according to type, SI
    transientComponent *transient; // transient part, for V, I, null if DC, or not V, I
    acComponent *ac; // ac part, for V, I, null if DC, or not V, I
    int isG2;
    struct twoTerminalsElement *next;
} twoTerminalsElement;

typedef struct threeTerminalsElement { // Just for parsing, not used
    ELEMENT_TYPE type;
    unsigned int name;
    char string_name[NAME_SIZE];
    unsigned int c_terminal;
    unsigned int b_terminal;
    unsigned int e_terminal;
    double value;
    struct threeTerminalsElement *next;
} threeTerminalsElement;

typedef struct fourTerminalsElement { // Just for parsing, not used
    ELEMENT_TYPE type;
    unsigned int name;
    char string_name[NAME_SIZE];
    unsigned int d_terminal;
    unsigned int g_terminal;
    unsigned int s_terminal;
    unsigned int b_terminal;
    double length;
    double width;
    struct fourTerminalsElement *next;
} fourTerminalsElement;


typedef struct {
    char name[NAME_SIZE];
    OPTION dc_sweep_plot; //Which nodes to plot on dc sweep
    OPTION transient_plot;  //Which nodes to plot on transient analysis
    OPTION ac_plot; //Which nodes to plot on ac analysis
} plotSettings;


typedef struct {
    SWEEP_OPT dc_sweep; // dc sweep type, disabled = SWEEP_OFF, voltage source = SWEEP_VOLTAGE_SOURCE, current source = SWEEP_CURRENT_SOURCE
    char sweep_name[NAME_SIZE];
    double dc_sweep_start; // dc sweep start value
    double dc_sweep_end ; // dc sweep end value
    double dc_sweep_step; // dc sweep step value
    int dc_sweep_index1; // helper indexes for dc sweep
    int dc_sweep_index2; // helper indexes for dc sweep
} dcSweep;

typedef struct {
    double time_step; // time step for the transient analysis
    double fin_time; // end time for the transinet analysis (start time = 0)
} transientAnalysis;

typedef struct {
    SCALE scale;
    unsigned int points;
    double start_freq; // starting frequency for ac analysis
    double end_freq; // ending frequency for ac analysis
} acAnalysis;

typedef struct {
    OPTION operating_point; // print currents at DC_Analysis.txt
} dcAnalysis;

typedef struct {
    double itol; // iterative threshold
    double EPS;
    unsigned int division_by_zero;
} iterativeMethods;

typedef struct {
    char file_name[NAME_SIZE];
    SPARSITY matrix_sparsity; // Sparse or Normal Matrices
    TYPE matrix_type; // General Purpose, or Symmetric Positive Definite Matrices
    LINEAR_SOLVING_METHOD method; // Direct or Iterative Methods for solving Ax=b
    DIFFERENTIAL_EQUATION_SOLVING_METHOD diff_method; // differential equation solving method, Trapezoidal, Backward Euler
    iterativeMethods iterative_methods;
    
    GNUPLOT_OPT gnuplot; // plot the results using gnuplot (needs to be installed)
    DOT_GRAPH_OPT dot_graph; // display the graph of the circuit using dot (needs to be installed)
    
    dcAnalysis dc_analysis_settings;
    
    dcSweep *dc_sweep_settings;
    unsigned int number_of_sweeps;
    
    transientAnalysis *transient_analysis_settings;
    unsigned int number_of_transient_analysis; 
    
    acAnalysis *ac_analysis_settings;
    unsigned int number_of_ac_analysis;
    AC_PLOT_SCALE ac_plot_scale;
    
    plotSettings *plot_settings;
    unsigned int number_of_nodes; // number of nodes, not counting ground
    
    unsigned int number_of_elements[ELEMENTS]; // helper array for counting each element type

    unsigned int group1_elements; // Group 1: R, C, I  
    unsigned int group2_elements; // Group 2: V, L
} circuitSimulation;

extern circuitSimulation circuit_simulation;
#endif
