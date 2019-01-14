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
	unsigned int dimension; 
	int l, j, counter;;
	char string[3000];
	char temp_string[1000];
	char string2[3000];
	char temp_string2[1000];
	double f, end_freq_decade = 0;
	double step;
	double arg, mag;
	double pi_180 = M_PI / 180;
	int points_per_decade, total_decades;
        long int total_points, i;
	double decade = 0, temp_decade1, temp_decade2;	
	int ac_analysis_no;
	char file_name[40];
	int run = 0;
	
	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;
	
	vector_b_complex = ( double complex * ) calloc( dimension, sizeof(double complex) );
	vector_x_complex = ( double complex * ) calloc( dimension, sizeof(double complex) );

	if (vector_b_complex == NULL || vector_x_complex == NULL)
	{
		printf( "Could not allocate matrices.\n" );
		printf( "Terminating.\n" );
		exit( -1 );
	}
	
	for(ac_analysis_no = 0; ac_analysis_no < circuit_simulation.number_of_ac_analysis; ac_analysis_no++) {

	      printf( "\n\nAC Analysis...\n\n" );
	      
	      if((circuit_simulation.ac_analysis_settings + ac_analysis_no)->scale == LIN) {
		  step = ((circuit_simulation.ac_analysis_settings + ac_analysis_no)->end_freq - (circuit_simulation.ac_analysis_settings + ac_analysis_no)->start_freq) / (circuit_simulation.ac_analysis_settings + ac_analysis_no)->points;
	      
		  total_points = (circuit_simulation.ac_analysis_settings + ac_analysis_no)->points + 1;
	      }
	      else {	    
		  decade = log10((circuit_simulation.ac_analysis_settings + ac_analysis_no)->start_freq);
		  end_freq_decade = log10((circuit_simulation.ac_analysis_settings + ac_analysis_no)->end_freq);
		  
		  if(decade - (int)decade != 0) {
		      temp_decade1 = (int)(decade - 1); //previous decade
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
		      step = (calculate_next_decade(decade) - decade) / (circuit_simulation.ac_analysis_settings + ac_analysis_no)->points;
		  }
		  else { // same decade	        
		      step = (end_freq_decade - decade) / (circuit_simulation.ac_analysis_settings + ac_analysis_no)->points;
		  }
			  
		  total_points = total_decades * (circuit_simulation.ac_analysis_settings + ac_analysis_no)->points + 1;
	      }
	      
	      //printf( "\n\nPrinting results to AC_Analysis.txt ...\n\n" );
	      
	      sprintf(file_name, "AC_Analysis%d.txt", ac_analysis_no + 1);
	      p_file = fopen( file_name, "w" );

	      fprintf( p_file, "The circuit, %s, contains:\n\n", circuit_simulation.file_name );

	      fprintf( p_file, "          Nodes: %7d\n", circuit_simulation.number_of_nodes + 1 );

	      if (circuit_simulation.number_of_elements[VOLTAGE_SOURCE])
	      {
		      fprintf( p_file, "Voltage Sources: %7d\n", circuit_simulation.number_of_elements[VOLTAGE_SOURCE] );
	      }

	      if (circuit_simulation.number_of_elements[CURRENT_SOURCE])
	      {
		      fprintf( p_file, "Current Sources: %7d\n", circuit_simulation.number_of_elements[CURRENT_SOURCE] );
	      }

	      if (circuit_simulation.number_of_elements[RESISTANCE])
	      {
		      fprintf( p_file, "    Resistances: %7d\n", circuit_simulation.number_of_elements[RESISTANCE] );
	      }

	      if (circuit_simulation.number_of_elements[CAPACITOR])
	      {
		      fprintf( p_file, "     Capacitors: %7d\n", circuit_simulation.number_of_elements[CAPACITOR] );
	      }

	      if (circuit_simulation.number_of_elements[INDUCTOR])
	      {
		      fprintf( p_file, "      Inductors: %7d\n", circuit_simulation.number_of_elements[INDUCTOR] );
	      }

	      if (circuit_simulation.number_of_elements[DIODE])
	      {
		      fprintf( p_file, "         Diodes: %7d\n", circuit_simulation.number_of_elements[DIODE] );
	      }

	      if (circuit_simulation.number_of_elements[BJT])
	      {
		      fprintf( p_file, "BJT Transistors: %7d\n", circuit_simulation.number_of_elements[BJT] );
	      }

	      if (circuit_simulation.number_of_elements[MOS])
	      {
		      fprintf( p_file, "MOS Transistors: %7d\n", circuit_simulation.number_of_elements[MOS] );
	      }
	      
	      if((circuit_simulation.ac_analysis_settings + ac_analysis_no)->scale == LOG) {
		  fprintf( p_file, "\n\n\nAC Analysis    start: %20.10lf,  stop: %20.10lf,  step: log10\n", (circuit_simulation.ac_analysis_settings + ac_analysis_no)->start_freq,
				    (circuit_simulation.ac_analysis_settings + ac_analysis_no)->end_freq);
		  fprintf( p_file, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n" );

	    #if DEBUG
		    printf("\n\nAC ANALYSIS    start: %20.10lf,  stop: %20.10lf,  step: log10\n", (circuit_simulation.ac_analysis_settings + ac_analysis_no)->start_freq,
				    (circuit_simulation.ac_analysis_settings + ac_analysis_no)->end_freq);
		    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n");
	    #endif
	      }
	      else {
		    fprintf( p_file, "\n\n\nAC Analysis    start: %20.10lf,  stop: %20.10lf,  step: %20.10lf\n", (circuit_simulation.ac_analysis_settings + ac_analysis_no)->start_freq,
				    (circuit_simulation.ac_analysis_settings + ac_analysis_no)->end_freq, step );
		    fprintf( p_file, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n" );

	    #if DEBUG
		    printf("\n\nAC ANALYSIS    start: %20.10lf,  stop: %20.10lf,  step: %20.10lf\n", (circuit_simulation.ac_analysis_settings + ac_analysis_no)->start_freq,
				    (circuit_simulation.ac_analysis_settings + ac_analysis_no)->end_freq, step);
		    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n");
	    #endif
	      }
	      
	      if (circuit_simulation.gnuplot)
	      {
		    sprintf(file_name, "ac_plot%d.txt", ac_analysis_no + 1);  
		    p_file2 = fopen( file_name, "w" );
	      }
		      
	      
	      for(i = 0, points_per_decade = 0; i < total_points; i++, points_per_decade++, run++) {	  

		  if((circuit_simulation.ac_analysis_settings + ac_analysis_no)->scale == LIN) {
		      f = (circuit_simulation.ac_analysis_settings + ac_analysis_no)->start_freq + i * step;
		  }
		  else {
		      f = pow(10, decade + points_per_decade * step);
		  }

		  createMnaSystemAC(f, run, i);
		  
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

		      if(circuit_simulation.ac_plot_scale == DB) {
			  fprintf( p_file, "\n_____________________________________________________Magnitude[dB]______________Phase_______________" );
		      }
		      else {
			  fprintf( p_file, "\n______________________________________________________Magnitude_________________Phase_______________" );
		      }
		      
		      for (l = 0; l < circuit_simulation.number_of_nodes + 1; l++)
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
					    mag = circuit_simulation.ac_plot_scale == DB ? 20 * log10(mag) : mag;
					    
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
			      
			      for (l = 0; l < circuit_simulation.number_of_nodes + 1; l++)
			      {
				      if ((circuit_simulation.plot_settings + l)->ac_plot)
				      {
					  if(l == 0) {
					      fprintf( p_file2, "%.14lf ", 0.0 ); 
					      fprintf( p_file2, "%.14lf ", 0.0 );
					  }
					  else {
					      mag = cabs(vector_x_complex[l - 1]);   
					      mag = circuit_simulation.ac_plot_scale == DB ? 20 * log10(mag) : mag;
						    
					      fprintf( p_file2, "%.14lf ", mag );
					      
					      arg = carg(vector_x_complex[l - 1]) / pi_180;
					      //arg = arg < 0 ? arg + 360 : arg;
					      
					      fprintf( p_file2, "%.14lf ", arg );
					  }				  
				      }
			      }
			      fprintf( p_file2, "\n" );
		      }
		  
		  freeAllocationsAC();
		  		  
		  if((circuit_simulation.ac_analysis_settings + ac_analysis_no)->scale == LOG) {
		      if(points_per_decade == (circuit_simulation.ac_analysis_settings + ac_analysis_no)->points) {
			    
			    decade = calculate_next_decade(decade);
				
			    if(end_freq_decade > (int)decade + 1) {
				step = (((int)decade) + 1 - decade) / (circuit_simulation.ac_analysis_settings + ac_analysis_no)->points;
			    }
			    else {
				step = (end_freq_decade - decade) / (circuit_simulation.ac_analysis_settings + ac_analysis_no)->points;	      
			    }
			    
			    points_per_decade = 0;
		      }
		  }
	      }
	      
	      DrawProgressBar( 30, 1.0 );
	      
	      if (circuit_simulation.gnuplot)
	      {
		      fclose( p_file2 );

		      for (l = 0, counter = 0; l < circuit_simulation.number_of_nodes + 1; l++)
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
			      strcat( string, "set terminal wxt size 800,600; plot " );
			      
			      string2[0] = '\0';
			      strcat( string2, "set terminal wxt size 800,600; plot " );
			      for (l = 0, j = 0; j < counter && l < circuit_simulation.number_of_nodes + 1; l++)
			      {
				      if ((circuit_simulation.plot_settings + l)->ac_plot)
				      {
					      j++;
					      if (j == counter)
					      {
						      if (circuit_simulation.gnuplot == LINES_POINTS)
						      {
							      sprintf( temp_string, "\"./ac_plot%d.txt\" using 1:%d title '%s' with linespoints", ac_analysis_no + 1, 2 * j,
									      (circuit_simulation.plot_settings + l)->name );
							      sprintf( temp_string2, "\"./ac_plot%d.txt\" using 1:%d title '%s' with linespoints", ac_analysis_no + 1, 2 * j + 1,
									      (circuit_simulation.plot_settings + l)->name );
						      }
						      else
						      {
							      sprintf( temp_string, "\"./ac_plot%d.txt\" using 1:%d title '%s' with lines", ac_analysis_no + 1, 2 * j,
									      (circuit_simulation.plot_settings + l)->name );
							      sprintf( temp_string2, "\"./ac_plot%d.txt\" using 1:%d title '%s' with lines", ac_analysis_no + 1, 2 * j + 1,
									      (circuit_simulation.plot_settings + l)->name );
						      }
						      strcat( string, temp_string );
						      strcat( string2, temp_string2 );
					      }
					      else
					      {
						      if (circuit_simulation.gnuplot == LINES_POINTS)
						      {
							      sprintf( temp_string, "\"./ac_plot%d.txt\" using 1:%d title '%s' with linespoints, ", ac_analysis_no + 1, 2 * j,
									      (circuit_simulation.plot_settings + l)->name );
							      sprintf( temp_string2, "\"./ac_plot%d.txt\" using 1:%d title '%s' with linespoints, ", ac_analysis_no + 1, 2 * j + 1,
									      (circuit_simulation.plot_settings + l)->name );
						      }
						      else
						      {
							      sprintf( temp_string, "\"./ac_plot%d.txt\" using 1:%d title '%s' with lines, ", ac_analysis_no + 1, 2 * j,
									      (circuit_simulation.plot_settings + l)->name );
							      sprintf( temp_string2, "\"./ac_plot%d.txt\" using 1:%d title '%s' with lines, ", ac_analysis_no + 1, 2 * j + 1,
									      (circuit_simulation.plot_settings + l)->name );
						      }
						      strcat( string, temp_string );
						      strcat( string2, temp_string2 );
					      }
				      }
			      }
			      
			      if((circuit_simulation.ac_analysis_settings + ac_analysis_no)->scale == LOG) {
				  sprintf( temp_string, "; set logscale x");
				  sprintf( temp_string2, "; set logscale x");
				  
				  strcat( string, temp_string );
				  strcat( string2, temp_string2 );
			      }

			      if(circuit_simulation.ac_plot_scale == DB) {
				  sprintf( temp_string,
					      "; set grid; set zeroaxis; set key out horiz top center; set title \"AC Analysis\t#circuit %s\"; set autoscale; set xlabel \"Frequency [Hz]\"; set ylabel \"V Magnitude [dB]\"; replot;",
					      circuit_simulation.file_name );
			      }
			      else {
				  sprintf( temp_string,
					      "; set grid; set zeroaxis; set key out horiz top center; set title \"AC Analysis\t#circuit %s\"; set autoscale; set xlabel \"Frequency [Hz]\"; set ylabel \"V Magnitude [Volt]\"; replot;",
					      circuit_simulation.file_name );
			      }
			      
			      strcat( string, temp_string );
			      
			      sprintf( temp_string2,
					      "; set grid; set zeroaxis; set key out horiz top center; set title \"AC Analysis\t#circuit %s\"; set autoscale; set xlabel \"Frequency [Hz]\"; set ylabel \"V Phase [Degrees]\"; replot;",
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
	      
	      printf( "\n\n");
	
	}
	
	free( vector_x_complex );

	free( vector_b_complex );
	   
	      
}

void DCAnalysis(void)
{
	FILE *p_file = NULL;
	int i; 
	unsigned int dimension;
	twoTerminalsElement *current1 = NULL;
	char temp[NAME_SIZE];

	printf( "\n\nDC Analysis...\n\n" );

	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;

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

	fprintf( p_file, "          Nodes: %7d\n", circuit_simulation.number_of_nodes + 1 );

	if (circuit_simulation.number_of_elements[VOLTAGE_SOURCE])
	{
		fprintf( p_file, "Voltage Sources: %7d\n", circuit_simulation.number_of_elements[VOLTAGE_SOURCE] );
	}

	if (circuit_simulation.number_of_elements[CURRENT_SOURCE])
	{
		fprintf( p_file, "Current Sources: %7d\n", circuit_simulation.number_of_elements[CURRENT_SOURCE] );
	}

	if (circuit_simulation.number_of_elements[RESISTANCE])
	{
		fprintf( p_file, "    Resistances: %7d\n", circuit_simulation.number_of_elements[RESISTANCE] );
	}

	if (circuit_simulation.number_of_elements[CAPACITOR])
	{
		fprintf( p_file, "     Capacitors: %7d\n", circuit_simulation.number_of_elements[CAPACITOR] );
	}

	if (circuit_simulation.number_of_elements[INDUCTOR])
	{
		fprintf( p_file, "      Inductors: %7d\n", circuit_simulation.number_of_elements[INDUCTOR] );
	}

	if (circuit_simulation.number_of_elements[DIODE])
	{
		fprintf( p_file, "         Diodes: %7d\n", circuit_simulation.number_of_elements[DIODE] );
	}

	if (circuit_simulation.number_of_elements[BJT])
	{
		fprintf( p_file, "BJT Transistors: %7d\n", circuit_simulation.number_of_elements[BJT] );
	}

	if (circuit_simulation.number_of_elements[MOS])
	{
		fprintf( p_file, "MOS Transistors: %7d\n", circuit_simulation.number_of_elements[MOS] );
	}

	fprintf( p_file, "\n\n\nDC Analysis\n" );
	fprintf( p_file, "~~~~~~~~~~~" );

	fprintf( p_file, "\n_____________________________________________________________________________\n" );

	fprintf( p_file, "|   Voltage @ node| %25s: %22.14e   Volt|", circuit_simulation.plot_settings->name, 0.0 );
	for (i = 0; i < circuit_simulation.number_of_nodes; i++)
	{
		fprintf( p_file, "\n|   Voltage @ node| %25s: %22.14e   Volt|", (circuit_simulation.plot_settings + i + 1)->name,
				fabs( vector_x[i] ) < ZERO ? 0.0 : vector_x[i] );
	}

	fprintf( p_file, "\n_____________________________________________________________________________" );

	if (circuit_simulation.dc_analysis_settings.operating_point)
	{
		for (current1 = head_twoTerminalsElement_list, i = 0; head_twoTerminalsElement_list != NULL && current1 != NULL; current1 = current1->next)
		{
			switch (current1->type)
			{
				case CAPACITOR:
				      temp[0] = '\0';
				      strcat( temp, "C " );
				      strcat( temp, current1->string_name );
				      fprintf( p_file, "\n|Current @ element| %25s: %22.14e Ampere|", temp, 0.0 );
				      break;
				case RESISTANCE:			
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
				      break;
				case CURRENT_SOURCE:		
				      temp[0] = '\0';
				      strcat( temp, "I " );
				      strcat( temp, current1->string_name );
				      fprintf( p_file, "\n|Current @ element| %25s: %22.14e Ampere|", temp, current1->value );
				      break;
				case VOLTAGE_SOURCE:		
				      temp[0] = '\0';
				      strcat( temp, "V " );
				      strcat( temp, current1->string_name );
				      fprintf( p_file, "\n|Current @ element| %25s: %22.14e Ampere|", temp,
						      fabs( vector_x[i + circuit_simulation.number_of_nodes] ) < ZERO ? 0.0 : vector_x[i + circuit_simulation.number_of_nodes] );
				      i++;
				      break;
				case INDUCTOR:
				      temp[0] = '\0';
				      strcat( temp, "L " );
				      strcat( temp, current1->string_name );
				      fprintf( p_file, "\n|Current @ element| %25s: %22.14e Ampere|", temp,
						      fabs( vector_x[i + circuit_simulation.number_of_nodes] ) < ZERO ? 0.0 : vector_x[i + circuit_simulation.number_of_nodes] );
				      i++;
				      break;
				default:
				      break;
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
	unsigned int dimension;
	int k, i, j, l, length, counter;
	double *vector_e_1 = NULL, *vector_e = NULL, t, time_steps;
	double *old_vec_x, *old_vec_b;
	char string[3000];
	char temp_string[1000];
	int transient_analysis_no;
	char file_name[40];

	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;
	
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
	
	for(transient_analysis_no = 0; transient_analysis_no < circuit_simulation.number_of_transient_analysis; transient_analysis_no++) {

		printf( "\n\nTransient Analysis...\n\n" );

		createMnaSystemTransient(transient_analysis_no, (circuit_simulation.transient_analysis_settings + transient_analysis_no)->time_step, (circuit_simulation.transient_analysis_settings + transient_analysis_no)->fin_time);

		vector_e = ( double * ) malloc( dimension * sizeof(double) );

		if (vector_e == NULL)
		{
			printf( "Could not allocate matrices.\n" );
			printf( "Terminating.\n" );
			exit( -1 );
		}
		
		if (circuit_simulation.diff_method == TRAPEZOIDAL)
		{
			vector_e_1 = ( double * ) malloc( dimension * sizeof(double) );

			if (vector_e_1 == NULL)
			{
				printf( "Could not allocate matrices.\n" );
				printf( "Terminating.\n" );
				exit( -1 );
			}

			memcpy( vector_e_1, vector_b, dimension * sizeof(double) );  // e_1 = b

		}

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

		sprintf(file_name, "Transient_Analysis%d.txt", transient_analysis_no + 1); 
		p_file = fopen( file_name, "w" );

		fprintf( p_file, "The circuit, %s, contains:\n\n", circuit_simulation.file_name );

		fprintf( p_file, "          Nodes: %7d\n", circuit_simulation.number_of_nodes + 1 );

		if (circuit_simulation.number_of_elements[VOLTAGE_SOURCE])
		{
			fprintf( p_file, "Voltage Sources: %7d\n", circuit_simulation.number_of_elements[VOLTAGE_SOURCE] );
		}

		if (circuit_simulation.number_of_elements[CURRENT_SOURCE])
		{
			fprintf( p_file, "Current Sources: %7d\n", circuit_simulation.number_of_elements[CURRENT_SOURCE] );
		}

		if (circuit_simulation.number_of_elements[RESISTANCE])
		{
			fprintf( p_file, "    Resistances: %7d\n", circuit_simulation.number_of_elements[RESISTANCE] );
		}

		if (circuit_simulation.number_of_elements[CAPACITOR])
		{
			fprintf( p_file, "     Capacitors: %7d\n", circuit_simulation.number_of_elements[CAPACITOR] );
		}

		if (circuit_simulation.number_of_elements[INDUCTOR])
		{
			fprintf( p_file, "      Inductors: %7d\n", circuit_simulation.number_of_elements[INDUCTOR] );
		}

		if (circuit_simulation.number_of_elements[DIODE])
		{
			fprintf( p_file, "         Diodes: %7d\n", circuit_simulation.number_of_elements[DIODE] );
		}

		if (circuit_simulation.number_of_elements[BJT])
		{
			fprintf( p_file, "BJT Transistors: %7d\n", circuit_simulation.number_of_elements[BJT] );
		}

		if (circuit_simulation.number_of_elements[MOS])
		{
			fprintf( p_file, "MOS Transistors: %7d\n", circuit_simulation.number_of_elements[MOS] );
		}

		fprintf( p_file, "\n\n\nTransient Analysis    start: %20.14lf,  stop: %20.14lf,  step: %20.14lf\n", 0.0,
				(circuit_simulation.transient_analysis_settings + transient_analysis_no)->fin_time, (circuit_simulation.transient_analysis_settings + transient_analysis_no)->time_step );
		fprintf( p_file, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n" );

	#if DEBUG
		printf("\n\nTRANSIENT ANALYSIS    start: %20.14lf,  stop: %20.14lf,  step: %20.14lf\n", 0.0, (circuit_simulation.transient_analysis_settings + transient_analysis_no)->fin_time, (circuit_simulation.transient_analysis_settings + transient_analysis_no)->time_step);
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n");
	#endif

		fprintf( p_file, "\nt = %22.14lf sec", 0.0 );
		fprintf( p_file, "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" );
		fprintf( p_file, "\n_____________________________________________________________________________" );
		for (l = 0; l < circuit_simulation.number_of_nodes + 1; l++)
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
			sprintf(file_name, "transient_plot%d.txt", transient_analysis_no + 1);  
			p_file2 = fopen( file_name, "w" );
			
			fprintf( p_file2, "%.14lf ", 0.0 );  //t
			
			for (l = 0; l < circuit_simulation.number_of_nodes + 1; l++)
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

		time_steps = (circuit_simulation.transient_analysis_settings + transient_analysis_no)->fin_time / (circuit_simulation.transient_analysis_settings + transient_analysis_no)->time_step;
		length = ( int ) time_steps + 1;

		for (i = 1, t = (circuit_simulation.transient_analysis_settings + transient_analysis_no)->time_step; i < length;
				i++, t += (circuit_simulation.transient_analysis_settings + transient_analysis_no)->time_step)
		{

			memset( vector_e, 0, dimension * sizeof(double) );

			for (j = 0, k = 0; j < circuit_simulation.number_of_elements[CURRENT_SOURCE] + circuit_simulation.group2_elements; j++)
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
						vector_e[circuit_simulation.number_of_nodes + k] +=
								sources[j]->type == INDUCTOR ? 0 : (sources[j]->transient == NULL ? sources[j]->value : sources[j]->transient->vals[i]);

						k++;
						break;
					default:
						break;

				}

			}

			DrawProgressBar( 30, (i + 1.0) / length );
			
			if (circuit_simulation.diff_method == TRAPEZOIDAL)
			{
				Trapezoidal( vector_e, vector_e_1 );
			}
			else
			{			
				BackwardEuler( vector_e );
			}


	#if DEBUG
			printf("\nt = %22.14lf sec", t);
			printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	#endif

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

			fprintf( p_file, "\nt = %22.14lf sec", t );
			fprintf( p_file, "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" );

			fprintf( p_file, "\n_____________________________________________________________________________" );
			for (l = 0; l < circuit_simulation.number_of_nodes + 1; l++)
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
				
				for (l = 0; l < circuit_simulation.number_of_nodes + 1; l++)
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

			for (l = 0, counter = 0; l < circuit_simulation.number_of_nodes + 1; l++)
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
				strcat( string, "set terminal wxt size 800,600; plot " );
				for (l = 0, j = 0; j < counter && l < circuit_simulation.number_of_nodes + 1; l++)
				{
					if ((circuit_simulation.plot_settings + l)->transient_plot)
					{
						j++;
						if (j == counter)
						{
							if (circuit_simulation.gnuplot == LINES_POINTS)
							{
								sprintf( temp_string, "\"./transient_plot%d.txt\" using 1:%d title '%s' with linespoints", transient_analysis_no + 1, j + 1,
										(circuit_simulation.plot_settings + l)->name );
							}
							else
							{
								sprintf( temp_string, "\"./transient_plot%d.txt\" using 1:%d title '%s' with lines", transient_analysis_no + 1, j + 1,
										(circuit_simulation.plot_settings + l)->name );
							}
							strcat( string, temp_string );
						}
						else
						{
							if (circuit_simulation.gnuplot == LINES_POINTS)
							{
								sprintf( temp_string, "\"./transient_plot%d.txt\" using 1:%d title '%s' with linespoints, ", transient_analysis_no + 1, j + 1,
										(circuit_simulation.plot_settings + l)->name );
							}
							else
							{
								sprintf( temp_string, "\"./transient_plot%d.txt\" using 1:%d title '%s' with lines, ", transient_analysis_no + 1, j + 1,
										(circuit_simulation.plot_settings + l)->name );
							}
							strcat( string, temp_string );
						}
					}
				}

				sprintf( temp_string,
						"; set grid; set zeroaxis; set key out horiz top center; set title \"Transient Analysis\t#circuit %s\"; set autoscale; set xlabel \"Time [sec]\"; set ylabel \"V values [Volt]\"; replot;",
						circuit_simulation.file_name );
				strcat( string, temp_string );
				printf("%s\n", string);
				fprintf( pipe, "%s\n", string );
				fclose( pipe );
			}
		}

		if (circuit_simulation.diff_method == TRAPEZOIDAL)
		{
			free( vector_e_1 );
		}

		free( vector_e );
		
		freeAllocationsTransient();
		
		memcpy( vector_x, old_vec_x, dimension * sizeof(double) );
		memcpy( vector_b, old_vec_b, dimension * sizeof(double) );

		fclose( p_file );
		
		if(circuit_simulation.method == ITERATIVE && circuit_simulation.matrix_type == NONSPD && circuit_simulation.iterative_methods.division_by_zero == 1) {
		    printf("\nWarning, division by zero was encountered during Bi-Conjugate Gradient execution.\n");
		    printf("The results might be inaccurate.\n\n");
		    circuit_simulation.iterative_methods.division_by_zero = 0;
		}
		
		printf( "\n\n");
	}
	
	
	free( old_vec_x );
	free( old_vec_b );
	free(vector_x);
	free(vector_b);
	free( inv_preconditioner );

}

void DCSweep(void)
{
	double k, *old_vec_x = NULL, *old_vec_b = NULL;
	unsigned int dimension; 
	int j, l, counter;
	FILE *p_file = NULL, *p_file2 = NULL, *pipe = NULL;
	char string[3000];
	char temp_string[1000];
	double percent = 0;
	double percent_step;
	int sweep_no;
	char file_name[40];

	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;
	
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

	for(sweep_no = 0; sweep_no < circuit_simulation.number_of_sweeps; sweep_no++) {
		
	      printf( "\n\nDc Sweep...\n\n\n\n" );

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

	      sprintf(file_name, "DC_Sweep%d.txt", sweep_no + 1);
	      p_file = fopen( file_name, "w" );


	      (circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_index1 +=
			      (circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep == SWEEP_VOLTAGE_SOURCE ? circuit_simulation.number_of_nodes : 0;

	      fprintf( p_file, "The circuit, %s, contains:\n\n", circuit_simulation.file_name );

	      fprintf( p_file, "          Nodes: %7d\n", circuit_simulation.number_of_nodes + 1 );

	      if (circuit_simulation.number_of_elements[VOLTAGE_SOURCE])
	      {
		      fprintf( p_file, "Voltage Sources: %7d\n", circuit_simulation.number_of_elements[VOLTAGE_SOURCE] );
	      }

	      if (circuit_simulation.number_of_elements[CURRENT_SOURCE])
	      {
		      fprintf( p_file, "Current Sources: %7d\n", circuit_simulation.number_of_elements[CURRENT_SOURCE] );
	      }

	      if (circuit_simulation.number_of_elements[RESISTANCE])
	      {
		      fprintf( p_file, "    Resistances: %7d\n", circuit_simulation.number_of_elements[RESISTANCE] );
	      }

	      if (circuit_simulation.number_of_elements[CAPACITOR])
	      {
		      fprintf( p_file, "     Capacitors: %7d\n", circuit_simulation.number_of_elements[CAPACITOR] );
	      }

	      if (circuit_simulation.number_of_elements[INDUCTOR])
	      {
		      fprintf( p_file, "      Inductors: %7d\n", circuit_simulation.number_of_elements[INDUCTOR] );
	      }

	      if (circuit_simulation.number_of_elements[DIODE])
	      {
		      fprintf( p_file, "         Diodes: %7d\n", circuit_simulation.number_of_elements[DIODE] );
	      }

	      if (circuit_simulation.number_of_elements[BJT])
	      {
		      fprintf( p_file, "BJT Transistors: %7d\n", circuit_simulation.number_of_elements[BJT] );
	      }

	      if (circuit_simulation.number_of_elements[MOS])
	      {
		      fprintf( p_file, "MOS Transistors: %7d\n", circuit_simulation.number_of_elements[MOS] );
	      }

	      fprintf( p_file, "\n\n\nDC Sweep @ Source %16s    start: %12lf,  stop: %12lf,  step: %12lf\n", (circuit_simulation.dc_sweep_settings + sweep_no)->sweep_name,
			      (circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_start, (circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_end,
			      (circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_step );
	      fprintf( p_file, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n\n" );

      #if DEBUG
	      printf("\n\nDC SWEEP @ Source %16s    start: %12lf,  stop: %12lf,  step: %12lf\n", (circuit_simulation.dc_sweep_settings + sweep_no)->sweep_name, (circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_start, (circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_end, (circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_step);
	      printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n");
      #endif

	      if (circuit_simulation.gnuplot)
	      {
		    sprintf(file_name, "dc_sweep_plot%d.txt", sweep_no + 1);  
		    p_file2 = fopen( file_name, "w" );
	      }

	      percent_step = fabs( (circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_step )
			      / fabs( (circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_start - (circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_end );

	      for (k = (circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_start;
			      (circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_start < (circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_end ?
					      k - (circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_end <= ZERO : k - (circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_end >= -ZERO;
			      k += (circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_step)
	      {

		      if ((circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep == SWEEP_VOLTAGE_SOURCE)
		      {
			      vector_b[(circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_index1] = k;
		      }
		      else
		      {
			      if ((circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_index1 != -1)
			      {
				      vector_b[(circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_index1] = -k;
			      }

			      if ((circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_index2 != -1)
			      {
				      vector_b[(circuit_simulation.dc_sweep_settings + sweep_no)->dc_sweep_index2] = k;
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
		      for (l = 0; l < circuit_simulation.number_of_nodes + 1; l++)
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

			      for (l = 0; l < circuit_simulation.number_of_nodes + 1; l++)
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

		      for (l = 0, counter = 0; l < circuit_simulation.number_of_nodes + 1; l++)
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
			      strcat( string, "set terminal wxt size 800,600; plot " );
			      for (l = 0, j = 0; j < counter && l < circuit_simulation.number_of_nodes + 1; l++)
			      {
				      if ((circuit_simulation.plot_settings + l)->dc_sweep_plot)
				      {
					      j++;
					      if (j == counter)
					      {
						      if (circuit_simulation.gnuplot == LINES_POINTS)
						      {
							      sprintf( temp_string, "\"./dc_sweep_plot%d.txt\" using 1:%d title '%s' with linespoints", sweep_no + 1, j + 1,
									      (circuit_simulation.plot_settings + l)->name );
						      }
						      else
						      {
							      sprintf( temp_string, "\"./dc_sweep_plot%d.txt\" using 1:%d title '%s' with lines", sweep_no + 1, j + 1,
									      (circuit_simulation.plot_settings + l)->name );
						      }
						      strcat( string, temp_string );
					      }
					      else
					      {
						      if (circuit_simulation.gnuplot == LINES_POINTS)
						      {
							      sprintf( temp_string, "\"./dc_sweep_plot%d.txt\" using 1:%d title '%s' with linespoints, ", sweep_no + 1, j + 1,
									      (circuit_simulation.plot_settings + l)->name );
						      }
						      else
						      {
							      sprintf( temp_string, "\"./dc_sweep_plot%d.txt\" using 1:%d title '%s' with lines, ", sweep_no + 1, j + 1,
									      (circuit_simulation.plot_settings + l)->name );
						      }
						      strcat( string, temp_string );
					      }
				      }
			      }

			      if ((circuit_simulation.dc_sweep_settings + sweep_no)->sweep_name[0] == 'V')
			      {
				      sprintf( temp_string,
						      "; set grid; set zeroaxis; set key out horiz top center; set title \"DC Sweep @ Source %s\t#circuit %s\"; set autoscale; set xlabel \"Sweep Values [Volt]\"; set ylabel \"V values [Volt]\";  replot;",
						      (circuit_simulation.dc_sweep_settings + sweep_no)->sweep_name, circuit_simulation.file_name );
			      }
			      else
			      {
				      sprintf( temp_string,
						      "; set grid; set zeroaxis; set key out horiz top center; set title \"DC Sweep @ Source %s\t#circuit %s\"; set autoscale; set xlabel \"Sweep Values [Ampere]\"; set ylabel \"V values [Volt]\";  replot;",
						      (circuit_simulation.dc_sweep_settings + sweep_no)->sweep_name, circuit_simulation.file_name );
			      }

			      strcat( string, temp_string );
			      fprintf( pipe, "%s\n", string );
			      fclose( pipe );
		      }
	      }

	      memcpy( vector_x, old_vec_x, dimension * sizeof(double) );
	      memcpy( vector_b, old_vec_b, dimension * sizeof(double) );

	     
	      fclose( p_file );
	      
	      if(circuit_simulation.method == ITERATIVE && circuit_simulation.matrix_type == NONSPD && circuit_simulation.iterative_methods.division_by_zero == 1) {
		  printf("\nWarning, division by zero was encountered during Bi-Conjugate Gradient execution.\n");
		  printf("The results might be inaccurate.\n\n");
		  circuit_simulation.iterative_methods.division_by_zero = 0;
	      }
	      
	       printf( "\n\n");
	
	}
	
	free( old_vec_x );
	free( old_vec_b );

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
