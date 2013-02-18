#include "circuit_simulation.h" 

double *matrix_G, *matrix_C, *vector_b, *vector_y, *vector_x, *temp_matrix, *temp_matrix2;
int *transposition;
char *node_real_names, *group_two_names;
int *node_dc_plot_vector, *node_transient_plot_vector;
char *sweep_name;
struct twoTerminalsElement** sources;

cs *G2, *C2, *temp_mat, *temp_mat2;
css *S;
csn *N;

int main(int argc, char* argv[]) {
  FILE *p_file = NULL;
  struct timeval calc_start, calc_end;
  
  
    if(argc != 2) {
        printf("Invalid argument count.\n");
	printf("./circuit_simulation <input>\n");
	printf("To see the netlist help type,\n");
	printf("./circuit_simulation -help\n");
	printf("Terminating.\n");
	exit(-1);
    }
    
    if(!strcmp(argv[1], "-help")) {
         printf("\nValid Netlist Declarations.\n\n");
	 
	 printf("Elements\n");
	 printf("________\n\n");
	 
	 printf("Voltage Source:\n");
	 printf("V<name> <(+) positive node name>  <(-) negative node name> <value in Volt> [transient part]\n\n");
	 printf("transient part:\n");
	 printf("EXP(i1 i2 td1 tc1 td2 tc2)\nSIN(i1 ia fr td df ph)\nPULSE(i1 i2 td tr tf pw per)\nPWL(t1 i1) (t2 i2)...(tn in)\n\n");
	 
	 printf("Current Source:\n");
	 printf("I<name> <(+) positive node name>  <(-) negative node name> <value in Ampere> [transient part]\n\n");
	 printf("transient part:\n");
	 printf("EXP(i1 i2 td1 tc1 td2 tc2)\nSIN(i1 ia fr td df ph)\nPULSE(i1 i2 td tr tf pw per)\nPWL(t1 i1) (t2 i2)...(tn in)\n\n");
	 
	 printf("Resistance:\n");
	 printf("R<name> <(+) positive node name>  <(-) negative node name> <value in Ohm>\n\n");
	 
	 printf("Capacitor:\n");
	 printf("C<name> <(+) positive node name>  <(-) negative node name> <value in Farad>\n\n");
	 
	 printf("Inductor:\n");
	 printf("L<name> <(+) positive node name>  <(-) negative node name> <value in Henry>\n\n");
	 
	 printf("Diode:\n");
         printf("D<name> <(+) positive node name>  <(-) negative node name> <model name> [area]\n");
	 printf("(Just for netlist parsing. The simulation works only with linear elements).\n\n");
	 
	 printf("Transistor MOS:\n");
	 printf("M<name> <D>  <G> <S> <B> <model name> l=<value> w=<value>\n");
	 printf("(Just for netlist parsing. The simulation works only with linear elements).\n\n");
	 
	 printf("Transistor BJT:\n");
	 printf("Q<name> <C>  <B> <E> <model name> [area]\n");
	 printf("(Just for netlist parsing. The simulation works only with linear elements).\n\n\n");
	 
	 printf("Linear Systems Solving Methods:\n");
	 printf("_______________________________\n\n");
	 printf("default LU.\n\n");
	 
	 printf("Cholesky:\n");
	 printf(".OPTIONS SPD\n\n");
	 
	 printf("Iterative:\n");
	 printf(".OPTIONS ITER\n\n");
	 
	 printf("Iterative for Symmetric - Positive Definite Matrices:\n");
	 printf(".OPTIONS ITER SPD\n\n");
	 
	 printf("Iterative converging threshold:\n");
	 printf("default 1e-3.\n\n");
	 
	 printf("Custom threshold:\n");
	 printf(".OPTIONS ITOL=<threshold>\n\n\n");
	 
	 printf("Matrix Sparsity:\n");
	 printf("_______________\n\n");
	 printf("default normal matrix.\n\n");
	 
	 printf("Sparse:\n");
	 printf(".OPTIONS SPARSE\n\n\n");
	 
	 printf("Differential Equations Solving Methods:\n");
	 printf("______________________________________\n\n");
	 printf("default Trapezoidal.\n\n");
	 
	 printf("Trapezoidal:\n");
	 printf(".OPTIONS METHOD=TR\n\n");
	 
	 printf("Backward Euler:\n");
	 printf(".OPTIONS METHOD=BE\n\n\n");
	 
	 printf("Graphs:\n");
	 printf("_______\n\n");
	 printf("Use gnuplot to visualize the results of DC Sweep, and Transient Analysis.\n");
	 printf("You also need to specify the plotted nodes, using the PLOT/PRINT options (See below).\n");
	 printf(".OPTIONS GNUPLOTLP, to plot using lines and points\n\n");
	 printf(".OPTIONS GNUPLOTL, to plot using lines.\n\n\n");
	 
	 printf("DC Analysis:\n");
	 printf("_________\n\n");
	 printf("To print the currents of each element at DC_Analysis.txt, use\n");
	 printf(".OP\n\n\n");
	 
	 printf("DC Sweep:\n");
	 printf("_________\n\n");
	 
	 printf(".DC <V or I><name> <start> <end> <step>\n\n");
	 
	 printf("To print the results of the sweep/transient on the file DC_sweep.txt/Transient_Analysis.txt\nyou need to write the following command,\n");
	 printf(".PLOT or .PRINT <mode> V(<node name>) V(<node name>) ... V(<node name>)\n");
	 printf("where <mode>, is either DC or TRAN (DC Sweep / Transient Analysis).\n");
	 printf("where V(<node name>), will print the sweep/transient result of the Voltage at node <node name>.\n");
	 printf("The PLOT/PRINT options will also have effect on the gnuplot graph.\n\n\n");
	 
	 printf("Transient Analysis:\n");
	 printf("___________________\n\n");
	 printf("To perform the transient analysis of the circuit, you need to type,\n");
	 printf(".TRAN <time step> <finish time>\n\n\n");
	 
	 printf("Formating:\n");
	 printf("__________\n\n");
	 printf("The application is case insensitive.\n");
	 printf("Empty lines, or line starting with * (comments) will be ignored.\n");
	 printf("Extra spaces or tabs will be ignored.\n");
	 printf("Expressions enclosed in square brackets, as shown in voltage sources, current sources, and transistors, can be left blank.\n\n");
	 
	 exit(0);
    }
    
    strcpy(file_name, argv[1]);
    
    p_file = fopen(file_name, "r");
    
    if(p_file == NULL) {
        printf("The file does not exist.\n");
	printf("Terminating.\n");
	exit(-1);
    }
    
    init();
    
    gettimeofday(&calc_start, NULL);
    
    parseInput(p_file);
     
    DCAnalysis();
    
    if(dc_sweep) {
        DCSweep();
    }
    
    if(transient_analysis) {
	TransientAnalysis();
    }

    gettimeofday(&calc_end, NULL);
    

    printf("\n\nDone in, %ld ms.\n\n", (calc_end.tv_sec * 1000 + calc_end.tv_usec / 1000) - (calc_start.tv_sec * 1000 + calc_start.tv_usec / 1000));
    
    fclose(p_file);
    
    freeAllocations();
    
      
    return 0;
    
}


