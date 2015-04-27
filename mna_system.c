#include "circuit_simulation.h" 

void createMnaSystemDC(void)
{
	int l, k, i, dimension, temp3;
	twoTerminalsElement *current1 = NULL;
	cs *G = NULL;

	dimension = number_of_nodes + group2_elements;

	if (circuit_simulation.matrix_sparsity == SPARSE)
	{
		G = cs_di_spalloc( dimension, dimension, non_zeroes_G, 1, 1 );

		if (G == NULL)
		{
			printf( "Could not allocate matrices.\n" );
			printf( "Terminating.\n" );
			exit( -1 );
		}
	}
	else
	{
		matrix_G = ( double * ) calloc( dimension * dimension, sizeof(double) );

		if (matrix_G == NULL)
		{
			printf( "Could not allocate matrices.\n" );
			printf( "Terminating.\n" );
			exit( -1 );
		}
	}

	memset( vector_b, 0, dimension * sizeof(double) );

	printf( "\n\nCreating MNA System DC: G[x] = [b] (%dx%d)...\n\n", dimension, dimension );

	if (circuit_simulation.matrix_sparsity == SPARSE)
	{
		for (current1 = head_twoTerminalsElement_list, k = 0; head_twoTerminalsElement_list != NULL && current1 != NULL;
				current1 = current1->next)
		{
			switch (current1->type)
			{

				case RESISTANCE:

					if (current1->positive_terminal)
					{
						cs_di_entry(G, current1->positive_terminal - 1, current1->positive_terminal - 1, 1 / (current1->value));
					}

					if (current1->negative_terminal)
					{
						cs_di_entry(G, current1->negative_terminal - 1, current1->negative_terminal - 1, 1 / (current1->value));
					}

					if (current1->positive_terminal && current1->negative_terminal)
					{
						cs_di_entry(G, current1->positive_terminal - 1, current1->negative_terminal - 1, -1 / (current1->value));
						cs_di_entry(G, current1->negative_terminal - 1, current1->positive_terminal - 1, -1 / (current1->value));
					}

					break;

				case CURRENT_SOURCE:

					if (current1->positive_terminal)
					{
						vector_b[current1->positive_terminal - 1] -= current1->value;
					}

					if (current1->negative_terminal)
					{
						vector_b[current1->negative_terminal - 1] += current1->value;
					}

					break;

				case VOLTAGE_SOURCE:
				case INDUCTOR:

					temp3 = number_of_nodes + k;

					if (current1->positive_terminal)
					{
						cs_di_entry(G, current1->positive_terminal - 1, temp3, 1.0);
						cs_di_entry(G, temp3, current1->positive_terminal - 1, 1.0);
					}

					if (current1->negative_terminal)
					{
						cs_di_entry(G, current1->negative_terminal - 1, temp3, -1.0);
						cs_di_entry(G, temp3, current1->negative_terminal - 1, -1.0);
					}

					vector_b[temp3] += current1->type == INDUCTOR ? 0 : current1->value;

					k++;

					break;

			}

		}

		G2 = cs_di_compress( G );
		cs_di_spfree( G );
		cs_di_dupl( G2 );

#if DEBUG
		printf("\nMNA System DC\n~~~~~~~~~~~~~\n\n");
		printf("\nMatrix G:\n\n");
		for(k = 0; k < dimension; k++)
		{
			for(l = 0; l < dimension; l++)
			{
				printf("%12lf ", getElementAt(G2, k, l));
				if(l == number_of_nodes - 1)
				{
					printf("   |");
				}
			}
			printf("\n");

			if(k == number_of_nodes - 1)
			{
				for(i = 0; i < dimension * 14; i++)
				{
					printf("_");
				}
				printf("\n\n");
			}

		}
#endif

	}
	else
	{
		for (current1 = head_twoTerminalsElement_list, k = 0; head_twoTerminalsElement_list != NULL && current1 != NULL; current1 = current1->next)
		{
			switch (current1->type)
			{

				case RESISTANCE:

					if (current1->positive_terminal)
					{
						matrix_G[(current1->positive_terminal - 1) * dimension + (current1->positive_terminal - 1)] += 1 / (current1->value);
					}

					if (current1->negative_terminal)
					{
						matrix_G[(current1->negative_terminal - 1) * dimension + (current1->negative_terminal - 1)] += 1 / (current1->value);
					}

					if (current1->positive_terminal && current1->negative_terminal)
					{
						matrix_G[(current1->positive_terminal - 1) * dimension + (current1->negative_terminal - 1)] -= 1 / (current1->value);
						matrix_G[(current1->negative_terminal - 1) * dimension + (current1->positive_terminal - 1)] -= 1 / (current1->value);
					}

					break;

				case CURRENT_SOURCE:

					if (current1->positive_terminal)
					{
						vector_b[current1->positive_terminal - 1] -= current1->value;
					}

					if (current1->negative_terminal)
					{
						vector_b[current1->negative_terminal - 1] += current1->value;
					}

					break;

				case VOLTAGE_SOURCE:
				case INDUCTOR:

					temp3 = number_of_nodes + k;

					if (current1->positive_terminal)
					{
						matrix_G[(current1->positive_terminal - 1) * dimension + temp3] += 1;
						matrix_G[temp3 * dimension + (current1->positive_terminal - 1)] += 1;
					}

					if (current1->negative_terminal)
					{
						matrix_G[(current1->negative_terminal - 1) * dimension + temp3] -= 1;
						matrix_G[temp3 * dimension + (current1->negative_terminal - 1)] -= 1;
					}

					vector_b[temp3] += current1->type == INDUCTOR ? 0 : current1->value;

					k++;

					break;

			}

		}

#if DEBUG
		printf("\nMNA System DC\n~~~~~~~~~~~~~\n\n");

		printf("\nMatrix G:\n\n");
		for(k = 0; k < dimension; k++)
		{
			for(l = 0; l < dimension; l++)
			{
				printf("%12lf ", matrix_G[k * dimension + l]);
				if(l == number_of_nodes - 1)
				{
					printf("   |");
				}
			}
			printf("\n");

			if(k == number_of_nodes - 1)
			{
				for(i = 0; i < dimension * 14; i++)
				{
					printf("_");
				}
				printf("\n\n");
			}

		}
#endif
	}

#if DEBUG
	printf("\n\n\nVector b:\n\n");
	for(k = 0; k < dimension; k++)
	{
		printf("%12lf ", vector_b[k]);
		printf("\n");
		if(k == number_of_nodes - 1)
		{
			printf("_____________\n\n");
		}
	}

	printf("\n\n");
#endif

}

