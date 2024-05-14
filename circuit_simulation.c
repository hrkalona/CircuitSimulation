#include "circuit_simulation.h" 

double *matrix_G, *matrix_C, *vector_b, *vector_y, *vector_x, *temp_matrix, *temp_matrix2;
double *inv_preconditioner;
int *transposition;
twoTerminalsElement** sources;
circuitSimulation circuit_simulation;

twoTerminalsElement *head_twoTerminalsElement_list = NULL, *last_twoTerminalsElement_list = NULL;
threeTerminalsElement *head_threeTerminalsElement_list = NULL, *last_threeTerminalsElement_list = NULL;
fourTerminalsElement *head_fourTerminalsElement_list = NULL, *last_fourTerminalsElement_list = NULL;

cs_di *G2, *C2, *temp_mat, *temp_mat2;
cs_dis *S;
cs_din *N;

cs_ci *G2complex;
cs_cis *Scomplex;
cs_cin *Ncomplex;

double complex *matrix_Gcomplex, *vector_b_complex, *vector_x_complex, *vector_y_complex;
double complex *inv_preconditioner_complex;

int main(int argc, char* argv[])
{
	FILE *p_file = NULL;
	struct timeval calc_start, calc_end;
	char *ptr;

	if (argc != 2)
	{
		printf( "Invalid argument count.\n" );
		printf( "./circuit_simulation <input>\n" );
		printf( "To see the netlist help type,\n" );
		printf( "./circuit_simulation -help\n" );
		printf( "Terminating.\n" );
		exit( -1 );
	}

	if (!strcmp( argv[1], "-help" ))
	{
		Help();
		exit( 0 );
	}

	ptr = strrchr( argv[1], '/' );

	if (ptr == NULL)
	{
		strcpy( circuit_simulation.file_name, argv[1] );
	}
	else
	{
		strcpy( circuit_simulation.file_name, ptr + 1 );
	}

	p_file = fopen( argv[1], "r" );

	if (p_file == NULL)
	{
		printf( "The file does not exist.\n" );
		printf( "Terminating.\n" );
		exit( -1 );
	}

	init();

	gettimeofday( &calc_start, NULL );

	parseInput( p_file );
	
	fclose( p_file );

	if (circuit_simulation.dot_graph == DOT_GRAPH_ON)
	{
		Graph();
	}

	DCAnalysis();

	if (circuit_simulation.number_of_sweeps > 0)
	{
		DCSweep();
	}

	if (circuit_simulation.number_of_transient_analysis > 0)
	{
		TransientAnalysis();
	}
	else {
		freeAllocationsDC();
	}
	
	if (circuit_simulation.number_of_ac_analysis > 0) {
	      ACAnalysis();
	}
	
	freeAllocationsFinal();

	gettimeofday( &calc_end, NULL );

	printf( "\n\nDone in, %ld ms.\n\n", (calc_end.tv_sec * 1000 + calc_end.tv_usec / 1000) - (calc_start.tv_sec * 1000 + calc_start.tv_usec / 1000) );

	return 0;

}