void init(void) {
  int i;
  
    matrix_G = NULL;
    matrix_C = NULL;
    vector_b = NULL;
    transposition = NULL;
    vector_y = NULL;
    vector_x = NULL;
    
    temp_matrix = NULL;
    temp_matrix2 = NULL;
    
    G2 = NULL;
    C2 = NULL;
    temp_mat = NULL;
    temp_mat2 = NULL;
    S = NULL;
    N = NULL;
    
    sources = NULL;
    
    node_real_names = NULL;
    sweep_name = NULL;
    node_dc_plot_vector = NULL;
    node_transient_plot_vector = NULL;
    
    node_real_names = (char *) malloc(256 * sizeof(char));
    
    if(node_real_names == NULL) {
        printf("Could not allocate matrices.\n");
        printf("Terminating.\n");
        exit(-1);
    }
    
    sweep_name = (char *) malloc(256 * sizeof(char));
    
    if(sweep_name == NULL) {
        printf("Could not allocate memory.\n");
        printf("Terminating.\n");
        exit(-1);
    }
    
    node_dc_plot_vector = (int *) malloc(sizeof(int));
    
    if(node_dc_plot_vector == NULL) {
        printf("Could not allocate matrices.\n");
        printf("Terminating.\n");
        exit(-1);
    }
    
    node_dc_plot_vector[0] = 0;
    
    node_transient_plot_vector = (int *) malloc(sizeof(int));
    
    if(node_transient_plot_vector == NULL) {
        printf("Could not allocate matrices.\n");
        printf("Terminating.\n");
        exit(-1);
    }
    
    node_transient_plot_vector[0] = 0;
    
    strcpy(node_real_names, "0 (Ground)");
    
    number_of_nodes = 0;
    group1_elements = 0;
    group2_elements = 0;
    
    dc_sweep = 0;
    dc_sweep_start = 0;
    dc_sweep_end = 0;
    dc_sweep_step = 0;
    dc_sweep_index1 = -1;
    dc_sweep_index2 = -1;
    
    for(i = 0; i < ELEMENTS; i++) {
        number_of_elements[i] = 0;
    }
    
    for(i = 0; i < HASH_TABLE_SIZE; i++) {
        head_hash_node_list[i] = NULL;
    }
    
    non_zeroes_G = 0;
    non_zeroes_C = 0;
    
    matrix_sparsity = NORMAL;
    method = DIRECT;
    matrix_type = NONSPD;
    
    transient_analysis = 0;
    
    diff_method = TRAPEZOIDAL;
    
    itol = 1e-3;
    
    operating_point = 0;
    
    gnuplot = 0;
  
}