void createMnaSystemTransient(void)
{
	int l, k, i, j, dimension, temp3;
	twoTerminalsElement *current1 = NULL;
	cs_di *C = NULL;
	double *temp_mat_forward_euler = NULL;
	double temp_val;

	dimension = number_of_nodes + group2_elements;

	if (circuit_simulation.matrix_sparsity == SPARSE)
	{
		C = cs_di_spalloc( dimension, dimension, non_zeroes_C, 1, 1 );

		if (C == NULL)
		{
			printf( "Could not allocate matrices.\n" );
			printf( "Terminating.\n" );
			exit( -1 );
		}
	}
	else
	{
		matrix_C = ( double * ) calloc( dimension * dimension, sizeof(double) );
		temp_matrix = ( double * ) malloc( dimension * dimension * sizeof(double) );

		if (matrix_C == NULL || temp_matrix == NULL)
		{
			printf( "Could not allocate matrices.\n" );
			printf( "Terminating.\n" );
			exit( -1 );
		}

		if (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL)
		{
			temp_matrix2 = ( double * ) malloc( dimension * dimension * sizeof(double) );

			if (temp_matrix2 == NULL)
			{
				printf( "Could not allocate matrices.\n" );
				printf( "Terminating.\n" );
				exit( -1 );
			}

		}
	}

	if (circuit_simulation.method == DIRECT)
	{
		if (circuit_simulation.matrix_sparsity == NORMAL)
		{
			free( matrix_G );

			if (circuit_simulation.matrix_type == NONSPD)
			{
				free( transposition );
			}
		}
		else
		{
			cs_di_sfree( S );
			cs_di_nfree( N );
		}
		createMnaSystemDC();
	}

	sources = ( twoTerminalsElement** ) malloc( (number_of_elements[CURRENT_SOURCE] + group2_elements) * sizeof(twoTerminalsElement) );

	if (sources == NULL)
	{
		printf( "Could not allocate matrices.\n" );
		printf( "Terminating.\n" );
		exit( -1 );
	}

	printf( "\n\nCreating MNA System Transient: G[x] + C[x'] = [e] (%dx%d)...\n\n", dimension, dimension );

	if (circuit_simulation.matrix_sparsity == SPARSE)
	{
		for (current1 = head_twoTerminalsElement_list, k = 0, j = 0; head_twoTerminalsElement_list != NULL && current1 != NULL; current1 =
				current1->next)
		{
			switch (current1->type)
			{

				case CAPACITOR:

					if (current1->positive_terminal)
					{
						cs_di_entry(C, current1->positive_terminal - 1, current1->positive_terminal - 1, current1->value
								* (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL ?
										2 / circuit_simulation.transient_analysis_settings.time_step :
										1 / circuit_simulation.transient_analysis_settings.time_step));
					}

					if (current1->negative_terminal)
					{
						cs_di_entry(C, current1->negative_terminal - 1, current1->negative_terminal - 1, current1->value
								* (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL ?
										2 / circuit_simulation.transient_analysis_settings.time_step :
										1 / circuit_simulation.transient_analysis_settings.time_step));
					}

					if (current1->positive_terminal && current1->negative_terminal)
					{
						/*if (circuit_simulation.transient_analysis_settings.diff_method == FORWARD_EULER)
						 {
						 printf( "Forward Euler will only work if the circuit contains only parasitic capacitors.\n" );
						 printf( "Terminating.\n" );
						 exit( -1 );
						 }*/
						
						
						cs_di_entry(C, current1->positive_terminal - 1, current1->negative_terminal - 1, -current1->value
								* (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL ?
										2 / circuit_simulation.transient_analysis_settings.time_step :
										1 / circuit_simulation.transient_analysis_settings.time_step));
						
						cs_di_entry(C, current1->negative_terminal - 1, current1->positive_terminal - 1, -current1->value
								* (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL ?
										2 / circuit_simulation.transient_analysis_settings.time_step :
										1 / circuit_simulation.transient_analysis_settings.time_step));
					}

					break;

				case CURRENT_SOURCE:
					sources[j] = current1;
					j++;

					if (current1->transient != NULL)
					{
						createTransientSteps( current1->transient );
					}
					break;

				case VOLTAGE_SOURCE:
				case INDUCTOR:

					temp3 = number_of_nodes + k;

					if (current1->type == INDUCTOR)
					{	
						cs_di_entry(C, temp3, temp3, -current1->value
								* (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL ?
										2 / circuit_simulation.transient_analysis_settings.time_step :
										1 / circuit_simulation.transient_analysis_settings.time_step));
					}

					sources[j] = current1;
					j++;

					if (current1->transient != NULL)
					{
						createTransientSteps( current1->transient );
					}

					k++;

					break;

			}

		}


		C2 = cs_di_compress( C );
		cs_di_spfree( C );
		cs_di_dupl( C2 );

		//if (circuit_simulation.transient_analysis_settings.diff_method != FORWARD_EULER)
		//{
		temp_mat = cs_di_add( G2, C2, 1.0, 1.0 );  // temp_mat = G + C

		if (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL)
		{
			temp_mat2 = cs_di_add( G2, C2, 1.0, -1.0 );  // temp_mat2 = G - C
		}
		//}
		/*else
		 {
		 C = cs_spalloc( dimension, dimension, dimension, 1, 1 );

		 if (C == NULL)
		 {
		 printf( "Could not allocate matrices.\n" );
		 printf( "Terminating.\n" );
		 exit( -1 );
		 }

		 for (i = 0; i < dimension; i++)
		 {
		 temp_val = getElementAt( C2, i, i );

		 if (temp_val == 0 && i < number_of_nodes)
		 {
		 temp_val = ZERO / circuit_simulation.transient_analysis_settings.time_step;
		 }
		 else if (temp_val == 0)
		 {
		 printf( "The matrix containing the capacitors/inductors is always assumed to be diagonal.\n" );
		 printf( "One of the diagonal elements found to be zero.\n" );
		 printf( "Terminating.\n" );
		 exit( -1 );
		 }

		 C->i[i] = i;
		 C->p[i] = i;
		 C->x[i] = 1 / temp_val;

		 }

		 cs_spfree( C2 );

		 C->nz = dimension;

		 C2 = cs_compress( C );
		 cs_spfree( C );
		 cs_dupl( C2 );  // C = Inverse(C / h)

		 temp_mat = cs_multiply( C2, G2 );  // temp_mat = C * G <=> temp_mat = Inverse(C / h) * G
		 }*/

#if DEBUG
		printf("\nMNA System Transient\n~~~~~~~~~~~~~~~~~~~~\n\n");
		printf("\nMatrix C:\n\n");
		for(k = 0; k < dimension; k++)
		{
			for(l = 0; l < dimension; l++)
			{
				printf("%12lf ", getElementAt(C2, k, l) * (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL ? circuit_simulation.transient_analysis_settings.time_step / 2 : circuit_simulation.transient_analysis_settings.time_step));
				if(l == number_of_nodes - 1)
				{
					printf("   |");
				}
			}
			printf("\n");

			if(k == number_of_nodes - 1)
			{
				for(i = 0; i < dimension * 14; i++)
				{
					printf("_");
				}
				printf("\n\n");
			}

		}

		printf("\n\n");
#endif

	}
	else
	{
		for (current1 = head_twoTerminalsElement_list, k = 0, j = 0; head_twoTerminalsElement_list != NULL && current1 != NULL;
				current1 = current1->next)
		{
			switch (current1->type)
			{

				case CAPACITOR:

					if (current1->positive_terminal)
					{
						matrix_C[(current1->positive_terminal - 1) * dimension + (current1->positive_terminal - 1)] += current1->value
								* (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL ?
										2 / circuit_simulation.transient_analysis_settings.time_step :
										1 / circuit_simulation.transient_analysis_settings.time_step);
					}

					if (current1->negative_terminal)
					{
						matrix_C[(current1->negative_terminal - 1) * dimension + (current1->negative_terminal - 1)] += current1->value
								* (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL ?
										2 / circuit_simulation.transient_analysis_settings.time_step :
										1 / circuit_simulation.transient_analysis_settings.time_step);
					}

					if (current1->positive_terminal && current1->negative_terminal)
					{
						/*if (circuit_simulation.transient_analysis_settings.diff_method == FORWARD_EULER)
						{
							printf( "Forward Euler will only work if the circuit contains only parasitic capacitors.\n" );
							printf( "Terminating.\n" );
							exit( -1 );
						}*/
						matrix_C[(current1->positive_terminal - 1) * dimension + (current1->negative_terminal - 1)] -= current1->value
								* (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL ?
										2 / circuit_simulation.transient_analysis_settings.time_step :
										1 / circuit_simulation.transient_analysis_settings.time_step);
						matrix_C[(current1->negative_terminal - 1) * dimension + (current1->positive_terminal - 1)] -= current1->value
								* (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL ?
										2 / circuit_simulation.transient_analysis_settings.time_step :
										1 / circuit_simulation.transient_analysis_settings.time_step);
					}

					break;

				case CURRENT_SOURCE:
					sources[j] = current1;
					j++;

					if (current1->transient != NULL)
					{
						createTransientSteps( current1->transient );
					}

					break;

				case VOLTAGE_SOURCE:
				case INDUCTOR:

					temp3 = number_of_nodes + k;

					if (current1->type == INDUCTOR)
					{
						matrix_C[temp3 * dimension + temp3] -= current1->value
								* (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL ?
										2 / circuit_simulation.transient_analysis_settings.time_step :
										1 / circuit_simulation.transient_analysis_settings.time_step);
					}
					else
					{
						if (current1->transient != NULL)
						{
							createTransientSteps( current1->transient );
						}
					}

					sources[j] = current1;
					j++;

					k++;

					break;

			}

		}

		//if (circuit_simulation.transient_analysis_settings.diff_method != FORWARD_EULER)
		//{
			memcpy( temp_matrix, matrix_G, dimension * dimension * sizeof(double) );  // temp_matrix = G
			cblas_daxpy( dimension * dimension, 1.0, matrix_C, 1, temp_matrix, 1 );  // temp_matrix = temp_matrix + C  <=>  temp_matrix = G + C

			if (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL)
			{
				memcpy( temp_matrix2, matrix_G, dimension * dimension * sizeof(double) );  // temp_matrix2 = G
				cblas_daxpy( dimension * dimension, -1.0, matrix_C, 1, temp_matrix2, 1 );  // temp_matrix2 = temp_matrix2 - C  <=>  temp_matrix = G - C
			}
		//}
		/*else
		{
			temp_mat_forward_euler = ( double * ) calloc( dimension * dimension, sizeof(double) );

			if (temp_mat_forward_euler == NULL)
			{
				printf( "Could not allocate matrices.\n" );
				printf( "Terminating.\n" );
				exit( -1 );
			}

			for (i = 0; i < dimension; i++)
			{
				temp_val = matrix_C[i * dimension + i];

				if (temp_val == 0 && i < number_of_nodes)
				{
					temp_val = ZERO / circuit_simulation.transient_analysis_settings.time_step;
				}
				if (temp_val == 0)
				{
					printf( "The matrix containing the capacitors/inductors is always assumed to be diagonal.\n" );
					printf( "One of the diagonal elements found to be zero.\n" );
					printf( "Terminating.\n" );
					exit( -1 );
				}

				temp_mat_forward_euler[i * dimension + i] = 1 / temp_val;
			}

			free( matrix_C );

			matrix_C = temp_mat_forward_euler;  // C = Inverse(C / h)
			cblas_dgemm( CblasRowMajor, CblasNoTrans, CblasNoTrans, dimension, dimension, dimension, 1.0, matrix_C, dimension, matrix_G, dimension,
					0.0, temp_matrix, dimension );  // temp_matrix = C * G <=> temp_matrix = Inverse(C / h) * G
		}*/

#if DEBUG
		printf("\nMNA System Transient\n~~~~~~~~~~~~~~~~~~~~\n\n");
		printf("\nMatrix C:\n\n");
		for(k = 0; k < dimension; k++)
		{
			for(l = 0; l < dimension; l++)
			{
				printf("%12lf ", matrix_C[k * dimension + l] * (circuit_simulation.transient_analysis_settings.diff_method == TRAPEZOIDAL ? circuit_simulation.transient_analysis_settings.time_step / 2 : circuit_simulation.transient_analysis_settings.time_step));
				if(l == number_of_nodes - 1)
				{
					printf("   |");
				}
			}
			printf("\n");

			if(k == number_of_nodes - 1)
			{
				for(i = 0; i < dimension * 14; i++)
				{
					printf("_");
				}
				printf("\n\n");
			}

		}

		printf("\n\n");
#endif

	}

}