void init(void)
{
	int i;

	ground_found = 0;

	matrix_G = NULL;
	matrix_C = NULL;
	vector_b = NULL;
	transposition = NULL;
	vector_y = NULL;
	vector_x = NULL;

	root_node = NULL;

	temp_matrix = NULL;
	temp_matrix2 = NULL;

	G2 = NULL;
	C2 = NULL;
	temp_mat = NULL;
	temp_mat2 = NULL;
	S = NULL;
	N = NULL;
	
	
	matrix_Gcomplex = NULL;
	
	G2complex = NULL;
	vector_b_complex = NULL;
	vector_x_complex = NULL;
	vector_y_complex = NULL;
	
	inv_preconditioner_complex = NULL;
	
	Scomplex = NULL;
	Ncomplex = NULL;

	sources = NULL;

	circuit_simulation.plot_settings = NULL;

	inv_preconditioner = NULL;

	circuit_simulation.plot_settings = ( plotSettings * ) malloc( sizeof(plotSettings) );

	if (circuit_simulation.plot_settings == NULL)
	{
		printf( "Could not allocate memory.\n" );
		printf( "Terminating.\n" );
		exit( -1 );
	}

	strcpy( circuit_simulation.plot_settings->name, "0" );
	circuit_simulation.plot_settings->dc_sweep_plot = DISABLED;
	circuit_simulation.plot_settings->transient_plot = DISABLED;
	circuit_simulation.plot_settings->ac_plot = DISABLED;
	
	circuit_simulation.number_of_sweeps = 0;
	circuit_simulation.number_of_transient_analysis = 0;
	circuit_simulation.number_of_ac_analysis = 0;
	
	circuit_simulation.dc_sweep_settings = NULL;
	circuit_simulation.transient_analysis_settings = NULL;
	circuit_simulation.ac_analysis_settings = NULL;

	circuit_simulation.number_of_nodes = 0;
	circuit_simulation.group1_elements = 0;
	circuit_simulation.group2_elements = 0;

	for (i = 0; i < ELEMENTS; i++)
	{
		circuit_simulation.number_of_elements[i] = 0;
	}
	
	circuit_simulation.ac_plot_scale = LINEAR;

	non_zeroes_G = 0;
	non_zeroes_C = 0;
	non_zeroes_Gcomplex = 0;

	circuit_simulation.matrix_sparsity = NORMAL;
	circuit_simulation.method = DIRECT;
	circuit_simulation.matrix_type = NONSPD;
	circuit_simulation.diff_method = TRAPEZOIDAL;	    

	circuit_simulation.iterative_methods.itol = 1e-3;
	circuit_simulation.iterative_methods.EPS = 1e-16;
	circuit_simulation.iterative_methods.division_by_zero = 0;

	circuit_simulation.dc_analysis_settings.operating_point = DISABLED;
	

	circuit_simulation.gnuplot = GNUPLOT_OFF;
	circuit_simulation.dot_graph = DOT_GRAPH_OFF;

}

void freeAllocationsFinal(void) {
    twoTerminalsElement *current1 = NULL, *last1 = NULL; 
    threeTerminalsElement *current2 = NULL, *last2 = NULL;
    fourTerminalsElement *current3 = NULL, *last3 = NULL;

	for (last1 = current1 = head_twoTerminalsElement_list; head_twoTerminalsElement_list != NULL && (current1->next != NULL);)
	{
		current1 = current1->next;
		if (last1->transient != NULL)
		{
			switch (last1->transient->impulse)
			{
				case EXP:
					free( last1->transient->exp );
					break;
				case SIN:
					free( last1->transient->sin );
					break;
				case PULSE:
					free( last1->transient->pulse );
					break;
				case PWL:
					free( last1->transient->pwl->t );
					free( last1->transient->pwl->i );
					free( last1->transient->pwl );
                                        break;
                                case SFFM:
                                        free(last1->transient->sffm);
					break;
			}		
			free( last1->transient );
		}
		
		if(last1->ac != NULL) {
			free(last1->ac);
		}
		
		free( last1 );
		last1 = current1;
	}

	if (last1->transient != NULL)
	{
		switch (last1->transient->impulse)
		{
			case EXP:
				free( last1->transient->exp );
				break;
			case SIN:
				free( last1->transient->sin );
				break;
			case PULSE:
				free( last1->transient->pulse );
				break;
			case PWL:
				free( last1->transient->pwl->t );
				free( last1->transient->pwl->i );
				free( last1->transient->pwl );
				break;
                        case SFFM:
                                free(last1->transient->sffm);
				break;
		}
		free( last1->transient );
	}
	
	if(last1->ac != NULL) {
		free(last1->ac);
	}
		
	free( last1 );

	for (last2 = current2 = head_threeTerminalsElement_list; head_threeTerminalsElement_list != NULL && (current2->next != NULL);)
	{
		current2 = current2->next;
		free( last2 );
		last2 = current2;
	}
	free( last2 );

	for (last3 = current3 = head_fourTerminalsElement_list; head_fourTerminalsElement_list != NULL && (current3->next != NULL);)
	{
		current3 = current3->next;
		free( last3 );
		last3 = current3;
	}
	free( last3 );
	
	free( circuit_simulation.plot_settings );
	
		
	if(circuit_simulation.dc_sweep_settings != NULL)
	{
		free(circuit_simulation.dc_sweep_settings);
	}
	
		
	if(circuit_simulation.transient_analysis_settings != NULL)
	{
		free(circuit_simulation.transient_analysis_settings);
	}
	
		
	if(circuit_simulation.ac_analysis_settings != NULL)
	{
		free(circuit_simulation.ac_analysis_settings);
	}
  
}

