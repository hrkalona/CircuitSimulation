#include "circuit_simulation.h" 

void createMnaSystemDC(void)
{
#if DEBUG
	int l, k, i;
#endif
	int group2_index, group2_counter;
	unsigned int dimension;
	twoTerminalsElement *current1 = NULL;
	cs *G = NULL;

	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;

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

		for (current1 = head_twoTerminalsElement_list, group2_counter = 0; head_twoTerminalsElement_list != NULL && current1 != NULL;
				current1 = current1->next)
		{
			switch (current1->type)
			{

				case RESISTANCE:
				       if (circuit_simulation.matrix_sparsity == SPARSE) {
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
                                            
                                             if(current1->isG2) {
                                               group2_index = circuit_simulation.number_of_nodes + group2_counter;
                                              if (current1->positive_terminal)
					      {
						      cs_di_entry(G, current1->positive_terminal - 1, group2_index, 1.0);
						      cs_di_entry(G, group2_index, current1->positive_terminal - 1, 1.0);
					      }

					      if (current1->negative_terminal)
					      {
						      cs_di_entry(G, current1->negative_terminal - 1, group2_index, -1.0);
						      cs_di_entry(G, group2_index, current1->negative_terminal - 1, -1.0);
					      }
                                              cs_di_entry(G, group2_index, group2_index, -current1->value);
                    
                                              group2_counter++;
                                            }
				       }
				       else {
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
                                            
                                            if(current1->isG2) {
                                              group2_index = circuit_simulation.number_of_nodes + group2_counter;
                                              if (current1->positive_terminal)
					      {
						      matrix_G[(current1->positive_terminal - 1) * dimension + group2_index] += 1;
						      matrix_G[group2_index * dimension + (current1->positive_terminal - 1)] += 1;
					      }

					      if (current1->negative_terminal)
					      {
						      matrix_G[(current1->negative_terminal - 1) * dimension + group2_index] -= 1;
						      matrix_G[group2_index * dimension + (current1->negative_terminal - 1)] -= 1;
					      }
                                              matrix_G[group2_index * dimension + group2_index] -= current1->value;
                                              group2_counter++;
                                            }
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
                                case CAPACITOR:
                                         if(current1->isG2) {
                                              group2_index = circuit_simulation.number_of_nodes + group2_counter;

                                              if (circuit_simulation.matrix_sparsity == SPARSE) {
						 
                                                       cs_di_entry(G, group2_index, group2_index, 1.0);
                                              }
                                              else {

		                                      matrix_G[group2_index * dimension + group2_index] += 1;
                                              }
                                              group2_counter++;
                                            }
                                        break;

				case VOLTAGE_SOURCE:
				case INDUCTOR:
                                case SHORT_CIRCUIT:

					group2_index = circuit_simulation.number_of_nodes + group2_counter;

					if (circuit_simulation.matrix_sparsity == SPARSE) {
					      if (current1->positive_terminal)
					      {
						      cs_di_entry(G, current1->positive_terminal - 1, group2_index, 1.0);
						      cs_di_entry(G, group2_index, current1->positive_terminal - 1, 1.0);
					      }

					      if (current1->negative_terminal)
					      {
						      cs_di_entry(G, current1->negative_terminal - 1, group2_index, -1.0);
						      cs_di_entry(G, group2_index, current1->negative_terminal - 1, -1.0);
					      }
					}
					else {
					      if (current1->positive_terminal)
					      {
						      matrix_G[(current1->positive_terminal - 1) * dimension + group2_index] += 1;
						      matrix_G[group2_index * dimension + (current1->positive_terminal - 1)] += 1;
					      }

					      if (current1->negative_terminal)
					      {
						      matrix_G[(current1->negative_terminal - 1) * dimension + group2_index] -= 1;
						      matrix_G[group2_index * dimension + (current1->negative_terminal - 1)] -= 1;
					      }
					}

					vector_b[group2_index] += current1->type == INDUCTOR ||  current1->type == SHORT_CIRCUIT ? 0 : current1->value;

					group2_counter++;

					break;
                                case VOLTAGE_CONTROLLED_CURRENT_SOURCE:
                                        if (circuit_simulation.matrix_sparsity == SPARSE) {
					     if(current1 -> positive_terminal) {
						    if(current1 -> in_positive_terminal) {
                                                           cs_di_entry(G, current1->positive_terminal - 1, current1 -> in_positive_terminal - 1,  current1->value);
						    }

						     if(current1 -> in_negative_terminal) {
                                                           cs_di_entry(G, current1->positive_terminal - 1, current1 -> in_negative_terminal - 1,  -current1->value);
						    }
						}
		
						if(current1 -> negative_terminal) {
						    if(current1 -> in_positive_terminal) {
                                                         cs_di_entry(G, current1->negative_terminal - 1, current1 -> in_positive_terminal - 1,  -current1->value);
						    }

						     if(current1 -> in_negative_terminal) {
							   cs_di_entry(G, current1->negative_terminal - 1, current1 -> in_negative_terminal - 1,  current1->value);
						    }
						}
                                        }
                                        else {
                                           if(current1 -> positive_terminal) {
						    if(current1 -> in_positive_terminal) {
							 matrix_G[(current1->positive_terminal - 1) * dimension + (current1 -> in_positive_terminal - 1)]+=  current1->value;
						    }

						     if(current1 -> in_negative_terminal) {
							  matrix_G[(current1->positive_terminal - 1) * dimension + (current1 -> in_negative_terminal - 1)] -=  current1->value;
						    }
						}
		
						if(current1 -> negative_terminal) {
						    if(current1 -> in_positive_terminal) {
							 matrix_G[(current1->negative_terminal - 1) * dimension + (current1 -> in_positive_terminal - 1)] -=  current1->value;
						    }

						     if(current1 -> in_negative_terminal) {
							  matrix_G[(current1->negative_terminal - 1) * dimension + (current1 -> in_negative_terminal - 1)] +=  current1->value;
						    }
						}

                                        }
                                        break;
                                case VOLTAGE_CONTROLLED_VOLTAGE_SOURCE:
                                        group2_index = circuit_simulation.number_of_nodes + group2_counter;
                                        
                                        if (circuit_simulation.matrix_sparsity == SPARSE) {
                                              if (current1->positive_terminal)
					      {
						      cs_di_entry(G, current1->positive_terminal - 1, group2_index, 1.0);
						      cs_di_entry(G, group2_index, current1->positive_terminal - 1, 1.0);
					      }

					      if (current1->negative_terminal)
					      {
						      cs_di_entry(G, current1->negative_terminal - 1, group2_index, -1.0);
						      cs_di_entry(G, group2_index, current1->negative_terminal - 1, -1.0);
					      }

                                              
                                               if (current1->in_positive_terminal) {
                                                    cs_di_entry(G, group2_index, current1->in_positive_terminal - 1, -current1->value);                                             
                  
                                               }

                                                if (current1->in_negative_terminal) {                                             
                                                    cs_di_entry(G, group2_index, current1->in_negative_terminal - 1, current1->value);       
                                               }

                                        }
                                        else {
                                             if (current1->positive_terminal) {
                                                 matrix_G[(current1->positive_terminal - 1) * dimension + group2_index] += 1;
                                                 matrix_G[group2_index * dimension + (current1->positive_terminal - 1)] += 1;
                                             }

                                              if (current1->negative_terminal)
					      {
						      matrix_G[(current1->negative_terminal - 1) * dimension + group2_index] -= 1;
						      matrix_G[group2_index * dimension + (current1->negative_terminal - 1)] -= 1;
					      }

                                               if (current1->in_positive_terminal) {                                             
                                                 matrix_G[group2_index * dimension + (current1->in_positive_terminal - 1)] -=  current1->value;
                                               }

                                                if (current1->in_negative_terminal) {                                             
                                                 matrix_G[group2_index * dimension + (current1->in_negative_terminal - 1)] =  current1->value;
                                               }
                                        }
                                        group2_counter++;
                                        break;
				default:
					break;

			}

		}

		if (circuit_simulation.matrix_sparsity == SPARSE) {
		      G2 = cs_di_compress( G );
		      cs_di_spfree( G );
		      cs_di_dupl( G2 );
		}

#if DEBUG
		      printf("\nMNA System DC\n~~~~~~~~~~~~~\n\n");
		      printf("\nMatrix G:\n\n");
		      for(k = 0; k < dimension; k++)
		      {
			      for(l = 0; l < dimension; l++)
			      {
				      if (circuit_simulation.matrix_sparsity == SPARSE) {     
					  printf("%12lf ", getElementAt(G2, k, l));
				      }
				      else {
					  printf("%12lf ", matrix_G[k * dimension + l]);
				      }
				      if(l == circuit_simulation.number_of_nodes - 1)
				      {
					      printf("   |");
				      }
			      }
			      printf("\n");

			      if(k == circuit_simulation.number_of_nodes - 1)
			      {
				      for(i = 0; i < dimension * 14; i++)
				      {
					      printf("_");
				      }
				      printf("\n\n");
			      }

		      }
#endif
	

#if DEBUG
	printf("\n\n\nVector b:\n\n");
	for(k = 0; k < dimension; k++)
	{
		printf("%12lf ", vector_b[k]);
		printf("\n");
		if(k == circuit_simulation.number_of_nodes - 1)
		{
			printf("_____________\n\n");
		}
	}

	printf("\n\n");
#endif

}

void createMnaSystemTransient(int run, double time_step, double fin_time)
{
#if DEBUG
	int l, k, i;
#endif
	int group2_counter, sources_counter, group2_index;
	unsigned int dimension;
	twoTerminalsElement *current1 = NULL;
	cs_di *C = NULL;

	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;

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

		if (circuit_simulation.diff_method == TRAPEZOIDAL)
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

	if(run > 0) {
	      createMnaSystemDC();
	}
	else if (circuit_simulation.method == DIRECT)
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

	sources = ( twoTerminalsElement** ) calloc( (circuit_simulation.number_of_elements[CURRENT_SOURCE] + circuit_simulation.group2_elements), sizeof(twoTerminalsElement));

	if (sources == NULL)
	{
		printf( "Could not allocate matrices.\n" );
		printf( "Terminating.\n" );
		exit( -1 );
	}

	printf( "\n\nCreating MNA System Transient: G[x] + C[x'] = [e] (%dx%d)...\n\n", dimension, dimension );

		for (current1 = head_twoTerminalsElement_list, group2_counter = 0, sources_counter = 0; head_twoTerminalsElement_list != NULL && current1 != NULL; current1 =
				current1->next)
		{
			switch (current1->type)
			{

				case CAPACITOR:                               
					if (circuit_simulation.matrix_sparsity == SPARSE)
					{			  
					      if (current1->positive_terminal)
					      {
						      cs_di_entry(C, current1->positive_terminal - 1, current1->positive_terminal - 1, current1->value
								      * (circuit_simulation.diff_method == TRAPEZOIDAL ?
										      2 / time_step :
										      1 / time_step));
					      }

					      if (current1->negative_terminal)
					      {
						      cs_di_entry(C, current1->negative_terminal - 1, current1->negative_terminal - 1, current1->value
								      * (circuit_simulation.diff_method == TRAPEZOIDAL ?
										      2 / time_step :
										      1 / time_step));
					      }

					      if (current1->positive_terminal && current1->negative_terminal)
					      {
						      
						      cs_di_entry(C, current1->positive_terminal - 1, current1->negative_terminal - 1, -current1->value
								      * (circuit_simulation.diff_method == TRAPEZOIDAL ?
										      2 / time_step :
										      1 / time_step));
						      
						      cs_di_entry(C, current1->negative_terminal - 1, current1->positive_terminal - 1, -current1->value
								      * (circuit_simulation.diff_method == TRAPEZOIDAL ?
										      2 / time_step :
										      1 / time_step));
					      }
					}
					else {
					      if (current1->positive_terminal)
					      {
						      matrix_C[(current1->positive_terminal - 1) * dimension + (current1->positive_terminal - 1)] += current1->value
								      * (circuit_simulation.diff_method == TRAPEZOIDAL ?
										      2 / time_step :
										      1 / time_step);
					      }

					      if (current1->negative_terminal)
					      {
						      matrix_C[(current1->negative_terminal - 1) * dimension + (current1->negative_terminal - 1)] += current1->value
								      * (circuit_simulation.diff_method == TRAPEZOIDAL ?
										      2 / time_step :
										      1 / time_step);
					      }

					      if (current1->positive_terminal && current1->negative_terminal)
					      {

						      matrix_C[(current1->positive_terminal - 1) * dimension + (current1->negative_terminal - 1)] -= current1->value
								      * (circuit_simulation.diff_method == TRAPEZOIDAL ?
										      2 / time_step :
										      1 / time_step);
						      matrix_C[(current1->negative_terminal - 1) * dimension + (current1->positive_terminal - 1)] -= current1->value
								      * (circuit_simulation.diff_method == TRAPEZOIDAL ?
										      2 / time_step :
										      1 / time_step);
					      }
					}


                                         if(current1->isG2) {
                                              group2_index = circuit_simulation.number_of_nodes + group2_counter;

                                              if (circuit_simulation.matrix_sparsity == SPARSE) {
						 
                                                       
                                                       if (current1->positive_terminal) {
                                                            cs_di_entry(C, group2_index, current1->positive_terminal - 1, -current1->value
								      * (circuit_simulation.diff_method == TRAPEZOIDAL ?
										      2 / time_step :
										      1 / time_step));
                                                       }

                                                        if (current1->negative_terminal) {
 								cs_di_entry(C, group2_index, current1->negative_terminal - 1, current1->value
								      * (circuit_simulation.diff_method == TRAPEZOIDAL ?
										      2 / time_step :
										      1 / time_step));
                                                        }
                                              }
                                              else {

                                                       if (current1->positive_terminal) {
                                                             matrix_C[group2_index * dimension + (current1->positive_terminal - 1)] -= current1->value
								      * (circuit_simulation.diff_method == TRAPEZOIDAL ?
										      2 / time_step :
										      1 / time_step);
                                                       }

                                                        if (current1->negative_terminal) {

 						             matrix_C[group2_index * dimension + (current1->negative_terminal - 1)] += current1->value
								      * (circuit_simulation.diff_method == TRAPEZOIDAL ?
										      2 / time_step :
										      1 / time_step);
                                                        }
		                                     
                                              }

                                              
					      sources_counter++;
                                              group2_counter++;
                                            }

					break;

				case CURRENT_SOURCE:
					sources[sources_counter] = current1;
					sources_counter++;

					if (current1->transient != NULL)
					{
						createTransientSteps( current1->transient, time_step, fin_time );
					}
					break;

				case VOLTAGE_SOURCE:
				case INDUCTOR:
                                case SHORT_CIRCUIT:

					group2_index = circuit_simulation.number_of_nodes + group2_counter;
					
					if (current1->type == INDUCTOR)
					{	
					      if (circuit_simulation.matrix_sparsity == SPARSE)
					      {
						      cs_di_entry(C, group2_index, group2_index, -current1->value
								      * (circuit_simulation.diff_method == TRAPEZOIDAL ?
										      2 / time_step :
										      1 / time_step));
					      }
					      else {
						      
						      matrix_C[group2_index * dimension + group2_index] -= current1->value
								* (circuit_simulation.diff_method == TRAPEZOIDAL ?
										2 / time_step :
										1 / time_step);
					      }
					}
					else if(current1->type == VOLTAGE_SOURCE)
					{
						if (current1->transient != NULL)
						{
							createTransientSteps( current1->transient, time_step, fin_time );
						}
					}
					

                                        if(current1->type == VOLTAGE_SOURCE) {
						sources[sources_counter] = current1;
                                        }
					sources_counter++;
					group2_counter++;

					break;
				case RESISTANCE:
                                        if(current1->isG2) {
						sources_counter++;
		                                group2_counter++;
					}
					break;
                                      
				default:
				       break;

			}

		}


		if (circuit_simulation.matrix_sparsity == SPARSE)
		{
		      C2 = cs_di_compress( C );
		      cs_di_spfree( C );
		      cs_di_dupl( C2 );

		      temp_mat = cs_di_add( G2, C2, 1.0, 1.0 );  // temp_mat = G + C

		      if (circuit_simulation.diff_method == TRAPEZOIDAL)
		      {
			      temp_mat2 = cs_di_add( G2, C2, 1.0, -1.0 );  // temp_mat2 = G - C
		      }
		}
		else {
		      memcpy( temp_matrix, matrix_G, dimension * dimension * sizeof(double) );  // temp_matrix = G
		      cblas_daxpy( dimension * dimension, 1.0, matrix_C, 1, temp_matrix, 1 );  // temp_matrix = temp_matrix + C  <=>  temp_matrix = G + C

		      if (circuit_simulation.diff_method == TRAPEZOIDAL)
		      {
			      memcpy( temp_matrix2, matrix_G, dimension * dimension * sizeof(double) );  // temp_matrix2 = G
		              cblas_daxpy( dimension * dimension, -1.0, matrix_C, 1, temp_matrix2, 1 );  // temp_matrix2 = temp_matrix2 - C  <=>  temp_matrix = G - C
		      }  
		}
		
#if DEBUG
		printf("\nMNA System Transient\n~~~~~~~~~~~~~~~~~~~~\n\n");
		printf("\nMatrix C:\n\n");
		for(k = 0; k < dimension; k++)
		{
			for(l = 0; l < dimension; l++)
			{
				if (circuit_simulation.matrix_sparsity == SPARSE)
				{
				      printf("%12lf ", getElementAt(C2, k, l) * (circuit_simulation.diff_method == TRAPEZOIDAL ? time_step / 2 : time_step));
				}
				else {
				      printf("%12lf ", matrix_C[k * dimension + l] * (circuit_simulation.diff_method == TRAPEZOIDAL ? time_step / 2 : time_step));
				}
				
				if(l == circuit_simulation.number_of_nodes - 1)
				{
					printf("   |");
				}
			}
			printf("\n");

			if(k == circuit_simulation.number_of_nodes - 1)
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
		
		if (circuit_simulation.matrix_sparsity == SPARSE)
		{
			cs_di_spfree( G2 );
			G2 = temp_mat;  // G = G + C, trapezoidal => G = G + 2/h * C, backward euler => G = G + 1/h * C
		}
		else
		{
			free( matrix_G );
			matrix_G = temp_matrix;  // G = G + C, trapezoidal => G = G + 2/h * C, backward euler => G = G + 1/h * C
		}
		
		
		if (circuit_simulation.diff_method == TRAPEZOIDAL)
		{

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
	

}

void createTransientSteps(transientComponent* transient, double time_step, double fin_time)
{
	int i, k, length;
	double t, time_steps;
	ExpImpulse* e = NULL;
	SinImpulse* s = NULL;
	PulseImpulse* p = NULL;
	PWLImpulse* pw = NULL;
        SFFMImpulse *sffm = NULL;

	time_steps = fin_time / time_step;

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
			for (i = 0, t = 0; i < length; i++, t += time_step)
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
			for (i = 0, t = 0; i < length; i++, t += time_step)
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
                case SFFM:
                        sffm = transient->sffm;
                        for (i = 0, t = 0; i < length; i++, t += time_step) {
                           transient->vals[i] = sffm->v0 + sffm->va * sin( 2 * M_PI * sffm->fc * t + sffm->mdi * sin( 2 * M_PI * sffm->fs * t));
                        }
                        break;
		case PULSE:
			p = transient->pulse;
			for (i = 0, t = 0, k = 0; i < length; i++, t += time_step)
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
			pw->t[pw->length - 1] = fin_time;
			for (i = 0, t = 0, k = 0; i < length; i++, t += time_step)
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

void createMnaSystemAC(double f, long int run, long int internal_run) {
  
#if DEBUG
	int l, k, i;
#endif
	int group2_index, group2_counter;
	unsigned int dimension;
	twoTerminalsElement *current1 = NULL;
	cs_ci *Gcomplex = NULL;
	double angle_rad;
	double complex temp_val;
	
	double omega = 2 * M_PI * f;

	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;

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

	if(internal_run == 0) {
	    printf( "\n\nCreating MNA System AC: G[x] = [b] (%dx%d)...\n\n", dimension, dimension );
	}

		for (current1 = head_twoTerminalsElement_list, group2_counter = 0; head_twoTerminalsElement_list != NULL && current1 != NULL;
				current1 = current1->next)
		{
			switch (current1->type)
			{

				case RESISTANCE:

					if (circuit_simulation.matrix_sparsity == SPARSE)
					{
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

                                              
                                             if(current1->isG2) {
                                               group2_index = circuit_simulation.number_of_nodes + group2_counter;
                                              if (current1->positive_terminal)
					      {
						        cs_ci_entry(Gcomplex, current1->positive_terminal - 1, group2_index, 1.0);
						        cs_ci_entry(Gcomplex, group2_index, current1->positive_terminal - 1, 1.0);
					      }

					      if (current1->negative_terminal)
					      {
						        cs_ci_entry(Gcomplex, current1->negative_terminal - 1, group2_index, -1.0);
						        cs_ci_entry(Gcomplex, group2_index, current1->negative_terminal - 1, -1.0);
					      }
                                                cs_ci_entry(Gcomplex, group2_index, group2_index, -current1->value);
                    
                                              group2_counter++;
                                            }
					}
					else {
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

                                                  
                                            if(current1->isG2) {
                                              group2_index = circuit_simulation.number_of_nodes + group2_counter;
                                              if (current1->positive_terminal)
					      {
						       matrix_Gcomplex[(current1->positive_terminal - 1) * dimension + group2_index] += 1;
						       matrix_Gcomplex[group2_index * dimension + (current1->positive_terminal - 1)] += 1;
					      }

					      if (current1->negative_terminal)
					      {
						       matrix_Gcomplex[(current1->negative_terminal - 1) * dimension + group2_index] -= 1;
						       matrix_Gcomplex[group2_index * dimension + (current1->negative_terminal - 1)] -= 1;
					      }
                                               matrix_Gcomplex[group2_index * dimension + group2_index] -= current1->value;
                                              group2_counter++;
                                            }
					}

					break;
					
				case CAPACITOR:
					if (circuit_simulation.matrix_sparsity == SPARSE)
					{
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
					}
					else {
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
					}

                                         if(current1->isG2) {
                                              group2_index = circuit_simulation.number_of_nodes + group2_counter;

                                              if (circuit_simulation.matrix_sparsity == SPARSE) {
						 
                                                       cs_ci_entry(Gcomplex, group2_index, group2_index, 1.0);
                                              }
                                              else {

		                                       matrix_Gcomplex[group2_index * dimension + group2_index] += 1;
                                              }
                                              group2_counter++;
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
				case SHORT_CIRCUIT:
				        group2_index = circuit_simulation.number_of_nodes + group2_counter;
					
					if (circuit_simulation.matrix_sparsity == SPARSE)
					{
					      if (current1->positive_terminal)
					      {						
						      cs_ci_entry (Gcomplex, current1->positive_terminal - 1, group2_index, 1.0);
						      cs_ci_entry (Gcomplex, group2_index, current1->positive_terminal - 1, 1.0);
					      }
					      
					      if (current1->negative_terminal)
					      {	
						      cs_ci_entry (Gcomplex, current1->negative_terminal - 1, group2_index, -1.0);
						      cs_ci_entry (Gcomplex, group2_index, current1->negative_terminal - 1, -1.0);
					      }
					}
					else {
					      if (current1->positive_terminal)
					      {
						      matrix_Gcomplex[(current1->positive_terminal - 1) * dimension + group2_index] += 1;
						      matrix_Gcomplex[group2_index * dimension + (current1->positive_terminal - 1)] += 1;
					      }

					      if (current1->negative_terminal)
					      {
						      matrix_Gcomplex[(current1->negative_terminal - 1) * dimension + group2_index] -= 1;
						      matrix_Gcomplex[group2_index * dimension + (current1->negative_terminal - 1)] -= 1;
					      } 
					}
					
					if(current1->type == VOLTAGE_SOURCE && run == 0) {
					    temp_val = 0;
					    
					    if(current1 -> ac != NULL) {
						angle_rad = current1 -> ac -> phase * (M_PI / 180);
						
						temp_val =  current1 -> ac -> mag * (cos(angle_rad) + sin(angle_rad) * I);
					    }
					    			    
					    vector_b_complex[group2_index] += temp_val;
					}
					
					group2_counter++;
				        break;
				case INDUCTOR:

					group2_index = circuit_simulation.number_of_nodes + group2_counter;

					if (circuit_simulation.matrix_sparsity == SPARSE)
					{
					      if (current1->positive_terminal)
					      {	
						      cs_ci_entry (Gcomplex, current1->positive_terminal - 1, group2_index, 1.0);
						      cs_ci_entry (Gcomplex, group2_index, current1->positive_terminal - 1, 1.0);
					      }
					      
					      if (current1->negative_terminal)
					      {	
						      cs_ci_entry (Gcomplex, current1->negative_terminal - 1, group2_index, -1.0);
						      cs_ci_entry (Gcomplex, group2_index, current1->negative_terminal - 1, -1.0);
					      }
					      
					      cs_ci_entry (Gcomplex, group2_index, group2_index, 0.0 -current1->value * omega * I);
					}
					else {
					      if (current1->positive_terminal)
					      {
						      matrix_Gcomplex[(current1->positive_terminal - 1) * dimension + group2_index] += 1;
						      matrix_Gcomplex[group2_index * dimension + (current1->positive_terminal - 1)] += 1;
					      }

					      if (current1->negative_terminal)
					      {
						      matrix_Gcomplex[(current1->negative_terminal - 1) * dimension + group2_index] -= 1;
						      matrix_Gcomplex[group2_index * dimension + (current1->negative_terminal - 1)] -= 1;
					      }
					      
					      matrix_Gcomplex[group2_index * dimension + group2_index] -= current1->value * omega * I;	
					}

					group2_counter++;

					break;
                                 case VOLTAGE_CONTROLLED_CURRENT_SOURCE:
                                        if (circuit_simulation.matrix_sparsity == SPARSE) {
					     if(current1 -> positive_terminal) {
						    if(current1 -> in_positive_terminal) {
                                                           cs_ci_entry(Gcomplex, current1->positive_terminal - 1, current1 -> in_positive_terminal - 1,  current1->value);
						    }

						     if(current1 -> in_negative_terminal) {
                                                           cs_ci_entry(Gcomplex, current1->positive_terminal - 1, current1 -> in_negative_terminal - 1,  -current1->value);
						    }
						}
		
						if(current1 -> negative_terminal) {
						    if(current1 -> in_positive_terminal) {
                                                         cs_ci_entry(Gcomplex, current1->negative_terminal - 1, current1 -> in_positive_terminal - 1,  -current1->value);
						    }

						     if(current1 -> in_negative_terminal) {
							   cs_ci_entry(Gcomplex, current1->negative_terminal - 1, current1 -> in_negative_terminal - 1,  current1->value);
						    }
						}
                                        }
                                        else {
                                           if(current1 -> positive_terminal) {
						    if(current1 -> in_positive_terminal) {
							 matrix_Gcomplex[(current1->positive_terminal - 1) * dimension + (current1 -> in_positive_terminal - 1)]+=  current1->value;
						    }

						     if(current1 -> in_negative_terminal) {
							  matrix_Gcomplex[(current1->positive_terminal - 1) * dimension + (current1 -> in_negative_terminal - 1)] -=  current1->value;
						    }
						}
		
						if(current1 -> negative_terminal) {
						    if(current1 -> in_positive_terminal) {
							 matrix_Gcomplex[(current1->negative_terminal - 1) * dimension + (current1 -> in_positive_terminal - 1)] -=  current1->value;
						    }

						     if(current1 -> in_negative_terminal) {
							  matrix_Gcomplex[(current1->negative_terminal - 1) * dimension + (current1 -> in_negative_terminal - 1)] +=  current1->value;
						    }
						}

                                        }
                                        break;
                                 case VOLTAGE_CONTROLLED_VOLTAGE_SOURCE:
                                        group2_index = circuit_simulation.number_of_nodes + group2_counter;

                                        
                                        if (circuit_simulation.matrix_sparsity == SPARSE) {
                                              if (current1->positive_terminal)
					      {
						      cs_ci_entry(Gcomplex, current1->positive_terminal - 1, group2_index, 1.0);
						      cs_ci_entry(Gcomplex, group2_index, current1->positive_terminal - 1, 1.0);
					      }

					      if (current1->negative_terminal)
					      {
						      cs_ci_entry(Gcomplex, current1->negative_terminal - 1, group2_index, -1.0);
						      cs_ci_entry(Gcomplex, group2_index, current1->negative_terminal - 1, -1.0);
					      }

                                              
                                               if (current1->in_positive_terminal) {
                                                    cs_ci_entry(Gcomplex, group2_index, current1->in_positive_terminal - 1, -current1->value);                                             
                  
                                               }

                                                if (current1->in_negative_terminal) {                                             
                                                    cs_ci_entry(Gcomplex, group2_index, current1->in_negative_terminal - 1, current1->value);       
                                               }

                                        }
                                        else {
                                             if (current1->positive_terminal) {
                                                 matrix_Gcomplex[(current1->positive_terminal - 1) * dimension + group2_index] += 1;
                                                 matrix_Gcomplex[group2_index * dimension + (current1->positive_terminal - 1)] += 1;
                                             }

                                              if (current1->negative_terminal)
					      {
						      matrix_Gcomplex[(current1->negative_terminal - 1) * dimension + group2_index] -= 1;
						      matrix_Gcomplex[group2_index * dimension + (current1->negative_terminal - 1)] -= 1;
					      }

                                               if (current1->in_positive_terminal) {                                             
                                                 matrix_Gcomplex[group2_index * dimension + (current1->in_positive_terminal - 1)] -=  current1->value;
                                               }

                                                if (current1->in_negative_terminal) {                                             
                                                 matrix_Gcomplex[group2_index * dimension + (current1->in_negative_terminal - 1)] =  current1->value;
                                               }
                                        }
                                        group2_counter++;
                                        break;
				default:
					break;

			}

		}

		if (circuit_simulation.matrix_sparsity == SPARSE)
		{
		      G2complex = cs_ci_compress( Gcomplex );
		      cs_ci_spfree( Gcomplex );
		      cs_ci_dupl( G2complex );
		}


#if DEBUG
		printf("\nMNA System AC\n~~~~~~~~~~~~~\n\n");
		printf("\nMatrix G:\n\n");
		for(k = 0; k < dimension; k++)
		{
			for(l = 0; l < dimension; l++)
			{
			        if (circuit_simulation.matrix_sparsity == SPARSE)
				{
				      temp_val = getElementAtComplex(G2complex, k, l);
				      printf("(%7.3lf %7.3lfi) ", creal(temp_val), cimag(temp_val));
				}
				else {
				      printf("(%7.3lf %7.3lfi) ", creal(matrix_Gcomplex[k * dimension + l]), cimag(matrix_Gcomplex[k * dimension + l]));			  
				}
					       
				if(l == circuit_simulation.number_of_nodes - 1)
				{
					printf("   |");
				}
			}
			printf("\n");

			if(k == circuit_simulation.number_of_nodes - 1)
			{
				for(i = 0; i < dimension * 20; i++)
				{
					printf("_");
				}
				printf("\n\n");
			}

		}
#endif

#if DEBUG
	printf("\n\n\nVector b:\n\n");
	for(k = 0; k < dimension; k++)
	{
	        printf("(%7.3lf %7.3lfi) ", creal(vector_b_complex[k]), cimag(vector_b_complex[k]));
		printf("\n");
		if(k == circuit_simulation.number_of_nodes - 1)
		{
			printf("__________________\n\n");
		}
	}

	printf("\n\n");
#endif

  
}