void createTransientSteps(transientComponent* transient)
{
	int i, k, length;
	double t, time_steps;
	ExpImpulse* e = NULL;
	SinImpulse* s = NULL;
	PulseImpulse* p = NULL;
	PWLImpulse* pw = NULL;

	time_steps = circuit_simulation.transient_analysis_settings.fin_time / circuit_simulation.transient_analysis_settings.time_step;

	length = ( int ) time_steps + 1;

	transient->vals = ( double * ) malloc( length * sizeof(double) );

	if (transient->vals == NULL)
	{
		printf( "Could not allocate matrices.\n" );
		printf( "Terminating.\n" );
		exit( -1 );
	}

	switch (transient->impulse)
	{
		case EXP:
			e = transient->exp;
			for (i = 0, t = 0; i < length; i++, t += circuit_simulation.transient_analysis_settings.time_step)
			{  //precalculate values for every time step
				if (t >= 0 && t < e->td1)
				{
					transient->vals[i] = e->i1;
				}
				else if (t >= e->td1 && t < e->td2)
				{
					transient->vals[i] = e->i1 + (e->i2 - e->i1) * (1 - exp( -(t - e->td1) / e->tc1 ));
				}
				else if (t >= e->td2)
				{
					transient->vals[i] = e->i1 + (e->i2 - e->i1) * (exp( -(t - e->td2) / e->tc2 ) - exp( -(t - e->td1) / e->tc1 ));
				}
			}
			break;
		case SIN:
			s = transient->sin;
			for (i = 0, t = 0; i < length; i++, t += circuit_simulation.transient_analysis_settings.time_step)
			{
				if (t >= 0 && t < s->td)
				{
					transient->vals[i] = s->i1 + s->ia * sin( (2 * M_PI * s->ph) / 360 );
				}
				else if (t >= s->td)
				{
					transient->vals[i] = s->i1
							+ s->ia * sin( 2 * M_PI * s->fr * (t - s->td) + (2 * M_PI * (s->ph)) / 360 ) * exp( -(t - s->td) * s->df );
				}
			}
			break;
		case PULSE:
			p = transient->pulse;
			for (i = 0, t = 0, k = 0; i < length; i++, t += circuit_simulation.transient_analysis_settings.time_step)
			{
				if (t >= p->td + (k + 1) * p->per)
				{
					k++;
				}

				if (t >= 0 && t < p->td)
				{
					transient->vals[i] = p->i1;
				}
				else if (t >= p->td + k * p->per && t < p->td + p->tr + k * p->per)
				{
					transient->vals[i] = (p->i2 - p->i1) / (p->td + p->tr + k * p->per - (p->td + k * p->per)) * (t - (p->td + k * p->per)) + p->i1;
				}
				else if (t >= p->td + p->tr + k * p->per && t < p->td + p->tr + p->pw + k * p->per)
				{
					transient->vals[i] = p->i2;
				}
				else if (t >= p->td + p->tr + p->pw + k * p->per && t < p->td + p->tr + p->pw + p->tf + k * p->per)
				{
					transient->vals[i] = (p->i1 - p->i2) / (p->td + p->tr + p->pw + p->tf + k * p->per - (p->td + p->tr + p->pw + k * p->per))
							* (t - (p->td + p->tr + p->pw + k * p->per)) + p->i2;
				}
				else if (t >= p->td + p->tr + p->pw + p->tf + k * p->per && t <= p->td + (k + 1) * p->per)
				{
					transient->vals[i] = p->i1;
				}
			}
			break;
		case PWL:
			pw = transient->pwl;
			pw->t[pw->length - 1] = circuit_simulation.transient_analysis_settings.fin_time;
			for (i = 0, t = 0, k = 0; i < length; i++, t += circuit_simulation.transient_analysis_settings.time_step)
			{
				for (; k < pw->length - 1; k++)
				{
					if (t >= pw->t[k] && t <= pw->t[k + 1])
					{
						break;
					}
				}

				transient->vals[i] = ((pw->i[k + 1] - pw->i[k]) / (pw->t[k + 1] - pw->t[k])) * (t - pw->t[k]) + pw->i[k];
			}
			break;

	}

}

