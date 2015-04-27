#include "circuit_simulation.h"

double calculate_next_decade(double decade) {
  
     if(decade - (int)decade != 0) {
         if(decade < 0) {
	     return (int)decade;
         }
         else {
             return (int)decade + 1.0;
	 }		
     }
     else {
         return (int)decade + 1.0;
     }
  
}

void ACAnalysis(void)
{
        FILE *p_file = NULL, *p_file2 = NULL, *pipe = NULL, *pipe2 = NULL;
	int dimension, l, j, counter;
	twoTerminalsElement *current1 = NULL;
	char temp[NAME_SIZE];
	char string[3000];
	char temp_string[300];
	char string2[3000];
	char temp_string2[300];
	double f, end_freq_decade;
	double step;
	double arg, mag;
	double pi_180 = M_PI / 180;
	int points_per_decade, total_decades;
        long int total_points, i;
	double decade, temp_decade1, temp_decade2;	


	printf( "\n\nAC Analysis...\n\n" );

	dimension = number_of_nodes + group2_elements;

	vector_b_complex = ( double complex * ) calloc( dimension, sizeof(double complex) );
	vector_x_complex = ( double complex * ) calloc( dimension, sizeof(double complex) );

	if (vector_b_complex == NULL || vector_x_complex == NULL)
	{
		printf( "Could not allocate matrices.\n" );
		printf( "Terminating.\n" );
		exit( -1 );
	}
	
	if(circuit_simulation.ac_analysis_settings.scale == LIN) {
	    step = (circuit_simulation.ac_analysis_settings.end_freq - circuit_simulation.ac_analysis_settings.start_freq) / circuit_simulation.ac_analysis_settings.points;
	
	    total_points = circuit_simulation.ac_analysis_settings.points + 1;
	}
	else {	    
	    decade = log10(circuit_simulation.ac_analysis_settings.start_freq);
	    end_freq_decade = log10(circuit_simulation.ac_analysis_settings.end_freq);
	    
	    if(decade - (int)decade != 0) {
	        temp_decade1 = (int)(decade - 1); //precious decade
	    }
	    else {
	        temp_decade1 = decade; //the number is a decade
	    }
	    
	    if(end_freq_decade - (int)end_freq_decade != 0) {
	        temp_decade2 = (int)(end_freq_decade + 1); // next decade
	    }
	    else {
                temp_decade2 = end_freq_decade; //the number is a decade
            }
            
            total_decades = temp_decade2 - temp_decade1;
	    
	    
	    if((int)end_freq_decade - (int)decade != 0) { //Not in the same decade
		 step = (calculate_next_decade(decade) - decade) / circuit_simulation.ac_analysis_settings.points;
	    }
	    else { // same decade	        
	         step = (end_freq_decade - decade) / circuit_simulation.ac_analysis_settings.points;
	    }
	            
	    total_points = total_decades * circuit_simulation.ac_analysis_settings.points + 1;
	}
	
	//printf( "\n\nPrinting results to AC_Analysis.txt ...\n\n" );
	
	p_file = fopen( "AC_Analysis.txt", "w" );

	fprintf( p_file, "The circuit, %s, contains:\n\n", circuit_simulation.file_name );

	fprintf( p_file, "          Nodes: %7d\n", number_of_nodes + 1 );

	if (number_of_elements[VOLTAGE_SOURCE])
	{
		fprintf( p_file, "Voltage Sources: %7d\n", number_of_elements[VOLTAGE_SOURCE] );
	}

	if (number_of_elements[CURRENT_SOURCE])
	{
		fprintf( p_file, "Current Sources: %7d\n", number_of_elements[CURRENT_SOURCE] );
	}

	if (number_of_elements[RESISTANCE])
	{
		fprintf( p_file, "    Resistances: %7d\n", number_of_elements[RESISTANCE] );
	}

	if (number_of_elements[CAPACITOR])
	{
		fprintf( p_file, "     Capacitors: %7d\n", number_of_elements[CAPACITOR] );
	}

	if (number_of_elements[INDUCTOR])
	{
		fprintf( p_file, "      Inductors: %7d\n", number_of_elements[INDUCTOR] );
	}

	if (number_of_elements[DIODE])
	{
		fprintf( p_file, "         Diodes: %7d\n", number_of_elements[DIODE] );
	}

	if (number_of_elements[BJT])
	{
		fprintf( p_file, "BJT Transistors: %7d\n", number_of_elements[BJT] );
	}

	if (number_of_elements[MOS])
	{
		fprintf( p_file, "MOS Transistors: %7d\n", number_of_elements[MOS] );
	}
	
	if(circuit_simulation.ac_analysis_settings.scale == LOG) {
	    fprintf( p_file, "\n\n\nAC Analysis    start: %20.10lf,  stop: %20.10lf,  step: log10\n", circuit_simulation.ac_analysis_settings.start_freq,
			      circuit_simulation.ac_analysis_settings.end_freq);
	    fprintf( p_file, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n" );

      #if DEBUG
	      printf("\n\nAC ANALYSIS    start: %20.10lf,  stop: %20.10lf,  step: log10\n", circuit_simulation.ac_analysis_settings.start_freq,
			      circuit_simulation.ac_analysis_settings.end_freq);
	      printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n");
      #endif
	}
	else {
	      fprintf( p_file, "\n\n\nAC Analysis    start: %20.10lf,  stop: %20.10lf,  step: %20.10lf\n", circuit_simulation.ac_analysis_settings.start_freq,
			      circuit_simulation.ac_analysis_settings.end_freq, step );
	      fprintf( p_file, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n" );

      #if DEBUG
	      printf("\n\nAC ANALYSIS    start: %20.10lf,  stop: %20.10lf,  step: %20.10lf\n", circuit_simulation.ac_analysis_settings.start_freq,
			      circuit_simulation.ac_analysis_settings.end_freq, step);
	      printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n");
      #endif
	}
	
	if (circuit_simulation.gnuplot)
	{
		p_file2 = fopen( "ac_plot.txt", "w" );
	}
		
	
	for(i = 0, points_per_decade = 0; i < total_points; i++, points_per_decade++) {	  

	    if(circuit_simulation.ac_analysis_settings.scale == LIN) {
	        f = circuit_simulation.ac_analysis_settings.start_freq + i * step;
	    }
	    else {
	        f = pow(10, decade + points_per_decade * step);
	    }

	    createMnaSystemAC(f, i);
	    
	    #if DEBUG
		printf("\nf = %22.9lf Hz", f);
		printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
            #endif
		
	    
	    if (circuit_simulation.method == DIRECT)
	    {
		    if (circuit_simulation.matrix_type == NONSPD)
		    {
			    LUDecompositionComplex(i);
		    }
		    else
		    {
			    CholeskyDecompositionComplex(i);
		    }

		    solve_direct_methods_Complex();
	    }
	    else
	    {
	            /*if(i == 0) {
		        printf( "Creating Preconditioner.\n\n" );
		    }*/
		    createInversePreconditionerComplex( dimension );
		    		        
		    if (circuit_simulation.matrix_type == NONSPD)
		    {
			    BiConjugateGradientComplex(1);
		    }
		    else
		    {
			    ConjugateGradientComplex(1);
		    }
	    }
	    
	    if(i == 0) {
		  if (circuit_simulation.method == DIRECT)
		  {
			  printf( "Solving with Backward/Forward substitution.\n\n" );
		  }
		  else
		  {
			  if (circuit_simulation.matrix_type == NONSPD)
			  {
				  printf( "Solving with Bi-Conjugate Gradient.\n\n" );
			  }
			  else
			  {
				  printf( "Solving with Conjugate Gradient.\n\n" );
			  }
		  }
	    }
	    
	    	DrawProgressBar( 30, (i + 1.0) / total_points );
	    
	        fprintf( p_file, "\nf = %22.9lf Hz", f );
		fprintf( p_file, "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" );

		if(circuit_simulation.ac_analysis_settings.plot_scale == DB) {
		    fprintf( p_file, "\n_____________________________________________________Magnitude[dB]______________Phase_______________" );
		}
		else {
		    fprintf( p_file, "\n______________________________________________________Magnitude_________________Phase_______________" );
		}
		
		for (l = 0; l < number_of_nodes + 1; l++)
		{
			if ((circuit_simulation.plot_settings + l)->ac_plot)
			{
				if (l == 0)
				{
					fprintf( p_file, "\n|   Voltage @ node| %25s: %22.14e %22.14e   Volt|", circuit_simulation.plot_settings->name, 0.0, 0.0 );
				}
				else
				{
				      mag = cabs(vector_x_complex[l - 1]);   
				      mag = circuit_simulation.ac_analysis_settings.plot_scale == DB ? 20 * log10(mag) : mag;
				      
				      arg = carg(vector_x_complex[l - 1]) / pi_180;
				      //arg = arg < 0 ? arg + 360 : arg;
				      				      
				      fprintf( p_file, "\n|   Voltage @ node| %25s: %22.14e %22.14e   Volt|", (circuit_simulation.plot_settings + l)->name,
							mag, arg );
				}
			}
		}
		fprintf( p_file, "\n____________________________________________________________________________________________________\n\n" );

		
		if (circuit_simulation.gnuplot)
		{
			fprintf( p_file2, "%.14lf ", f );
			
			for (l = 0; l < number_of_nodes + 1; l++)
			{
				if ((circuit_simulation.plot_settings + l)->ac_plot)
				{
				    if(l == 0) {
					fprintf( p_file2, "%.14lf ", 0.0 ); 
					fprintf( p_file2, "%.14lf ", 0.0 );
				    }
				    else {
					mag = cabs(vector_x_complex[l - 1]);   
					mag = circuit_simulation.ac_analysis_settings.plot_scale == DB ? 20 * log10(mag) : mag;
					      
					fprintf( p_file2, "%.14lf ", mag );
					
					arg = carg(vector_x_complex[l - 1]) / pi_180;
					//arg = arg < 0 ? arg + 360 : arg;
					
					fprintf( p_file2, "%.14lf ", arg );
				    }				  
				}
			}
			fprintf( p_file2, "\n" );
		}
	    
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
			    free( inv_preconditioner_complex );
		    }
	    }
	    else
	    {
		    free( matrix_Gcomplex );

		    if (circuit_simulation.method == DIRECT && circuit_simulation.matrix_type == NONSPD)
				    //&& circuit_simulation.transient_analysis_settings.diff_method != FORWARD_EULER)
		    {
			    free( transposition );
		    }
		    else if (circuit_simulation.method == ITERATIVE) {
		            free( inv_preconditioner_complex );
		    }
		    
	    }
	    
	    
	    if(circuit_simulation.ac_analysis_settings.scale == LOG) {
	         if(points_per_decade == circuit_simulation.ac_analysis_settings.points) {
		      
		      decade = calculate_next_decade(decade);
		          
		      if(end_freq_decade > (int)decade + 1) {
			  step = (((int)decade) + 1 - decade) / circuit_simulation.ac_analysis_settings.points;
		      }
		      else {
			  step = (end_freq_decade - decade) / circuit_simulation.ac_analysis_settings.points;	      
		      }
		      
		      points_per_decade = 0;
		 }
	    }
	}
	
	DrawProgressBar( 30, 1.0 );
	
	if (circuit_simulation.gnuplot)
	{
		fclose( p_file2 );

		for (l = 0, counter = 0; l < number_of_nodes + 1; l++)
		{
			if ((circuit_simulation.plot_settings + l)->ac_plot)
			{
				counter++;
			}
		}

		if (counter != 0)
		{
			pipe = popen( "gnuplot -persist", "w" );
			pipe2 = popen( "gnuplot -persist", "w" );

			string[0] = '\0';
			strcat( string, "plot " );
			
			string2[0] = '\0';
			strcat( string2, "plot " );
			for (l = 0, j = 0; j < counter && l < number_of_nodes + 1; l++)
			{
				if ((circuit_simulation.plot_settings + l)->ac_plot)
				{
					j++;
					if (j == counter)
					{
						if (circuit_simulation.gnuplot == LINES_POINTS)
						{
							sprintf( temp_string, "\"./ac_plot.txt\" using 1:%d title '%s' with linespoints", 2 * j,
									(circuit_simulation.plot_settings + l)->name );
							sprintf( temp_string2, "\"./ac_plot.txt\" using 1:%d title '%s' with linespoints", 2 * j + 1,
									(circuit_simulation.plot_settings + l)->name );
						}
						else
						{
							sprintf( temp_string, "\"./ac_plot.txt\" using 1:%d title '%s' with lines", 2 * j,
									(circuit_simulation.plot_settings + l)->name );
							sprintf( temp_string2, "\"./ac_plot.txt\" using 1:%d title '%s' with lines", 2 * j + 1,
									(circuit_simulation.plot_settings + l)->name );
						}
						strcat( string, temp_string );
						strcat( string2, temp_string2 );
					}
					else
					{
						if (circuit_simulation.gnuplot == LINES_POINTS)
						{
							sprintf( temp_string, "\"./ac_plot.txt\" using 1:%d title '%s' with linespoints, ", 2 * j,
									(circuit_simulation.plot_settings + l)->name );
							sprintf( temp_string2, "\"./ac_plot.txt\" using 1:%d title '%s' with linespoints, ", 2 * j + 1,
									(circuit_simulation.plot_settings + l)->name );
						}
						else
						{
							sprintf( temp_string, "\"./ac_plot.txt\" using 1:%d title '%s' with lines, ", 2 * j,
									(circuit_simulation.plot_settings + l)->name );
							sprintf( temp_string2, "\"./ac_plot.txt\" using 1:%d title '%s' with lines, ", 2 * j + 1,
									(circuit_simulation.plot_settings + l)->name );
						}
						strcat( string, temp_string );
						strcat( string2, temp_string2 );
					}
				}
			}
			
			if(circuit_simulation.ac_analysis_settings.scale == LOG) {
			    sprintf( temp_string, "; set logscale x");
			    sprintf( temp_string2, "; set logscale x");
			    
			    strcat( string, temp_string );
			    strcat( string2, temp_string2 );
			}

			if(circuit_simulation.ac_analysis_settings.plot_scale == DB) {
			    sprintf( temp_string,
					"; set grid; set zeroaxis; set key out horiz top center; set terminal wxt size 800,600; set title \"AC Analysis\t#circuit %s\"; set autoscale; set xlabel \"Frequency [Hz]\"; set ylabel \"V Magnitude [dB]\"; replot;",
					circuit_simulation.file_name );
			}
			else {
			    sprintf( temp_string,
					"; set grid; set zeroaxis; set key out horiz top center; set terminal wxt size 800,600; set title \"AC Analysis\t#circuit %s\"; set autoscale; set xlabel \"Frequency [Hz]\"; set ylabel \"V Magnitude [Volt]\"; replot;",
					circuit_simulation.file_name );
			}
			
			strcat( string, temp_string );
			
			sprintf( temp_string2,
					"; set grid; set zeroaxis; set key out horiz top center; set terminal wxt size 800,600; set title \"AC Analysis\t#circuit %s\"; set autoscale; set xlabel \"Frequency [Hz]\"; set ylabel \"V Phase [Degrees]\"; replot;",
					circuit_simulation.file_name );
			strcat( string2, temp_string2 );
			
			fprintf( pipe, "%s\n", string );
			fprintf( pipe2, "%s\n", string2 );
			fclose( pipe );
			fclose( pipe2 );
		}
	}

        fclose( p_file );
	
	if(circuit_simulation.method == ITERATIVE && circuit_simulation.matrix_type == NONSPD && circuit_simulation.iterative_methods.division_by_zero == 1) {
	    printf("\nWarning, division by zero was encountered during Bi-Conjugate Gradient execution.\n");
	    printf("The results might be inaccurate.\n\n");
	    circuit_simulation.iterative_methods.division_by_zero = 0;
	}
}