void freeAllocationsTransient(void) {
  int i;
	
			
	for(i = 0; i < circuit_simulation.number_of_elements[CURRENT_SOURCE] + circuit_simulation.group2_elements; i++) {
	      if(sources[i] != NULL && sources[i]->transient != NULL) {
		  free(sources[i]->transient->vals);
	      }
	}
	
	free( sources );

	if (circuit_simulation.matrix_sparsity == SPARSE)
	{
		cs_di_spfree( C2 );
	}
	else
	{
		free( matrix_C );
	}
	

	if (circuit_simulation.matrix_sparsity == SPARSE)
	{
		if (circuit_simulation.method == DIRECT)
		{ 
			cs_di_sfree( S );
			cs_di_nfree( N );
		}
		else
		{
			cs_di_spfree( G2 );
		}
	}
	else
	{
		free( matrix_G );

		if (circuit_simulation.method == DIRECT && circuit_simulation.matrix_type == NONSPD)
		
		{
			free( transposition );
		}
	}

}

void freeAllocationsDC(void) {

	if (circuit_simulation.matrix_sparsity == SPARSE)
	{
		if (circuit_simulation.method == DIRECT)
		{ 
			cs_di_sfree( S );
			cs_di_nfree( N );
		}
		else
		{
			cs_di_spfree( G2 );
		}
	}
	else
	{
		free( matrix_G );

		if (circuit_simulation.method == DIRECT && circuit_simulation.matrix_type == NONSPD)
		
		{
			free( transposition );
		}
	}

	if (circuit_simulation.method == ITERATIVE)
	{
		free( inv_preconditioner );
	}

	free( vector_x );

	free( vector_b );

}

void freeAllocationsAC(void) {
      
        if (circuit_simulation.matrix_sparsity == SPARSE)
	{
	      if (circuit_simulation.method == DIRECT)
	      {  
		    cs_ci_sfree( Scomplex );
		    cs_ci_nfree( Ncomplex );
	      }
	      else
	      {
		    cs_ci_spfree( G2complex );
	      }
	}
	else
	{
	    free( matrix_Gcomplex );

	    if (circuit_simulation.method == DIRECT && circuit_simulation.matrix_type == NONSPD)
	    {
		  free( transposition );
	    }
			  
        }
        
        if (circuit_simulation.method == ITERATIVE)
	{
		free( inv_preconditioner_complex );
	}
}