void freeAllocations(void) {
  struct twoTerminalsElement *current1 = NULL, *last1 = NULL; 
  struct threeTerminalsElement *current2 = NULL, *last2 = NULL; 
  struct fourTerminalsElement *current3 = NULL, *last3 = NULL;
  struct hash_node *current4 = NULL, *last4 = NULL; 
  int i;
  
  
    for(last1 = current1 = head_twoTerminalsElement_list; head_twoTerminalsElement_list != NULL && (current1 -> next != NULL);) {
        current1 = current1 -> next;
	if(last1 -> transient != NULL) {
	    switch (last1 -> transient -> impulse) {
		case EXP:
		    free(last1 -> transient -> exp);
		    break;
		case SIN:
		    free(last1 -> transient -> sin);
		    break;
		case PULSE:
		    free(last1 -> transient -> pulse);
		    break;
		case PWL:
		    free(last1 -> transient -> pwl -> t);
		    free(last1 -> transient -> pwl -> i);
		    free(last1 -> transient -> pwl);
		    break;
	    }
	    free(last1 -> transient -> vals);
	    free(last1 -> transient);
	}
        free(last1);
        last1 = current1;
    }
    
    if(last1 -> transient != NULL) {
	switch (last1 -> transient -> impulse) {
	    case EXP:
		free(last1 -> transient -> exp);
		break;
	    case SIN:
		free(last1 -> transient -> sin);
		break;
	    case PULSE:
		free(last1 -> transient -> pulse);
		break;
	    case PWL:
		free(last1 -> transient -> pwl -> t);
		free(last1 -> transient -> pwl -> i);
		free(last1 -> transient -> pwl);
		break;
	}
	free(last1 -> transient -> vals);
	free(last1 -> transient);
    }
    free(last1);
    
    for(last2 = current2 = head_threeTerminalsElement_list; head_threeTerminalsElement_list != NULL && (current2 -> next != NULL);) {
        current2 = current2 -> next;
        free(last2);
        last2 = current2;
    }
    free(last2);
    
    for(last3 = current3 = head_fourTerminalsElement_list; head_fourTerminalsElement_list != NULL && (current3 -> next != NULL);) {
        current3 = current3 -> next;
        free(last3);
        last3 = current3;
    }
    free(last3);
    
    for(i = 0; i < HASH_TABLE_SIZE; i++) {
        for(last4 = current4 = head_hash_node_list[i]; head_hash_node_list[i] != NULL && (current4 -> next != NULL);) {
            current4 = current4 -> next;
            free(last4);
            last4 = current4;
        }
        free(last4);
    }
    
 
    
    if(transient_analysis) {
	free(sources);
	
	if(matrix_sparsity == SPARSE) {
	    cs_spfree(C2);
	}
	else {
	    free(matrix_C);
	}
    }
    
    if(matrix_sparsity == SPARSE) {
	if(method == DIRECT) {
	    cs_sfree(S);
	    cs_nfree(N);
	}
	else {
	    cs_spfree(G2);
	}
    }
    else {
        free(matrix_G);
	
        if(method == DIRECT && matrix_type == NONSPD) {
	    free(transposition);   
	}
    }
    
    free(vector_x);
    
    free(vector_b);
    
    free(node_real_names);
    
    free(sweep_name);
    
    free(node_dc_plot_vector);
    free(node_transient_plot_vector);
    
}