void DCAnalysis(void)
{
	FILE *p_file = NULL;
	int i, dimension;
	twoTerminalsElement *current1 = NULL;
	char temp[NAME_SIZE];

	printf( "\n\nDC Analysis...\n\n" );

	dimension = number_of_nodes + group2_elements;

	vector_b = ( double * ) calloc( dimension, sizeof(double) );
	vector_x = ( double * ) calloc( dimension, sizeof(double) );

	if (vector_b == NULL || vector_x == NULL)
	{
		printf( "Could not allocate matrices.\n" );
		printf( "Terminating.\n" );
		exit( -1 );
	}

	createMnaSystemDC();

	if (circuit_simulation.method == DIRECT)
	{
		if (circuit_simulation.matrix_type == NONSPD)
		{
			LUDecomposition();
		}
		else
		{
			CholeskyDecomposition();
		}
		
		printf( "Solving with Backward/Forward substitution.\n\n" );

		solve_direct_methods();
	}
	else
	{
		//printf( "Creating Preconditioner.\n\n" );
	        createInversePreconditioner( dimension );

		if (circuit_simulation.matrix_type == NONSPD)
		{
		       printf( "Solving with Bi-Conjugate Gradient.\n\n" );	
		       BiConjugateGradient(1);
		}
		else
		{
		       printf( "Solving with Conjugate Gradient.\n\n" );
		       ConjugateGradient(1);
		}
	}


	//printf( "\n\nPrinting results to DC_Analysis.txt ...\n\n" );

	p_file = fopen( "DC_analysis.txt", "w" );

	fprintf( p_file, "The circuit, %s, contains:\n\n", circuit_simulation.file_name );

	fprintf( p_file, "          Nodes: %7d\n", number_of_nodes + 1 );

	if (number_of_elements[VOLTAGE_SOURCE])
	{
		fprintf( p_file, "Voltage Sources: %7d\n", number_of_elements[VOLTAGE_SOURCE] );
	}

	if (number_of_elements[CURRENT_SOURCE])
	{
		fprintf( p_file, "Current Sources: %7d\n", number_of_elements[CURRENT_SOURCE] );
	}

	if (number_of_elements[RESISTANCE])
	{
		fprintf( p_file, "    Resistances: %7d\n", number_of_elements[RESISTANCE] );
	}

	if (number_of_elements[CAPACITOR])
	{
		fprintf( p_file, "     Capacitors: %7d\n", number_of_elements[CAPACITOR] );
	}

	if (number_of_elements[INDUCTOR])
	{
		fprintf( p_file, "      Inductors: %7d\n", number_of_elements[INDUCTOR] );
	}

	if (number_of_elements[DIODE])
	{
		fprintf( p_file, "         Diodes: %7d\n", number_of_elements[DIODE] );
	}

	if (number_of_elements[BJT])
	{
		fprintf( p_file, "BJT Transistors: %7d\n", number_of_elements[BJT] );
	}

	if (number_of_elements[MOS])
	{
		fprintf( p_file, "MOS Transistors: %7d\n", number_of_elements[MOS] );
	}

	fprintf( p_file, "\n\n\nDC Analysis\n" );
	fprintf( p_file, "~~~~~~~~~~~" );

	fprintf( p_file, "\n_____________________________________________________________________________\n" );

	fprintf( p_file, "|   Voltage @ node| %25s: %22.14e   Volt|", circuit_simulation.plot_settings->name, 0.0 );
	for (i = 0; i < number_of_nodes; i++)
	{
		fprintf( p_file, "\n|   Voltage @ node| %25s: %22.14e   Volt|", (circuit_simulation.plot_settings + i + 1)->name,
				fabs( vector_x[i] ) < ZERO ? 0.0 : vector_x[i] );
	}

	fprintf( p_file, "\n_____________________________________________________________________________" );

	if (circuit_simulation.dc_analysis_settings.operating_point)
	{
		for (current1 = head_twoTerminalsElement_list, i = 0; head_twoTerminalsElement_list != NULL && current1 != NULL; current1 = current1->next)
		{
			if (current1->type == CAPACITOR)
			{
				temp[0] = '\0';
				strcat( temp, "C " );
				strcat( temp, current1->string_name );
				fprintf( p_file, "\n|Current @ element| %25s: %22.14e Ampere|", temp, 0.0 );
			}
			else if (current1->type == RESISTANCE)
			{
				temp[0] = '\0';
				strcat( temp, "R " );
				strcat( temp, current1->string_name );
				fprintf( p_file, "\n|Current @ element| %25s: %22.14e Ampere|", temp,
						((current1->positive_terminal == 0 ?
								0 : (fabs( vector_x[current1->positive_terminal - 1] ) < ZERO ? 0.0 : vector_x[current1->positive_terminal - 1]))
								- (current1->negative_terminal == 0 ?
										0 :
										(fabs( vector_x[current1->negative_terminal - 1] ) < ZERO ? 0.0 : vector_x[current1->negative_terminal - 1])))
								/ current1->value );
			}
			else if (current1->type == CURRENT_SOURCE)
			{
				temp[0] = '\0';
				strcat( temp, "I " );
				strcat( temp, current1->string_name );
				fprintf( p_file, "\n|Current @ element| %25s: %22.14e Ampere|", temp, current1->value );
			}
			else if (current1->type == VOLTAGE_SOURCE)
			{
				temp[0] = '\0';
				strcat( temp, "V " );
				strcat( temp, current1->string_name );
				fprintf( p_file, "\n|Current @ element| %25s: %22.14e Ampere|", temp,
						fabs( vector_x[i + number_of_nodes] ) < ZERO ? 0.0 : vector_x[i + number_of_nodes] );
				i++;
			}
			else if (current1->type == INDUCTOR)
			{
				temp[0] = '\0';
				strcat( temp, "L " );
				strcat( temp, current1->string_name );
				fprintf( p_file, "\n|Current @ element| %25s: %22.14e Ampere|", temp,
						fabs( vector_x[i + number_of_nodes] ) < ZERO ? 0.0 : vector_x[i + number_of_nodes] );
				i++;
			}
		}

		fprintf( p_file, "\n_____________________________________________________________________________\n\n" );
	}

	fclose( p_file );
	
	if(circuit_simulation.method == ITERATIVE && circuit_simulation.matrix_type == NONSPD && circuit_simulation.iterative_methods.division_by_zero == 1) {
	    printf("\nWarning, division by zero was encountered during Bi-Conjugate Gradient execution.\n");
	    printf("The results might be inaccurate.\n\n");
	    circuit_simulation.iterative_methods.division_by_zero = 0;
	}

}