void Help(void)
{

	printf( "\nValid Netlist Declarations.\n\n" );

	printf( "Elements\n" );
	printf( "________\n\n" );

	printf( "Voltage Source:\n" );
	printf( "V<name> <(+) positive node name>  <(-) negative node name> <value in Volt> [transient part] [ac part]\n\n" );
	printf( "transient part:\n" );
	printf( "EXP(i1 i2 td1 tc1 td2 tc2)\nSIN(i1 ia fr td df ph)\nPULSE(i1 i2 td tr tf pw per)\nPWL(t1 i1) (t2 i2)...(tn in)\nSFFM(v0 va fc mdi fs)\n\n" );
        printf( "ac part:\n" );
	printf( "AC magnitude phase\n\n");
	
	printf( "Current Source:\n" );
	printf( "I<name> <(+) positive node name>  <(-) negative node name> <value in Ampere> [transient part] [ac part]\n\n" );
	printf( "transient part:\n" );
	printf( "EXP(i1 i2 td1 tc1 td2 tc2)\nSIN(i1 ia fr td df ph)\nPULSE(i1 i2 td tr tf pw per)\nPWL(t1 i1) (t2 i2)...(tn in)\nSFFM(v0 va fc mdi fs)\n\n" );
        printf( "ac part:\n" );
	printf( "AC magnitude phase\n\n");


        printf( "Voltage Controlled Voltage Source:\n" );
	printf( "E<name> <(+) positive node name>  <(-) negative node name>  <(+) positive controlling node name>  <(-) negative controlling node name> <voltage gain>\n\n" );

        
        printf( "Voltage Controlled Current Source:\n" );
	printf( "G<name> <(+) positive node name>  <(-) negative node name>  <(+) positive controlling node name>  <(-) negative controlling node name> <transconductance>\n\n" );

         
        printf( "Short Circuit(Not compliant to spice format, emulates a zero volt voltage source):\n" );
	printf( "_<name> <(+) positive node name>  <(-) negative node name>\n\n" );
	
	printf( "Resistance:\n" );
	printf( "R<name> <(+) positive node name>  <(-) negative node name> <value in Ohm>\n\n" );

	printf( "Capacitor:\n" );
	printf( "C<name> <(+) positive node name>  <(-) negative node name> <value in Farad>\n\n" );

	printf( "Inductor:\n" );
	printf( "L<name> <(+) positive node name>  <(-) negative node name> <value in Henry>\n\n" );

	/*printf( "Diode:\n" );
	printf( "D<name> <(+) positive node name>  <(-) negative node name> <model name> [area]\n" );
	printf( "(Just for netlist parsing. The simulation works only with linear elements).\n\n" );

	printf( "Transistor MOS:\n" );
	printf( "M<name> <D>  <G> <S> <B> <model name> l=<value> w=<value>\n" );
	printf( "(Just for netlist parsing. The simulation works only with linear elements).\n\n" );

	printf( "Transistor BJT:\n" );
	printf( "Q<name> <C>  <B> <E> <model name> [area]\n" );
	printf( "(Just for netlist parsing. The simulation works only with linear elements).\n\n\n" );*/

	printf( "Linear Systems Solving Methods:\n" );
	printf( "_______________________________\n\n" );
	printf( "default LU.\n\n" );

	printf( "Cholesky:\n" );
	printf( ".OPTIONS SPD\n\n" );

	printf( "Iterative:\n" );
	printf( ".OPTIONS ITER\n\n" );

	printf( "Iterative for Symmetric/Hermitian - Positive Definite Matrices:\n" );
	printf( ".OPTIONS ITER SPD\n\n" );

	printf( "Iterative converging threshold:\n" );
	printf( "default 1e-3.\n\n" );

	printf( "Custom threshold:\n" );
	printf( ".OPTIONS ITOL=<threshold>\n\n\n" );

	printf( "Matrix Sparsity:\n" );
	printf( "_______________\n\n" );
	printf( "default normal matrix.\n\n" );

	printf( "Sparse:\n" );
	printf( ".OPTIONS SPARSE\n\n\n" );

	printf( "Differential Equations Solving Methods:\n" );
	printf( "______________________________________\n\n" );
	printf( "default Trapezoidal.\n\n" );

	printf( "Trapezoidal:\n" );
	printf( ".OPTIONS METHOD=TR\n\n" );

	printf( "Backward Euler:\n" );
	printf( ".OPTIONS METHOD=BE\n\n\n" );

	//printf( "Forward Euler:\n" );
	//printf( ".OPTIONS METHOD=FE\n\n\n" );

	printf( "Graphs:\n" );
	printf( "_______\n\n" );
	printf( "Use gnuplot to visualize the results of DC Sweep, Transient Analysis, and AC Analysis.\n" );
	printf( "You also need to specify the plotted nodes, using the PLOT/PRINT options (See below).\n" );
	printf( ".OPTIONS GNUPLOTLP, to plot using lines and points\n\n" );
	printf( ".OPTIONS GNUPLOTL, to plot using lines.\n\n" );
	printf( "Use xdot to visualize your circuit. The visualization only works and looks good for small circuits.\n");
	printf( ".OPTIONS GRAPH\n\n\n");

	printf( "DC Analysis:\n" );
	printf( "_________\n\n" );
	printf( "To print the currents of each element at DC_Analysis.txt, use\n" );
	printf( ".OP\n\n\n" );

	printf( "DC Sweep:\n" );
	printf( "_________\n\n" );

	printf( ".DC <V or I><name> <start> <end> <step>\n\n\n" );

	printf( "Transient Analysis:\n" );
	printf( "___________________\n\n" );
	printf( "To perform the transient analysis of the circuit, you need to type,\n" );
	printf( ".TRAN <time step> <finish time>\n\n\n" );
	
	printf( "AC Analysis:\n" );
	printf( "___________________\n\n" );
	printf( "To perform the ac analysis of the circuit, you need to type,\n" );
	printf( ".AC <sweep method> <points> <starting frequency> <ending frequency>\n" );
	printf( "The sweep method can be either LIN or LOG. In LIN sweep method the points will\n");
	printf( "be the total number of points. In LOG sweep method the points will be the points\n");
	printf( "per decade.\n\n\n");
	
	printf("To print the results of the sweep/transient/ac on the file DC_sweep.txt/Transient_Analysis.txt/AC_Analysis.txt\nyou need to write the following command,\n" );
	printf( ".PLOT or .PRINT <mode> V(<node name>) V(<node name>) ... V(<node name>)\n" );
	printf( "where <mode>, is either DC or TRAN or AC (DC Sweep / Transient Analysis / AC Analysis).\n" );
	printf( "where V(<node name>), will print the sweep/transient result of the Voltage at node <node name>.\n" );
	printf( "The PLOT/PRINT options will also have effect on the gnuplot graph.\n" );
	printf( "Additionally in AC mode you can set the scale of the magnitude using either LINEAR or DB.\n\n\n");

	printf( "Formating:\n" );
	printf( "__________\n\n" );
	printf( "The application is case insensitive.\n" );
	printf( "Empty lines, or lines starting with * (comments) will be ignored.\n" );
	printf( "Extra spaces or tabs will be ignored.\n" );
	printf( "Expressions enclosed in square brackets, as shown in voltage sources,\ncurrent sources, and transistors, can be left blank.\n\n" );

}