void DCAnalysis(void) {
  FILE *p_file = NULL;
  int i, dimension;
  struct twoTerminalsElement *current1 = NULL;
  char temp[256];
  
    printf("\n\nDC Analysis...\n\n");
    
    dimension = number_of_nodes + group2_elements;
    
    vector_b = (double *) calloc(dimension, sizeof(double));
    vector_x = (double *) calloc(dimension, sizeof(double));
    
    if(vector_b == NULL || vector_x == NULL) {
	printf("Could not allocate matrices.\n");
	printf("Terminating.\n");
	exit(-1);
    }
    
    createMnaSystemDC();
  
    if(method == DIRECT) {
	if(matrix_type == NONSPD) {
	    LUDecomposition(); 
	}
	else {
	    CholeskyDecomposition();
	}
	
	solve_direct_methods();
    }
    else {
        if(matrix_type == NONSPD) {
	    BiConjugateGradient();
	}
	else {
	    ConjugateGradient();
	}
    }
    
    printf("\n\nPrinting results to DCAnalysis.txt ...\n\n");
    
    p_file = fopen("DC_analysis.txt", "w");
	    
    
    fprintf(p_file, "The circuit, %s, contains:\n\n", file_name);

    fprintf(p_file, "          Nodes: %7d\n", number_of_nodes + 1);
    
    if(number_of_elements[VOLTAGE_SOURCE]) {
	fprintf(p_file, "Voltage Sources: %7d\n", number_of_elements[VOLTAGE_SOURCE]);
    }
    
    if(number_of_elements[CURRENT_SOURCE]) {
	fprintf(p_file, "Current Sources: %7d\n", number_of_elements[CURRENT_SOURCE]);
    }
    
    if(number_of_elements[RESISTANCE]) {
	fprintf(p_file, "    Resistances: %7d\n", number_of_elements[RESISTANCE]);
    }
    
    if(number_of_elements[CAPACITOR]) {
	fprintf(p_file, "     Capacitors: %7d\n", number_of_elements[CAPACITOR]);
    }
    
    if(number_of_elements[INDUCTOR]) {
	fprintf(p_file, "      Inductors: %7d\n", number_of_elements[INDUCTOR]);
    }
    
    if(number_of_elements[DIODE]) {
	fprintf(p_file, "         Diodes: %7d\n", number_of_elements[DIODE]);
    }
    
    if(number_of_elements[BJT]) {
	fprintf(p_file, "BJT Transistors: %7d\n", number_of_elements[BJT]);
    }
    
    if(number_of_elements[MOS]) {
	fprintf(p_file, "MOS Transistors: %7d\n", number_of_elements[MOS]);
    }
    
    fprintf (p_file, "\n\n\nDC Analysis\n");
    fprintf (p_file, "~~~~~~~~~~~");
	    
   
    fprintf (p_file, "\n_____________________________________________________________________________\n");

    fprintf (p_file, "|   Voltage @ node| %25s: %22.14e   Volt|", node_real_names, 0.0); 
    for(i = 0; i < number_of_nodes; i++) {
	fprintf (p_file, "\n|   Voltage @ node| %25s: %22.14e   Volt|", node_real_names + (i + 1) * 256, fabs(vector_x[i]) < ZERO ? 0.0 : vector_x[i]); 
    }
	    
    fprintf (p_file, "\n_____________________________________________________________________________");
	  
	 
    if(operating_point) {
	for(current1 = head_twoTerminalsElement_list, i = 0; head_twoTerminalsElement_list != NULL && current1 != NULL; current1 = current1 -> next) {
	    if(current1 -> type == CAPACITOR) {
		temp[0] = '\0';
		strcat(temp, "C ");
		strcat(temp, current1 -> string_name);
		fprintf (p_file, "\n|Current @ element| %25s: %22.14e Ampere|", temp, 0.0);
	    }
	    else if(current1 -> type == RESISTANCE) {
		temp[0] = '\0';
		strcat(temp, "R ");
		strcat(temp, current1 -> string_name);
		fprintf (p_file, "\n|Current @ element| %25s: %22.14e Ampere|", temp, ((current1 -> positive_terminal == 0 ? 0 : (fabs(vector_x[current1 -> positive_terminal - 1]) < ZERO ? 0.0 : vector_x[current1 -> positive_terminal - 1])) - (current1 -> negative_terminal == 0 ? 0 : (fabs(vector_x[current1 -> negative_terminal - 1]) < ZERO ? 0.0 : vector_x[current1 -> negative_terminal - 1]))) / current1 -> value);
	    }
	    else if(current1 -> type == CURRENT_SOURCE) {
		temp[0] = '\0';
		strcat(temp, "I ");
		strcat(temp, current1 -> string_name);
		fprintf (p_file, "\n|Current @ element| %25s: %22.14e Ampere|", temp, current1 -> value);
	    }
	    else if(current1 -> type == VOLTAGE_SOURCE) {
		temp[0] = '\0';
		strcat(temp, "V ");
		strcat(temp, current1 -> string_name);
		fprintf (p_file, "\n|Current @ element| %25s: %22.14e Ampere|", temp, fabs(vector_x[i + number_of_nodes]) < ZERO ? 0.0 : vector_x[i + number_of_nodes]);
		i++;
	    }
	    else if(current1 -> type == INDUCTOR) {
		temp[0] = '\0';
		strcat(temp, "L ");
		strcat(temp, current1 -> string_name);
		fprintf (p_file, "\n|Current @ element| %25s: %22.14e Ampere|", temp, fabs(vector_x[i + number_of_nodes]) < ZERO ? 0.0 : vector_x[i + number_of_nodes]);
		i++;
	    }
	}
	
	fprintf (p_file, "\n_____________________________________________________________________________\n\n");
    }
    
	    
    fclose(p_file);
    
     
}