void createMnaSystemAC(double f, long int run) {
  
        int l, k, i, dimension, temp3;
	twoTerminalsElement *current1 = NULL;
	cs_ci *Gcomplex = NULL;
	double angle_rad;
	double complex temp_val;
	
	double omega = 2 * M_PI * f;

	dimension = number_of_nodes + group2_elements;

	if (circuit_simulation.matrix_sparsity == SPARSE)
	{	        
	        Gcomplex = cs_ci_spalloc( dimension, dimension, non_zeroes_Gcomplex, 1, 1 );

		if (Gcomplex == NULL)
		{
			printf( "Could not allocate matrices.\n" );
			printf( "Terminating.\n" );
			exit( -1 );
		}
	}
	else
	{
		  matrix_Gcomplex = ( double complex * ) calloc( dimension * dimension, sizeof(double complex) );

		  if (matrix_Gcomplex == NULL)
		  {
		      	printf( "Could not allocate matrices.\n" );
			printf( "Terminating.\n" );
			exit( -1 );
		  }
	}

	if(run == 0) {
	    printf( "\n\nCreating MNA System AC: G[x] = [b] (%dx%d)...\n\n", dimension, dimension );
	}

	if (circuit_simulation.matrix_sparsity == SPARSE)
	{
		for (current1 = head_twoTerminalsElement_list, k = 0; head_twoTerminalsElement_list != NULL && current1 != NULL;
				current1 = current1->next)
		{
			switch (current1->type)
			{

				case RESISTANCE:

					if (current1->positive_terminal)
					{
						cs_ci_entry (Gcomplex, current1->positive_terminal - 1, current1->positive_terminal - 1, 1 / (current1->value));
					}

					if (current1->negative_terminal)
					{
						cs_ci_entry (Gcomplex, current1->negative_terminal - 1, current1->negative_terminal - 1, 1 / (current1->value));
					}

					if (current1->positive_terminal && current1->negative_terminal)
					{
						
						cs_ci_entry (Gcomplex, current1->positive_terminal - 1, current1->negative_terminal - 1, -1 / (current1->value));
						cs_ci_entry (Gcomplex, current1->negative_terminal - 1, current1->positive_terminal - 1, -1 / (current1->value));
					}

					break;
					
				case CAPACITOR:
				        if (current1->positive_terminal)
					{
						cs_ci_entry (Gcomplex, current1->positive_terminal - 1, current1->positive_terminal - 1, current1->value * omega * I);
					}

					if (current1->negative_terminal)
					{
						cs_ci_entry (Gcomplex, current1->negative_terminal - 1, current1->negative_terminal - 1, current1->value * omega * I);
					}

					if (current1->positive_terminal && current1->negative_terminal)
					{
						cs_ci_entry (Gcomplex, current1->positive_terminal - 1, current1->negative_terminal - 1, 0.0 -current1->value * omega * I);
						cs_ci_entry (Gcomplex, current1->negative_terminal - 1, current1->positive_terminal - 1, 0.0 -current1->value * omega * I);
					}
				        break;

				case CURRENT_SOURCE:
				        if(run == 0) {
					    temp_val = 0;
					    
					    if(current1 -> ac != NULL) {
						angle_rad = current1 -> ac -> phase * (M_PI / 180);
						
						temp_val =  current1 -> ac -> mag * (cos(angle_rad) + sin(angle_rad) * I);
					    }
					    
					    
					    if (current1->positive_terminal)
					    {			
						vector_b_complex[current1->positive_terminal - 1] -= temp_val;	
					    }

					    if (current1->negative_terminal)
					    {
						vector_b_complex[current1->negative_terminal - 1] += temp_val;
					    }
					}
					break;

				case VOLTAGE_SOURCE:
				        temp3 = number_of_nodes + k;
					
					if (current1->positive_terminal)
					{						
						cs_ci_entry (Gcomplex, current1->positive_terminal - 1, temp3, 1.0);
						cs_ci_entry (Gcomplex, temp3, current1->positive_terminal - 1, 1.0);
					}
					
					if (current1->negative_terminal)
					{	
						cs_ci_entry (Gcomplex, current1->negative_terminal - 1, temp3, -1.0);
						cs_ci_entry (Gcomplex, temp3, current1->negative_terminal - 1, -1.0);
					}
					
					if(run == 0) {
					    temp_val = 0;
					    
					    if(current1 -> ac != NULL) {
						angle_rad = current1 -> ac -> phase * (M_PI / 180);
						
						temp_val =  current1 -> ac -> mag * (cos(angle_rad) + sin(angle_rad) * I);
					    }
					    			    
					    vector_b_complex[temp3] += temp_val;
					}
					
					k++;
				        break;
				case INDUCTOR:

					temp3 = number_of_nodes + k;

					if (current1->positive_terminal)
					{	
						cs_ci_entry (Gcomplex, current1->positive_terminal - 1, temp3, 1.0);
						cs_ci_entry (Gcomplex, temp3, current1->positive_terminal - 1, 1.0);
					}
					
					if (current1->negative_terminal)
					{	
						cs_ci_entry (Gcomplex, current1->negative_terminal - 1, temp3, -1.0);
						cs_ci_entry (Gcomplex, temp3, current1->negative_terminal - 1, -1.0);
					}
					
					cs_ci_entry (Gcomplex, temp3, temp3, 0.0 -current1->value * omega * I);			

					k++;

					break;

			}

		}

		G2complex = cs_ci_compress( Gcomplex );
		cs_ci_spfree( Gcomplex );
		cs_ci_dupl( G2complex );

#if DEBUG
		printf("\nMNA System AC\n~~~~~~~~~~~~~\n\n");
		printf("\nMatrix G:\n\n");
		for(k = 0; k < dimension; k++)
		{
			for(l = 0; l < dimension; l++)
			{
			        temp_val = getElementAtComplex(G2complex, k, l);
			        printf("(%7.3lf %7.3lfi) ", creal(temp_val), cimag(temp_val));
				if(l == number_of_nodes - 1)
				{
					printf("   |");
				}
			}
			printf("\n");

			if(k == number_of_nodes - 1)
			{
				for(i = 0; i < dimension * 20; i++)
				{
					printf("_");
				}
				printf("\n\n");
			}

		}
#endif

	}
	else
	{
		for (current1 = head_twoTerminalsElement_list, k = 0; head_twoTerminalsElement_list != NULL && current1 != NULL; current1 = current1->next)
		{
			switch (current1->type)
			{

				case RESISTANCE:

					if (current1->positive_terminal)
					{
						matrix_Gcomplex[(current1->positive_terminal - 1) * dimension + (current1->positive_terminal - 1)] += 1 / (current1->value);
					}

					if (current1->negative_terminal)
					{
						matrix_Gcomplex[(current1->negative_terminal - 1) * dimension + (current1->negative_terminal - 1)] += 1 / (current1->value);
					}

					if (current1->positive_terminal && current1->negative_terminal)
					{
						matrix_Gcomplex[(current1->positive_terminal - 1) * dimension + (current1->negative_terminal - 1)] -= 1 / (current1->value);
						matrix_Gcomplex[(current1->negative_terminal - 1) * dimension + (current1->positive_terminal - 1)] -= 1 / (current1->value);
					}

					break;
			        case CAPACITOR:
				        if (current1->positive_terminal)
					{
						matrix_Gcomplex[(current1->positive_terminal - 1) * dimension + (current1->positive_terminal - 1)] += current1->value * omega * I;
					}

					if (current1->negative_terminal)
					{
						matrix_Gcomplex[(current1->negative_terminal - 1) * dimension + (current1->negative_terminal - 1)] += current1->value * omega * I;
					}

					if (current1->positive_terminal && current1->negative_terminal)
					{			
					        matrix_Gcomplex[(current1->positive_terminal - 1) * dimension + (current1->negative_terminal - 1)] -= current1->value * omega * I;
						matrix_Gcomplex[(current1->negative_terminal - 1) * dimension + (current1->positive_terminal - 1)] -= current1->value * omega * I;
					}
				        break;

				case CURRENT_SOURCE:

					if(run == 0) {
					    temp_val = 0;
					    
					    if(current1 -> ac != NULL) {
						angle_rad = current1 -> ac -> phase * (M_PI / 180);
						
						temp_val =  current1 -> ac -> mag * (cos(angle_rad) + sin(angle_rad) * I);
					    }
					    
					    
					    if (current1->positive_terminal)
					    {			
						vector_b_complex[current1->positive_terminal - 1] -= temp_val;	
					    }

					    if (current1->negative_terminal)
					    {
						vector_b_complex[current1->negative_terminal - 1] += temp_val;
					    }
					}

					break;

				case VOLTAGE_SOURCE:
				        temp3 = number_of_nodes + k;
					
					if (current1->positive_terminal)
					{
						matrix_Gcomplex[(current1->positive_terminal - 1) * dimension + temp3] += 1;
						matrix_Gcomplex[temp3 * dimension + (current1->positive_terminal - 1)] += 1;
					}

					if (current1->negative_terminal)
					{
						matrix_Gcomplex[(current1->negative_terminal - 1) * dimension + temp3] -= 1;
						matrix_Gcomplex[temp3 * dimension + (current1->negative_terminal - 1)] -= 1;
					}
					
					if(run == 0) {
					    temp_val = 0;
					    
					    if(current1 -> ac != NULL) {
						angle_rad = current1 -> ac -> phase * (M_PI / 180);
						
						temp_val =  current1 -> ac -> mag * (cos(angle_rad) + sin(angle_rad) * I);
					    }
					    			    
					    vector_b_complex[temp3] += temp_val;
					}
				  
					k++;
				        break;
				case INDUCTOR:

					temp3 = number_of_nodes + k;

					if (current1->positive_terminal)
					{
						matrix_Gcomplex[(current1->positive_terminal - 1) * dimension + temp3] += 1;
						matrix_Gcomplex[temp3 * dimension + (current1->positive_terminal - 1)] += 1;
					}

					if (current1->negative_terminal)
					{
						matrix_Gcomplex[(current1->negative_terminal - 1) * dimension + temp3] -= 1;
						matrix_Gcomplex[temp3 * dimension + (current1->negative_terminal - 1)] -= 1;
					}
					
					matrix_Gcomplex[temp3 * dimension + temp3] -= current1->value * omega * I;	

					k++;

					break;

			}

		}

#if DEBUG
		printf("\nMNA System AC\n~~~~~~~~~~~~~\n\n");

		printf("\nMatrix G:\n\n");
		for(k = 0; k < dimension; k++)
		{
			for(l = 0; l < dimension; l++)
			{
				printf("(%7.3lf %7.3lfi) ", creal(matrix_Gcomplex[k * dimension + l]), cimag(matrix_Gcomplex[k * dimension + l]));
				if(l == number_of_nodes - 1)
				{
					printf("   |");
				}
			}
			printf("\n");

			if(k == number_of_nodes - 1)
			{
				for(i = 0; i < dimension * 20; i++)
				{
					printf("_");
				}
				printf("\n\n");
			}

		}
#endif
	}

#if DEBUG
	printf("\n\n\nVector b:\n\n");
	for(k = 0; k < dimension; k++)
	{
	        printf("(%7.3lf %7.3lfi) ", creal(vector_b_complex[k]), cimag(vector_b_complex[k]));
		printf("\n");
		if(k == number_of_nodes - 1)
		{
			printf("__________________\n\n");
		}
	}

	printf("\n\n");
#endif

  
}