void Graph(void)
{
	FILE *file_ptr = NULL, *pipe = NULL;
	twoTerminalsElement *current1 = NULL;

	file_ptr = fopen( "graph.txt", "w" );
	fprintf( file_ptr, "digraph G {\nrankdir=LR;\n" );
	fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );

	for (current1 = head_twoTerminalsElement_list; head_twoTerminalsElement_list != NULL && current1 != NULL; current1 = current1->next)
	{

		switch (current1->type)
		{
			case VOLTAGE_SOURCE:
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name );
				fprintf( file_ptr, "node [shape=square,  style=bold, color=blue];\n" );
				fprintf( file_ptr, "\"V%s\"\n", current1->string_name );
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->negative_terminal)->name );
				fprintf( file_ptr, "edge [dir=none, style=bold, color=blue, label=\"+\"];\n" );
				fprintf( file_ptr, "\"%s\" -> \"V%s\";\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name,
						current1->string_name );
				fprintf( file_ptr, "edge [dir=forward, style=bold, color=blue, label=\"-\"];\n" );
				fprintf( file_ptr, "\"V%s\" -> \"%s\";\n", current1->string_name,
						(circuit_simulation.plot_settings + current1->negative_terminal)->name );
				break;
                        case SHORT_CIRCUIT:
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name );
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->negative_terminal)->name );
				fprintf( file_ptr, "edge [dir=forward, style=bold, color=black, label=\"+ -\"];\n" );
				fprintf( file_ptr, "\"%s\" -> \"%s\";\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name,
						(circuit_simulation.plot_settings + current1->negative_terminal)->name );
				break;
			case VOLTAGE_CONTROLLED_VOLTAGE_SOURCE:
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name );
				fprintf( file_ptr, "node [shape=square,  style=bold, color=yellow];\n" );
				fprintf( file_ptr, "\"E%s\"\n", current1->string_name );
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->negative_terminal)->name );
				fprintf( file_ptr, "edge [dir=none, style=bold, color=yellow, label=\"+\"];\n" );
				fprintf( file_ptr, "\"%s\" -> \"E%s\";\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name,
						current1->string_name );
				fprintf( file_ptr, "edge [dir=forward, style=bold, color=yellow, label=\"-\"];\n" );
				fprintf( file_ptr, "\"E%s\" -> \"%s\";\n", current1->string_name,
						(circuit_simulation.plot_settings + current1->negative_terminal)->name );
				break;
                        case VOLTAGE_CONTROLLED_CURRENT_SOURCE:
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name );
				fprintf( file_ptr, "node [shape=square,  style=bold, color=cyan];\n" );
				fprintf( file_ptr, "\"G%s\"\n", current1->string_name );
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->negative_terminal)->name );
				fprintf( file_ptr, "edge [dir=none, style=bold, color=cyan, label=\"+\"];\n" );
				fprintf( file_ptr, "\"%s\" -> \"G%s\";\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name,
						current1->string_name );
				fprintf( file_ptr, "edge [dir=forward, style=bold, color=cyan, label=\"-\"];\n" );
				fprintf( file_ptr, "\"G%s\" -> \"%s\";\n", current1->string_name,
						(circuit_simulation.plot_settings + current1->negative_terminal)->name );
				break;
			case CURRENT_SOURCE:
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name );
				fprintf( file_ptr, "node [shape=square,  style=bold, color=red];\n" );
				fprintf( file_ptr, "\"I%s\"\n", current1->string_name );
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->negative_terminal)->name );
				fprintf( file_ptr, "edge [dir=none, style=bold, color=red, label=\"+\"];\n" );
				fprintf( file_ptr, "\"%s\" -> \"I%s\";\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name,
						current1->string_name );
				fprintf( file_ptr, "edge [dir=forward, style=bold, color=red, label=\"-\"];\n" );
				fprintf( file_ptr, "\"I%s\" -> \"%s\";\n", current1->string_name,
						(circuit_simulation.plot_settings + current1->negative_terminal)->name );
				break;
			case RESISTANCE:
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name );
				fprintf( file_ptr, "node [shape=square,  style=bold, color=green];\n" );
				fprintf( file_ptr, "\"R%s\"\n", current1->string_name );
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->negative_terminal)->name );
				fprintf( file_ptr, "edge [dir=none, style=bold, color=green, label=\"+\"];\n" );
				fprintf( file_ptr, "\"%s\" -> \"R%s\";\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name,
						current1->string_name );
				fprintf( file_ptr, "edge [dir=forward, style=bold, color=green, label=\"-\"];\n" );
				fprintf( file_ptr, "\"R%s\" -> \"%s\";\n", current1->string_name,
						(circuit_simulation.plot_settings + current1->negative_terminal)->name );
				break;
			case CAPACITOR:
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name );
				fprintf( file_ptr, "node [shape=square,  style=bold, color=orange];\n" );
				fprintf( file_ptr, "\"C%s\"\n", current1->string_name );
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->negative_terminal)->name );
				fprintf( file_ptr, "edge [dir=none, style=bold, color=orange, label=\"+\"];\n" );
				fprintf( file_ptr, "\"%s\" -> \"C%s\";\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name,
						current1->string_name );
				fprintf( file_ptr, "edge [dir=forward, style=bold, color=orange, label=\"-\"];\n" );
				fprintf( file_ptr, "\"C%s\" -> \"%s\";\n", current1->string_name,
						(circuit_simulation.plot_settings + current1->negative_terminal)->name );
				break;
			case INDUCTOR:
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name );
				fprintf( file_ptr, "node [shape=square,  style=bold, color=magenta];\n" );
				fprintf( file_ptr, "\"L%s\"\n", current1->string_name );
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->negative_terminal)->name );
				fprintf( file_ptr, "edge [dir=none, style=bold, color=magenta, label=\"+\"];\n" );
				fprintf( file_ptr, "\"%s\" -> \"L%s\";\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name,
						current1->string_name );
				fprintf( file_ptr, "edge [dir=forward, style=bold, color=magenta, label=\"-\"];\n" );
				fprintf( file_ptr, "\"L%s\" -> \"%s\";\n", current1->string_name,
						(circuit_simulation.plot_settings + current1->negative_terminal)->name );
				break;
			case DIODE:
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name );
				fprintf( file_ptr, "node [shape=square,  style=bold, color=grey];\n" );
				fprintf( file_ptr, "\"D%s\"\n", current1->string_name );
				fprintf( file_ptr, "node [shape=circle,  style=bold, color=black];\n" );
				fprintf( file_ptr, "\"%s\"\n", (circuit_simulation.plot_settings + current1->negative_terminal)->name );
				fprintf( file_ptr, "edge [dir=none, style=bold, color=grey, label=\"+\"];\n" );
				fprintf( file_ptr, "\"%s\" -> \"D%s\";\n", (circuit_simulation.plot_settings + current1->positive_terminal)->name,
						current1->string_name );
				fprintf( file_ptr, "edge [dir=forward, style=bold, color=grey, label=\"-\"];\n" );
				fprintf( file_ptr, "\"D%s\" -> \"%s\";\n", current1->string_name,
						(circuit_simulation.plot_settings + current1->negative_terminal)->name );
				break;
			default:
				break;

		}

	}

	fprintf( file_ptr, "node [shape=point,  style=bold];\n" );
	fprintf( file_ptr, "edge [dir=forward, shape=normal, style=bold, color=black, label=\"\"];\n" );
	fprintf( file_ptr, "\"%s\" -> \" \";\n", circuit_simulation.plot_settings->name );

	fprintf( file_ptr, "}\n" );

	fclose( file_ptr );

	pipe = popen( "xdot graph.txt & > /dev/null 2>&1", "w" );
	fclose( pipe );

}