void TransientAnalysis(void) {
  FILE *p_file = NULL, *p_file2 = NULL, *pipe = NULL;
  int dimension, k, i, j, l, temp3, length, counter;
  double *vector_e_1 = NULL, *temp_vec = NULL, *vector_e = NULL, t, time_steps;
  char string[3000];
  char temp_string[300];
  
    dimension = number_of_nodes + group2_elements;
    
    temp_vec = (double *) malloc(dimension * sizeof(double));
    vector_e = (double *) malloc(dimension * sizeof(double));
    
    
    if(temp_vec == NULL || vector_e == NULL) {
        printf("Could not allocate matrices.\n");
	printf("Terminating.\n");
	exit(-1);
    }
    
    printf("\n\nTransient Analysis...\n\n");

    createMnaSystemTransient();
    
    
    if(matrix_sparsity == SPARSE) {
        cs_spfree(G2);
	G2 = temp_mat; // G = G + C, trapezoidal => G = G + 2/h * C, backward euler => G = G + 1/h * C
    }
    else {
        free(matrix_G);
	matrix_G = temp_matrix; // G = G + C, trapezoidal => G = G + 2/h * C, backward euler => G = G + 1/h * C
    }
     
    if(diff_method == TRAPEZOIDAL) {
	vector_e_1 = (double *) malloc(dimension * sizeof(double));
	
	if(vector_e_1 == NULL) {
	    printf("Could not allocate matrices.\n");
	    printf("Terminating.\n");
	    exit(-1);
	}
	 
	memcpy(vector_e_1, vector_b, dimension * sizeof(double)); // e_1 = b

	if(matrix_sparsity == SPARSE) {
	    cs_spfree(C2);
	    C2 = temp_mat2; // C = G - C <=> G = G - 2/h * C
        }
        else {
	    free(matrix_C);
	    matrix_C = temp_matrix2; // C = G - C <=> G = G - 2/h * C
	}
	
    }
    

    if(method == DIRECT) {
	if(matrix_type == NONSPD) {
	    LUDecomposition(); // Decompose G
	}
	else {
	    CholeskyDecomposition(); // Decompose G
	}
    }
    
    
    printf("\n\nPrinting results to Transient_Analysis.txt ...\n\n");
  
    p_file = fopen("Transient_Analysis.txt", "w");
    
    fprintf(p_file, "The circuit, %s, contains:\n\n", file_name);

    fprintf(p_file, "          Nodes: %7d\n", number_of_nodes + 1);
    
    if(number_of_elements[VOLTAGE_SOURCE]) {
	fprintf(p_file, "Voltage Sources: %7d\n", number_of_elements[VOLTAGE_SOURCE]);
    }
    
    if(number_of_elements[CURRENT_SOURCE]) {
	fprintf(p_file, "Current Sources: %7d\n", number_of_elements[CURRENT_SOURCE]);
    }
    
    if(number_of_elements[RESISTANCE]) {
	fprintf(p_file, "    Resistances: %7d\n", number_of_elements[RESISTANCE]);
    }
    
    if(number_of_elements[CAPACITOR]) {
	fprintf(p_file, "     Capacitors: %7d\n", number_of_elements[CAPACITOR]);
    }
    
    if(number_of_elements[INDUCTOR]) {
	fprintf(p_file, "      Inductors: %7d\n", number_of_elements[INDUCTOR]);
    }
    
    if(number_of_elements[DIODE]) {
	fprintf(p_file, "         Diodes: %7d\n", number_of_elements[DIODE]);
    }
    
    if(number_of_elements[BJT]) {
	fprintf(p_file, "BJT Transistors: %7d\n", number_of_elements[BJT]);
    }
    
    if(number_of_elements[MOS]) {
	fprintf(p_file, "MOS Transistors: %7d\n", number_of_elements[MOS]);
    }
    
    fprintf (p_file, "\n\n\nTransient Analysis    start: %12lf,  stop: %12lf,  step: %12lf\n", 0.0, fin_time, time_step);
    fprintf (p_file, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n");
    
    #if DEBUG
    printf("\n\nTRANSIENT ANALYSIS    start: %12lf,  stop: %12lf,  step: %12lf\n", 0.0, fin_time, time_step);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n");
    #endif
    
    
    fprintf (p_file, "\nt = %12lf", 0.0);
    fprintf (p_file,"\n~~~~~~~~~~~~~~~~");
    fprintf (p_file, "\n_____________________________________________________________________________");
    for(l = 0; l < number_of_nodes + 1; l++) {
	if(node_transient_plot_vector[l] == 1) {
	    if(l == 0) {
		 fprintf (p_file, "\n|   Voltage @ node| %25s: %22.14e   Volt|", node_real_names, 0.0); 
	    }
	    else {
		 fprintf (p_file, "\n|   Voltage @ node| %25s: %22.14e   Volt|", node_real_names + l * 256, fabs(vector_x[l - 1]) < ZERO ? 0.0 : vector_x[l - 1]); 
	    }
	}
    }
    fprintf (p_file, "\n_____________________________________________________________________________\n\n");
    
    if(gnuplot) {
        p_file2 = fopen("transient_plot.txt", "w");
	fprintf(p_file2, "%.14lf ", 0.0); //t
	fprintf(p_file2, "%.14lf ", 0.0); // ground
	for(l = 0; l < number_of_nodes; l++) {
	    fprintf(p_file2, "%.14lf ", vector_x[l]);
	}
	fprintf(p_file2, "\n");
    }
    
    time_steps = fin_time / time_step;
    length = (int)time_steps + 1;
    
    for(i = 1, t = time_step; i < length; i++, t += time_step) {
      
        memset(vector_e, 0, dimension * sizeof(double));
	
	for(j = 0, k = 0; j < number_of_elements[CURRENT_SOURCE] + group2_elements; j++) {
	   
	    switch (sources[j] -> type) {
		case CURRENT_SOURCE:
		    if(sources[j]  -> positive_terminal) {
			vector_e[sources[j]  -> positive_terminal - 1] -= sources[j] -> transient == NULL ? sources[j] -> value : sources[j] -> transient -> vals[i];
		    }
		    
		    if(sources[j]  -> negative_terminal) {
			vector_e[sources[j]  -> negative_terminal - 1] += sources[j] -> transient == NULL ? sources[j] -> value : sources[j] -> transient -> vals[i];
		    }
		    break;
		    
		case VOLTAGE_SOURCE:
		case INDUCTOR:
		    temp3 = number_of_nodes + k;

		    vector_e[temp3] += sources[j]  -> type == INDUCTOR ? 0 : (sources[j] -> transient == NULL ? sources[j] -> value : sources[j] -> transient -> vals[i]);

		    k++;
		    break;
	      
	    }
	  
	}
	
	if(diff_method == TRAPEZOIDAL) {
	    Trapezoidal(vector_e, vector_e_1, temp_vec);
        }
        else {
	    BackwardEuler(vector_e, temp_vec);
        }
        
        #if DEBUG
        printf("\nt = %12lf", t);
	printf("\n~~~~~~~~~~~~~~~~\n");
        #endif
        
        if(method == DIRECT) {
	    solve_direct_methods();
	}
	else {
	    if(matrix_type == NONSPD) {
		BiConjugateGradient();
	    }
	    else {
		ConjugateGradient();
	    }
	}
	
        
	fprintf (p_file, "\nt = %12lf", t);
	fprintf (p_file,"\n~~~~~~~~~~~~~~~~");
	
	fprintf (p_file, "\n_____________________________________________________________________________");
	for(l = 0; l < number_of_nodes + 1; l++) {
	    if(node_transient_plot_vector[l] == 1) {
		if(l == 0) {
		    fprintf (p_file, "\n|   Voltage @ node| %25s: %22.14e   Volt|", node_real_names, 0.0); 
		}
		else {
		    fprintf (p_file, "\n|   Voltage @ node| %25s: %22.14e   Volt|", node_real_names + l * 256, fabs(vector_x[l - 1]) < ZERO ? 0.0 : vector_x[l - 1]); 
		}
	    }
	}
	fprintf (p_file, "\n_____________________________________________________________________________\n\n");
	
	if(gnuplot) {
	    fprintf(p_file2, "%.14lf ", t);
	    fprintf(p_file2, "%.14lf ", 0.0);
	    for(l = 0; l < number_of_nodes; l++) { //dimension
		fprintf(p_file2, "%.14lf ", vector_x[l]);
	    }
	    fprintf(p_file2, "\n");
	}
    }
    
    
    if(gnuplot) {
	fclose(p_file2);
	
	for(l = 0, counter = 0; l < number_of_nodes + 1; l++) {
	    if(node_transient_plot_vector[l] == 1) {
		counter++;
	    }
	}
	
	if(counter != 0) {
	    pipe = popen("gnuplot -persist", "w");
	    
	    string[0] = '\0';
	    strcat(string, "plot ");
	    for(l = 0, j = 0; j < counter && l < number_of_nodes + 1; l++) {
		if(node_transient_plot_vector[l] == 1) {
		    j++;
		    if(j  == counter) {
		        if(gnuplot == LINES_POINTS) {
			    sprintf(temp_string, "\"./transient_plot.txt\" using 1:%d title 'V (%s)' with linespoints", l + 2, node_real_names + l * 256);
			}
			else {
			    sprintf(temp_string, "\"./transient_plot.txt\" using 1:%d title 'V (%s)' with lines", l + 2, node_real_names + l * 256);
			}
		        strcat(string, temp_string);
		    }
		    else {
		        if(gnuplot == LINES_POINTS) {
			    sprintf(temp_string, "\"./transient_plot.txt\" using 1:%d title 'V (%s)' with linespoints, ", l + 2, node_real_names + l * 256);
			}
			else {
			    sprintf(temp_string, "\"./transient_plot.txt\" using 1:%d title 'V (%s)' with lines, ", l + 2, node_real_names + l * 256);
			}
		        strcat(string, temp_string);
		    }
		}
	    }
	
	    sprintf(temp_string, "; set grid; set zeroaxis; set key out horiz top center; set terminal wxt size 800,600; set title \"Transient Analysis\t#circuit %s\"; set autoscale; set xlabel \"Time [sec]\"; set ylabel \"V values\";  replot;", file_name);
	    strcat(string, temp_string);
	    fprintf(pipe, "%s\n", string);
	    fclose(pipe);
	}
    }
    
  
    if(diff_method == TRAPEZOIDAL) {
	free(vector_e_1);
    }
    free(temp_vec);
    free(vector_e);
    
    fclose(p_file);
    
}


void DCSweep(void) {
  double k, *old_vec_x = NULL, *old_vec_b = NULL;
  int dimension, j, l, counter;
  FILE *p_file = NULL, *p_file2 = NULL, *pipe = NULL;
  char string[3000];
  char temp_string[300];
  
  
    dimension = number_of_nodes + group2_elements;
    
    old_vec_x = (double *) malloc(dimension * sizeof(double));
    old_vec_b = (double *) malloc(dimension * sizeof(double));
    
    if(old_vec_x == NULL || old_vec_b == NULL) {
        printf("Could not allocate matrices.\n");
	printf("Terminating.\n");
	exit(-1);
    }
  
    memcpy(old_vec_x, vector_x, dimension * sizeof(double));
    memcpy(old_vec_b, vector_b, dimension * sizeof(double));
  
    printf("\n\nDc Sweep...\n\n");
    
    printf("\n\nPrinting results to DCSweep.txt ...\n\n");
  
    p_file = fopen("DC_sweep.txt", "w");
    
    
    memset(vector_x, 0, dimension * sizeof(double));
    
    dc_sweep_index1 += dc_sweep == SWEEP_VOLTAGE_SOURCE ? number_of_nodes : 0;
    
    fprintf(p_file, "The circuit, %s, contains:\n\n", file_name);

    fprintf(p_file, "          Nodes: %7d\n", number_of_nodes + 1);
    
    if(number_of_elements[VOLTAGE_SOURCE]) {
	fprintf(p_file, "Voltage Sources: %7d\n", number_of_elements[VOLTAGE_SOURCE]);
    }
    
    if(number_of_elements[CURRENT_SOURCE]) {
	fprintf(p_file, "Current Sources: %7d\n", number_of_elements[CURRENT_SOURCE]);
    }
    
    if(number_of_elements[RESISTANCE]) {
	fprintf(p_file, "    Resistances: %7d\n", number_of_elements[RESISTANCE]);
    }
    
    if(number_of_elements[CAPACITOR]) {
	fprintf(p_file, "     Capacitors: %7d\n", number_of_elements[CAPACITOR]);
    }
    
    if(number_of_elements[INDUCTOR]) {
	fprintf(p_file, "      Inductors: %7d\n", number_of_elements[INDUCTOR]);
    }
    
    if(number_of_elements[DIODE]) {
	fprintf(p_file, "         Diodes: %7d\n", number_of_elements[DIODE]);
    }
    
    if(number_of_elements[BJT]) {
	fprintf(p_file, "BJT Transistors: %7d\n", number_of_elements[BJT]);
    }
    
    if(number_of_elements[MOS]) {
	fprintf(p_file, "MOS Transistors: %7d\n", number_of_elements[MOS]);
    }
      
    fprintf (p_file, "\n\n\nDC Sweep @ Source %16s    start: %12lf,  stop: %12lf,  step: %12lf\n", sweep_name, dc_sweep_start, dc_sweep_end, dc_sweep_step);
    fprintf (p_file, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n\n");
    
    #if DEBUG
    printf("\n\nDC SWEEP @ Source %16s    start: %12lf,  stop: %12lf,  step: %12lf\n", sweep_name, dc_sweep_start, dc_sweep_end, dc_sweep_step);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n");
    #endif
    
    if(gnuplot) {
        p_file2 = fopen("dc_sweep_plot.txt", "w");
    }
       
    for(k = dc_sweep_start; dc_sweep_start < dc_sweep_end ? k - dc_sweep_end <= ZERO: k - dc_sweep_end >= -ZERO; k += dc_sweep_step) {

        
        if(dc_sweep == SWEEP_VOLTAGE_SOURCE) {
	    vector_b[dc_sweep_index1] = k;
	}
	else {
	    if(dc_sweep_index1 != -1) {
	        vector_b[dc_sweep_index1] = -k;
	    }
	    
	    if(dc_sweep_index2 != -1) {
	        vector_b[dc_sweep_index2] = k;
	    }
	}
	
        #if DEBUG
        printf("current sweep: %12lf\n", k);
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        printf("\nVector b:\n\n");
        for(j = 0; j < dimension; j++) {
            printf("%12lf\n", vector_b[j]);
        }
    
        printf("\n");
        #endif
	
	if(method == DIRECT) {
	    solve_direct_methods();
	}
	else {
	    if(matrix_type == NONSPD) {
	        BiConjugateGradient();
	    }
	    else {
	        ConjugateGradient();
	    }
	}
	
	fprintf (p_file, "current sweep: %12lf\n", k);
	fprintf (p_file, "~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    
	fprintf (p_file, "\n_____________________________________________________________________________");
        for(l = 0; l < number_of_nodes + 1; l++) {
            if(node_dc_plot_vector[l] == 1) {
		if(l == 0) {
		    fprintf (p_file, "\n|   Voltage @ node| %25s: %22.14e   Volt|", node_real_names, 0.0); 
		}
		else {
		    fprintf (p_file, "\n|   Voltage @ node| %25s: %22.14e   Volt|", node_real_names + l * 256, fabs(vector_x[l - 1]) < ZERO ? 0.0 : vector_x[l - 1]); 
		}
	    }
        }
        fprintf (p_file, "\n_____________________________________________________________________________");
    
        
        fprintf (p_file, "\n\n\n\n");
	
	if(gnuplot) {
	    fprintf(p_file2, "%.14lf ", k);
	    fprintf(p_file2, "%.14lf ", 0.0);
	    for(l = 0; l < number_of_nodes; l++) { // dimension
		fprintf(p_file2, "%.14lf ", vector_x[l]);
	    }
	    fprintf(p_file2, "\n");
	}
        
    }
        
    
    if(gnuplot) {
	fclose(p_file2);
	
	for(l = 0, counter = 0; l < number_of_nodes + 1; l++) {
	    if(node_dc_plot_vector[l] == 1) {
		counter++;
	    }
	}
	
	if(counter != 0) {
	    pipe = popen("gnuplot -persist", "w");
	    
	    string[0] = '\0';
	    strcat(string, "plot ");
	    for(l = 0, j = 0; j < counter && l < number_of_nodes + 1; l++) {
		if(node_dc_plot_vector[l] == 1) {
		    j++;
		     if(j  == counter) {
		        if(gnuplot == LINES_POINTS) {
			    sprintf(temp_string, "\"./dc_sweep_plot.txt\" using 1:%d title 'V (%s)' with linespoints", l + 2, node_real_names + l * 256);
			}
			else {
			    sprintf(temp_string, "\"./dc_sweep_plot.txt\" using 1:%d title 'V (%s)' with lines", l + 2, node_real_names + l * 256);
			}
		        strcat(string, temp_string);
		    }
		    else {
		        if(gnuplot == LINES_POINTS) {
			    sprintf(temp_string, "\"./dc_sweep_plot.txt\" using 1:%d title 'V (%s)' with linespoints, ", l + 2, node_real_names + l * 256);
			}
			else {
			    sprintf(temp_string, "\"./dc_sweep_plot.txt\" using 1:%d title 'V (%s)' with lines, ", l + 2, node_real_names + l * 256);
			}
		        strcat(string, temp_string);
		    }
		}
	    }
	
	    sprintf(temp_string, "; set grid; set zeroaxis; set key out horiz top center; set terminal wxt size 800,600; set title \"DC Sweep @ Source %s\t#circuit %s\"; set autoscale; set xlabel \"Sweep Values\"; set ylabel \"V values\";  replot;", sweep_name, file_name);
	    strcat(string, temp_string);
	    fprintf(pipe, "%s\n", string);
	    fclose(pipe);
	}
    }
    
    
    memcpy(vector_x, old_vec_x, dimension * sizeof(double));
    memcpy(vector_b, old_vec_b, dimension * sizeof(double));
    
    free(old_vec_x);
    free(old_vec_b);
    
    fclose(p_file);

}
  