void TransientAnalysis(void)
{
	FILE *p_file = NULL, *p_file2 = NULL, *pipe = NULL;
	int dimension, k, i, j, l, length, counter;
	double *vector_e_1 = NULL, *vector_e = NULL, *temp_vec = NULL, t, time_steps;
	char string[3000];
	char temp_string[300];

	dimension = number_of_nodes + group2_elements;

	vector_e = ( double * ) malloc( dimension * sizeof(double) );

	if (vector_e == NULL)
	{
		printf( "Could not allocate matrices.\n" );
		printf( "Terminating.\n" );
		exit( -1 );
	}

	/*if (circuit_simulation.transient_analysis_settings.diff_method == FORWARD_EULER)
	 {

	 temp_vec = ( double * ) malloc( dimension * sizeof(double) );

	 if (temp_vec == NULL)
	 {
	 printf( "Could not allocate matrices.\n" );
	 printf( "Terminating.\n" );
	 exit( -1 );
	 }
	 }*/

	printf( "\n\nTransient Analysis...\n\n" );

	createMnaSystemTransient();

	if (circuit_simulation.matrix_sparsity == SPARSE)
	{
		cs_di_spfree( G2 );
		G2 = temp_mat;  // G = G + C, trapezoidal => G = G + 2/h * C, backward euler => G = G + 1/h * C, forward euler => G = Inverse(C / H) * G
	}
	else
	{
		free( matrix_G );
		matrix_G = temp_matrix;  // G = G + C, trapezoidal => G = G + 2/h * C, backward euler => G = G + 1/h * C, forward euler => G = Inverse(C / H) * G
	}

	if (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL)
	{
		vector_e_1 = ( double * ) malloc( dimension * sizeof(double) );

		if (vector_e_1 == NULL)
		{
			printf( "Could not allocate matrices.\n" );
			printf( "Terminating.\n" );
			exit( -1 );
		}

		memcpy( vector_e_1, vector_b, dimension * sizeof(double) );  // e_1 = b

		if (circuit_simulation.matrix_sparsity == SPARSE)
		{
			cs_di_spfree( C2 );
			C2 = temp_mat2;  // C = G - C <=> G = G - 2/h * C
		}
		else
		{
			free( matrix_C );
			matrix_C = temp_matrix2;  // C = G - C <=> G = G - 2/h * C
		}

	}

	//if (circuit_simulation.transient_analysis_settings.diff_method != FORWARD_EULER)
	//{
	if (circuit_simulation.method == DIRECT)
	{
		if (circuit_simulation.matrix_type == NONSPD)
		{
			LUDecomposition();  // Decompose G
		}
		else
		{
			CholeskyDecomposition();  // Decompose G
		}
	}
	else
	{
		free( inv_preconditioner );
		//printf( "Creating Preconditioner.\n\n" );
		createInversePreconditioner( dimension );
	}
	//}

	//printf( "\n\nPrinting results to Transient_Analysis.txt ...\n\n" );

	if (circuit_simulation.method == DIRECT)
	{
		printf( "Solving with Backward/Forward substitution.\n\n" );
	}
	else
	{
		if (circuit_simulation.matrix_type == NONSPD)
		{
			printf( "Solving with Bi-Conjugate Gradient.\n\n" );
		}
		else
		{
			printf( "Solving with Conjugate Gradient.\n\n" );
		}
	}

	p_file = fopen( "Transient_Analysis.txt", "w" );

	fprintf( p_file, "The circuit, %s, contains:\n\n", circuit_simulation.file_name );

	fprintf( p_file, "          Nodes: %7d\n", number_of_nodes + 1 );

	if (number_of_elements[VOLTAGE_SOURCE])
	{
		fprintf( p_file, "Voltage Sources: %7d\n", number_of_elements[VOLTAGE_SOURCE] );
	}

	if (number_of_elements[CURRENT_SOURCE])
	{
		fprintf( p_file, "Current Sources: %7d\n", number_of_elements[CURRENT_SOURCE] );
	}

	if (number_of_elements[RESISTANCE])
	{
		fprintf( p_file, "    Resistances: %7d\n", number_of_elements[RESISTANCE] );
	}

	if (number_of_elements[CAPACITOR])
	{
		fprintf( p_file, "     Capacitors: %7d\n", number_of_elements[CAPACITOR] );
	}

	if (number_of_elements[INDUCTOR])
	{
		fprintf( p_file, "      Inductors: %7d\n", number_of_elements[INDUCTOR] );
	}

	if (number_of_elements[DIODE])
	{
		fprintf( p_file, "         Diodes: %7d\n", number_of_elements[DIODE] );
	}

	if (number_of_elements[BJT])
	{
		fprintf( p_file, "BJT Transistors: %7d\n", number_of_elements[BJT] );
	}

	if (number_of_elements[MOS])
	{
		fprintf( p_file, "MOS Transistors: %7d\n", number_of_elements[MOS] );
	}

	fprintf( p_file, "\n\n\nTransient Analysis    start: %20.14lf,  stop: %20.14lf,  step: %20.14lf\n", 0.0,
			circuit_simulation.transient_analysis_settings.fin_time, circuit_simulation.transient_analysis_settings.time_step );
	fprintf( p_file, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n" );

#if DEBUG
	printf("\n\nTRANSIENT ANALYSIS    start: %20.14lf,  stop: %20.14lf,  step: %20.14lf\n", 0.0, circuit_simulation.transient_analysis_settings.fin_time, circuit_simulation.transient_analysis_settings.time_step);
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n");
#endif

	fprintf( p_file, "\nt = %22.14lf sec", 0.0 );
	fprintf( p_file, "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" );
	fprintf( p_file, "\n_____________________________________________________________________________" );
	for (l = 0; l < number_of_nodes + 1; l++)
	{
		if ((circuit_simulation.plot_settings + l)->transient_plot)
		{
			if (l == 0)
			{
				fprintf( p_file, "\n|   Voltage @ node| %25s: %22.14e   Volt|", circuit_simulation.plot_settings->name, 0.0 );
			}
			else
			{
				fprintf( p_file, "\n|   Voltage @ node| %25s: %22.14e   Volt|", (circuit_simulation.plot_settings + l)->name,
						fabs( vector_x[l - 1] ) < ZERO ? 0.0 : vector_x[l - 1] );
			}
		}
	}
	fprintf( p_file, "\n_____________________________________________________________________________\n\n" );

	if (circuit_simulation.gnuplot)
	{
		p_file2 = fopen( "transient_plot.txt", "w" );
		
		fprintf( p_file2, "%.14lf ", 0.0 );  //t
		
		for (l = 0; l < number_of_nodes + 1; l++)
		{
			if ((circuit_simulation.plot_settings + l)->transient_plot)
			{
		            if(l == 0) {
	    	                fprintf( p_file2, "%.14lf ", 0.0 ); 
			    }
			    else {
			        fprintf( p_file2, "%.14lf ", vector_x[l - 1] );
			    }				  
			}
		}
		fprintf( p_file2, "\n" );
	}

	time_steps = circuit_simulation.transient_analysis_settings.fin_time / circuit_simulation.transient_analysis_settings.time_step;
	length = ( int ) time_steps + 1;

	for (i = 1, t = circuit_simulation.transient_analysis_settings.time_step; i < length;
			i++, t += circuit_simulation.transient_analysis_settings.time_step)
	{

		memset( vector_e, 0, dimension * sizeof(double) );

		for (j = 0, k = 0; j < number_of_elements[CURRENT_SOURCE] + group2_elements; j++)
		{

			switch (sources[j]->type)
			{
				case CURRENT_SOURCE:
					if (sources[j]->positive_terminal)
					{
						vector_e[sources[j]->positive_terminal - 1] -=
								sources[j]->transient == NULL ? sources[j]->value : sources[j]->transient->vals[i];
					}

					if (sources[j]->negative_terminal)
					{
						vector_e[sources[j]->negative_terminal - 1] +=
								sources[j]->transient == NULL ? sources[j]->value : sources[j]->transient->vals[i];
					}
					break;

				case VOLTAGE_SOURCE:
				case INDUCTOR:
					vector_e[number_of_nodes + k] +=
							sources[j]->type == INDUCTOR ? 0 : (sources[j]->transient == NULL ? sources[j]->value : sources[j]->transient->vals[i]);

					k++;
					break;

			}

		}

		if (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL)
		{
			DrawProgressBar( 30, (i + 1.0) / length );
			Trapezoidal( vector_e, vector_e_1 );
		}
		else if (circuit_simulation.transient_analysis_settings.diff_method == BACKWARD_EULER)
		{
			DrawProgressBar( 30, (i + 1.0) / length );
			BackwardEuler( vector_e );
		}
		/*else if (circuit_simulation.transient_analysis_settings.diff_method == FORWARD_EULER)
		 {
		 printf( "\n\nForward Euler %5.2lf%%...\n\n", (i + 1.0) / length * 100 );
		 ForwardEuler( vector_e, temp_vec );
		 }*/

#if DEBUG
		printf("\nt = %22.14lf sec", t);
		printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
#endif

		//if (circuit_simulation.transient_analysis_settings.diff_method != FORWARD_EULER)
		//{
		if (circuit_simulation.method == DIRECT)
		{
			solve_direct_methods();
		}
		else
		{	
		        if (circuit_simulation.matrix_type == NONSPD)
			{
				BiConjugateGradient(1);
			}
			else
			{
				ConjugateGradient(1);
			}
		}
		//}

		fprintf( p_file, "\nt = %22.14lf sec", t );
		fprintf( p_file, "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" );

		fprintf( p_file, "\n_____________________________________________________________________________" );
		for (l = 0; l < number_of_nodes + 1; l++)
		{
			if ((circuit_simulation.plot_settings + l)->transient_plot)
			{
				if (l == 0)
				{
					fprintf( p_file, "\n|   Voltage @ node| %25s: %22.14e   Volt|", circuit_simulation.plot_settings->name, 0.0 );
				}
				else
				{
					fprintf( p_file, "\n|   Voltage @ node| %25s: %22.14e   Volt|", (circuit_simulation.plot_settings + l)->name,
							fabs( vector_x[l - 1] ) < ZERO ? 0.0 : vector_x[l - 1] );
				}
			}
		}
		fprintf( p_file, "\n_____________________________________________________________________________\n\n" );

		if (circuit_simulation.gnuplot)
		{
			fprintf( p_file2, "%.14lf ", t );
			
			for (l = 0; l < number_of_nodes + 1; l++)
			{
				if ((circuit_simulation.plot_settings + l)->transient_plot)
				{
				    if(l == 0) {
				        fprintf( p_file2, "%.14lf ", 0.0 ); 
				    }
				    else {
				        fprintf( p_file2, "%.14lf ", vector_x[l - 1] );
				    }				  
				}
			}
			fprintf( p_file2, "\n" );
		}
	}
	
	DrawProgressBar( 30, 1.0 );

	if (circuit_simulation.gnuplot)
	{
		fclose( p_file2 );

		for (l = 0, counter = 0; l < number_of_nodes + 1; l++)
		{
			if ((circuit_simulation.plot_settings + l)->transient_plot)
			{
				counter++;
			}
		}

		if (counter != 0)
		{
			pipe = popen( "gnuplot -persist", "w" );

			string[0] = '\0';
			strcat( string, "plot " );
			for (l = 0, j = 0; j < counter && l < number_of_nodes + 1; l++)
			{
				if ((circuit_simulation.plot_settings + l)->transient_plot)
				{
					j++;
					if (j == counter)
					{
						if (circuit_simulation.gnuplot == LINES_POINTS)
						{
							sprintf( temp_string, "\"./transient_plot.txt\" using 1:%d title '%s' with linespoints", j + 1,
									(circuit_simulation.plot_settings + l)->name );
						}
						else
						{
							sprintf( temp_string, "\"./transient_plot.txt\" using 1:%d title '%s' with lines", j + 1,
									(circuit_simulation.plot_settings + l)->name );
						}
						strcat( string, temp_string );
					}
					else
					{
						if (circuit_simulation.gnuplot == LINES_POINTS)
						{
							sprintf( temp_string, "\"./transient_plot.txt\" using 1:%d title '%s' with linespoints, ", j + 1,
									(circuit_simulation.plot_settings + l)->name );
						}
						else
						{
							sprintf( temp_string, "\"./transient_plot.txt\" using 1:%d title '%s' with lines, ", j + 1,
									(circuit_simulation.plot_settings + l)->name );
						}
						strcat( string, temp_string );
					}
				}
			}

			sprintf( temp_string,
					"; set grid; set zeroaxis; set key out horiz top center; set terminal wxt size 800,600; set title \"Transient Analysis\t#circuit %s\"; set autoscale; set xlabel \"Time [sec]\"; set ylabel \"V values [Volt]\"; replot;",
					circuit_simulation.file_name );
			strcat( string, temp_string );
			fprintf( pipe, "%s\n", string );
			fclose( pipe );
		}
	}

	if (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL)
	{
		free( vector_e_1 );
	}

	/*if (circuit_simulation.transient_analysis_settings.diff_method == FORWARD_EULER)
	 {

	 free( temp_vec );

	 }*/

	free( vector_e );

	fclose( p_file );
	
	if(circuit_simulation.method == ITERATIVE && circuit_simulation.matrix_type == NONSPD && circuit_simulation.iterative_methods.division_by_zero == 1) {
	    printf("\nWarning, division by zero was encountered during Bi-Conjugate Gradient execution.\n");
	    printf("The results might be inaccurate.\n\n");
	    circuit_simulation.iterative_methods.division_by_zero = 0;
	}

}

void DCSweep(void)
{
	double k, *old_vec_x = NULL, *old_vec_b = NULL;
	int dimension, j, l, counter;
	FILE *p_file = NULL, *p_file2 = NULL, *pipe = NULL;
	char string[3000];
	char temp_string[300];
	double percent = 0;
	double percent_step;

	dimension = number_of_nodes + group2_elements;

	old_vec_x = ( double * ) malloc( dimension * sizeof(double) );
	old_vec_b = ( double * ) malloc( dimension * sizeof(double) );

	if (old_vec_x == NULL || old_vec_b == NULL)
	{
		printf( "Could not allocate matrices.\n" );
		printf( "Terminating.\n" );
		exit( -1 );
	}

	memcpy( old_vec_x, vector_x, dimension * sizeof(double) );
	memcpy( old_vec_b, vector_b, dimension * sizeof(double) );

	printf( "\n\nDc Sweep...\n\n" );

	//printf( "\n\nPrinting results to DC_Sweep.txt ...\n\n" );

	if (circuit_simulation.method == DIRECT)
	{
		printf( "Solving with Backward/Forward substitution.\n\n" );
	}
	else
	{
		if (circuit_simulation.matrix_type == NONSPD)
		{
			printf( "Solving with Bi-Conjugate Gradient.\n\n" );
		}
		else
		{
			printf( "Solving with Conjugate Gradient.\n\n" );
		}
	}

	p_file = fopen( "DC_Sweep.txt", "w" );

	memset( vector_x, 0, dimension * sizeof(double) );

	circuit_simulation.dc_sweep_settings.dc_sweep_index1 +=
			circuit_simulation.dc_sweep_settings.dc_sweep == SWEEP_VOLTAGE_SOURCE ? number_of_nodes : 0;

	fprintf( p_file, "The circuit, %s, contains:\n\n", circuit_simulation.file_name );

	fprintf( p_file, "          Nodes: %7d\n", number_of_nodes + 1 );

	if (number_of_elements[VOLTAGE_SOURCE])
	{
		fprintf( p_file, "Voltage Sources: %7d\n", number_of_elements[VOLTAGE_SOURCE] );
	}

	if (number_of_elements[CURRENT_SOURCE])
	{
		fprintf( p_file, "Current Sources: %7d\n", number_of_elements[CURRENT_SOURCE] );
	}

	if (number_of_elements[RESISTANCE])
	{
		fprintf( p_file, "    Resistances: %7d\n", number_of_elements[RESISTANCE] );
	}

	if (number_of_elements[CAPACITOR])
	{
		fprintf( p_file, "     Capacitors: %7d\n", number_of_elements[CAPACITOR] );
	}

	if (number_of_elements[INDUCTOR])
	{
		fprintf( p_file, "      Inductors: %7d\n", number_of_elements[INDUCTOR] );
	}

	if (number_of_elements[DIODE])
	{
		fprintf( p_file, "         Diodes: %7d\n", number_of_elements[DIODE] );
	}

	if (number_of_elements[BJT])
	{
		fprintf( p_file, "BJT Transistors: %7d\n", number_of_elements[BJT] );
	}

	if (number_of_elements[MOS])
	{
		fprintf( p_file, "MOS Transistors: %7d\n", number_of_elements[MOS] );
	}

	fprintf( p_file, "\n\n\nDC Sweep @ Source %16s    start: %12lf,  stop: %12lf,  step: %12lf\n", circuit_simulation.dc_sweep_settings.sweep_name,
			circuit_simulation.dc_sweep_settings.dc_sweep_start, circuit_simulation.dc_sweep_settings.dc_sweep_end,
			circuit_simulation.dc_sweep_settings.dc_sweep_step );
	fprintf( p_file, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n\n" );

#if DEBUG
	printf("\n\nDC SWEEP @ Source %16s    start: %12lf,  stop: %12lf,  step: %12lf\n", circuit_simulation.dc_sweep_settings.sweep_name, circuit_simulation.dc_sweep_settings.dc_sweep_start, circuit_simulation.dc_sweep_settings.dc_sweep_end, circuit_simulation.dc_sweep_settings.dc_sweep_step);
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n");
#endif

	if (circuit_simulation.gnuplot)
	{
		p_file2 = fopen( "dc_sweep_plot.txt", "w" );
	}

	percent_step = fabs( circuit_simulation.dc_sweep_settings.dc_sweep_step )
			/ fabs( circuit_simulation.dc_sweep_settings.dc_sweep_start - circuit_simulation.dc_sweep_settings.dc_sweep_end );

	for (k = circuit_simulation.dc_sweep_settings.dc_sweep_start;
			circuit_simulation.dc_sweep_settings.dc_sweep_start < circuit_simulation.dc_sweep_settings.dc_sweep_end ?
					k - circuit_simulation.dc_sweep_settings.dc_sweep_end <= ZERO : k - circuit_simulation.dc_sweep_settings.dc_sweep_end >= -ZERO;
			k += circuit_simulation.dc_sweep_settings.dc_sweep_step)
	{

		if (circuit_simulation.dc_sweep_settings.dc_sweep == SWEEP_VOLTAGE_SOURCE)
		{
			vector_b[circuit_simulation.dc_sweep_settings.dc_sweep_index1] = k;
		}
		else
		{
			if (circuit_simulation.dc_sweep_settings.dc_sweep_index1 != -1)
			{
				vector_b[circuit_simulation.dc_sweep_settings.dc_sweep_index1] = -k;
			}

			if (circuit_simulation.dc_sweep_settings.dc_sweep_index2 != -1)
			{
				vector_b[circuit_simulation.dc_sweep_settings.dc_sweep_index2] = k;
			}
		}

#if DEBUG
		printf("current sweep: %22.14lf\n", k);
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		printf("\nVector b:\n\n");
		for(j = 0; j < dimension; j++)
		{
			printf("%12lf\n", vector_b[j]);
		}

		printf("\n");
#endif

		//printf( "\n\nDC Sweep %5.2lf%%...\n\n", percent * 100 );
		DrawProgressBar( 30, percent );

		if (circuit_simulation.method == DIRECT)
		{
			solve_direct_methods();
		}
		else
		{	
		        if (circuit_simulation.matrix_type == NONSPD)
			{
				BiConjugateGradient(1);
			}
			else
			{
				ConjugateGradient(1);
			}
		}

		fprintf( p_file, "current sweep: %22.14lf\n", k );
		fprintf( p_file, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" );

		fprintf( p_file, "\n_____________________________________________________________________________" );
		for (l = 0; l < number_of_nodes + 1; l++)
		{
			if ((circuit_simulation.plot_settings + l)->dc_sweep_plot)
			{
				if (l == 0)
				{
					fprintf( p_file, "\n|   Voltage @ node| %25s: %22.14e   Volt|", circuit_simulation.plot_settings->name, 0.0 );
				}
				else
				{
					fprintf( p_file, "\n|   Voltage @ node| %25s: %22.14e   Volt|", (circuit_simulation.plot_settings + l)->name,
							fabs( vector_x[l - 1] ) < ZERO ? 0.0 : vector_x[l - 1] );
				}
			}
		}
		fprintf( p_file, "\n_____________________________________________________________________________" );

		fprintf( p_file, "\n\n\n\n" );

		if (circuit_simulation.gnuplot)
		{
			fprintf( p_file2, "%.14lf ", k );

			for (l = 0; l < number_of_nodes + 1; l++)
			{
				if ((circuit_simulation.plot_settings + l)->dc_sweep_plot)
				{
				    if(l == 0) {
				        fprintf( p_file2, "%.14lf ", 0.0 ); 
				    }
				    else {
				        fprintf( p_file2, "%.14lf ", vector_x[l - 1] );
				    }				  
				}
			}
			fprintf( p_file2, "\n" );
		}

		percent += percent_step;

	}
	
	DrawProgressBar( 30, 1.0 );

	if (circuit_simulation.gnuplot)
	{
		fclose( p_file2 );

		for (l = 0, counter = 0; l < number_of_nodes + 1; l++)
		{
			if ((circuit_simulation.plot_settings + l)->dc_sweep_plot)
			{
				counter++;
			}
		}

		if (counter != 0)
		{
			pipe = popen( "gnuplot -persist", "w" );

			string[0] = '\0';
			strcat( string, "plot " );
			for (l = 0, j = 0; j < counter && l < number_of_nodes + 1; l++)
			{
				if ((circuit_simulation.plot_settings + l)->dc_sweep_plot)
				{
					j++;
					if (j == counter)
					{
						if (circuit_simulation.gnuplot == LINES_POINTS)
						{
							sprintf( temp_string, "\"./dc_sweep_plot.txt\" using 1:%d title '%s' with linespoints", j + 1,
									(circuit_simulation.plot_settings + l)->name );
						}
						else
						{
							sprintf( temp_string, "\"./dc_sweep_plot.txt\" using 1:%d title '%s' with lines", j + 1,
									(circuit_simulation.plot_settings + l)->name );
						}
						strcat( string, temp_string );
					}
					else
					{
						if (circuit_simulation.gnuplot == LINES_POINTS)
						{
							sprintf( temp_string, "\"./dc_sweep_plot.txt\" using 1:%d title '%s' with linespoints, ", j + 1,
									(circuit_simulation.plot_settings + l)->name );
						}
						else
						{
							sprintf( temp_string, "\"./dc_sweep_plot.txt\" using 1:%d title '%s' with lines, ", j + 1,
									(circuit_simulation.plot_settings + l)->name );
						}
						strcat( string, temp_string );
					}
				}
			}

			if (circuit_simulation.dc_sweep_settings.sweep_name[0] == 'V')
			{
				sprintf( temp_string,
						"; set grid; set zeroaxis; set key out horiz top center; set terminal wxt size 800,600; set title \"DC Sweep @ Source %s\t#circuit %s\"; set autoscale; set xlabel \"Sweep Values [Volt]\"; set ylabel \"V values [Volt]\";  replot;",
						circuit_simulation.dc_sweep_settings.sweep_name, circuit_simulation.file_name );
			}
			else
			{
				sprintf( temp_string,
						"; set grid; set zeroaxis; set key out horiz top center; set terminal wxt size 800,600; set title \"DC Sweep @ Source %s\t#circuit %s\"; set autoscale; set xlabel \"Sweep Values [Ampere]\"; set ylabel \"V values [Volt]\";  replot;",
						circuit_simulation.dc_sweep_settings.sweep_name, circuit_simulation.file_name );
			}

			strcat( string, temp_string );
			fprintf( pipe, "%s\n", string );
			fclose( pipe );
		}
	}

	memcpy( vector_x, old_vec_x, dimension * sizeof(double) );
	memcpy( vector_b, old_vec_b, dimension * sizeof(double) );

	free( old_vec_x );
	free( old_vec_b );

	fclose( p_file );
	
	if(circuit_simulation.method == ITERATIVE && circuit_simulation.matrix_type == NONSPD && circuit_simulation.iterative_methods.division_by_zero == 1) {
	    printf("\nWarning, division by zero was encountered during Bi-Conjugate Gradient execution.\n");
	    printf("The results might be inaccurate.\n\n");
	    circuit_simulation.iterative_methods.division_by_zero = 0;
	}

}

void DrawProgressBar(int len, double percent)
{
	int i;
//cout << "\x1B[2K";  // Erase the entire current line.
//cout << "\x1B[0E";  // Move to the beginning of the current line.
	printf( "\r" );

	char progress[len + 1];
	for (i = 0; i < len; ++i)
	{
		if (i < ( int ) (len * percent))
		{
			progress[i] = '=';
		}
		else
		{
			progress[i] = ' ';
		}
	}
	progress[len] = '\0';

	printf( "[%s] %d%%", progress, (( unsigned int ) (100 * percent)) );
	fflush( stdout );
}
