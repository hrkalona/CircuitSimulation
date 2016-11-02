#include "circuit_simulation.h"

void LUDecomposition(void)
{
#if DEBUG
	int l;
#endif
	int k, i, m, j;
	unsigned int dimension;

	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;

	if (circuit_simulation.matrix_sparsity == SPARSE)
	{
		printf( "\n\nLU Decomposition...\n\n" );
		
		S = cs_di_sqr( 2, G2, 0 );
		N = cs_di_lu( G2, S, 1 );
		cs_di_spfree( G2 );

		if (N == NULL)
		{
			printf( "Error, the matrix cannot be inverted.\n\n" );
			printf( "Terminating.\n" );
			exit( -1 );
		}

	}
	else
	{
		transposition = ( int * ) malloc( dimension * sizeof(int) );

		if (transposition == NULL)
		{
			printf( "Could not allocate matrices.\n" );
			printf( "Terminating.\n" );
			exit( -1 );
		}

		for (i = 0; i < dimension; i++)
		{
			transposition[i] = i;
		}

		printf( "\n\nLU Decomposition...\n\n" );

		for (k = 0; k < dimension - 1; k++)
		{
			for (m = i = k; i < dimension; i++)
			{
				if (fabs( matrix_G[i * dimension + k] ) > fabs( matrix_G[m * dimension + k] ))
				{
					m = i;
				}
			}

			interchange( m, k );

			if (fabs( matrix_G[k * dimension + k] ) == 0)
			{
				printf( "Error, the matrix cannot be inverted.\n\n" );
				printf( "Terminating.\n" );
				exit( -1 );
			}

			for (i = k + 1; i < dimension; i++)
			{
				matrix_G[i * dimension + k] /= matrix_G[k * dimension + k];
				for (j = k + 1; j < dimension; j++)
				{
					matrix_G[i * dimension + j] += -matrix_G[i * dimension + k] * matrix_G[k * dimension + j];
				}
			}
		}

	}
	
#if DEBUG
		printf("\nLU Decomposition\n~~~~~~~~~~~~~~~~~\n\n");
		printf("\nDecomposed Matrix:\n\n");
		for(k = 0; k < dimension; k++)
		{
			for(l = 0; l < dimension; l++)
			{
				if(l == k)
				{
					printf("  \\\\");
				}
				
				if (circuit_simulation.matrix_sparsity == SPARSE)
				{
				      if(l >= k)
				      {
					      printf("%12lf ", getElementAt(N -> U, k, l));
				      }
				      else
				      {
					      printf("%12lf ", getElementAt(N -> L, k, l));
				      }
				}
				else {
				      printf("%12lf ", matrix_G[k * dimension + l]);  
				}
			}
			printf("\n\n");
		}

		printf("\n\n");

		printf("\nLine Transposition:\n");
		for(k = 0; k < dimension; k++)
		{		
		      if (circuit_simulation.matrix_sparsity == SPARSE)
		      {
			    printf("line %d contains line %d\n", N -> pinv[k], k);
		      }
		      else {
			    printf("line %d contains line %d\n", k, transposition[k]);
		      }
		}
		printf("\n\n");
#endif

}


void LUDecompositionComplex(long int run)
{
	int k, i, m, j;
	unsigned int dimension;
#if DEBUG
	int l;
	double complex temp_val;
#endif

	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;

	if (circuit_simulation.matrix_sparsity == SPARSE)
	{
		if(run == 0) {
	            printf( "\n\nLU Decomposition...\n\n" );
		}
		Scomplex = cs_ci_sqr( 2, G2complex, 0 );
		Ncomplex = cs_ci_lu( G2complex, Scomplex, 1 );
		cs_ci_spfree( G2complex );

		if (Ncomplex == NULL)
		{
			printf( "Error, the matrix cannot be inverted.\n\n" );
			printf( "Terminating.\n" );
			exit( -1 );
		}
	}
	else
	{
		transposition = ( int * ) malloc( dimension * sizeof(int) );

		if (transposition == NULL)
		{
			printf( "Could not allocate matrices.\n" );
			printf( "Terminating.\n" );
			exit( -1 );
		}

		for (i = 0; i < dimension; i++)
		{
			transposition[i] = i;
		}

		if(run == 0) {
		      printf( "\n\nLU Decomposition...\n\n" );
		}

		for (k = 0; k < dimension - 1; k++)
		{
			for (m = i = k; i < dimension; i++)
			{
				if (cabs( matrix_Gcomplex[i * dimension + k] ) > cabs( matrix_Gcomplex[m * dimension + k] ))
				{
					m = i;
				}
			}

			interchangeComplex( m, k );

			if (cabs( matrix_Gcomplex[k * dimension + k] ) == 0)
			{
				printf( "Error, the matrix cannot be inverted.\n\n" );
				printf( "Terminating.\n" );
				exit( -1 );
			}

			for (i = k + 1; i < dimension; i++)
			{
				matrix_Gcomplex[i * dimension + k] /= matrix_Gcomplex[k * dimension + k];
				for (j = k + 1; j < dimension; j++)
				{
					matrix_Gcomplex[i * dimension + j] += -matrix_Gcomplex[i * dimension + k] * matrix_Gcomplex[k * dimension + j];
				}
			}
		}
	}
	
	
#if DEBUG
		printf("\nLU Decomposition\n~~~~~~~~~~~~~~~~~\n\n");
		printf("\nDecomposed Matrix:\n\n");
		for(k = 0; k < dimension; k++)
		{
			for(l = 0; l < dimension; l++)
			{
				if(l == k)
				{
					printf("  \\\\");
				}
				if (circuit_simulation.matrix_sparsity == SPARSE)
				{
				      if(l >= k)
				      {
					  temp_val = getElementAtComplex(Ncomplex -> U, k, l);	
					  printf("(%7.3lf %7.3lfi) ", creal(temp_val), cimag(temp_val));
				      }
				      else
				      {
					  temp_val = getElementAtComplex(Ncomplex -> L, k, l);
					  printf("(%7.3lf %7.3lfi) ", creal(temp_val), cimag(temp_val));
				      }
				}
				else {
				      printf("(%7.3lf %7.3lfi) ", creal(matrix_Gcomplex[k * dimension + l]), cimag(matrix_Gcomplex[k * dimension + l]));
				}
			}
			printf("\n\n");
		}

		printf("\n\n");

		printf("\nLine Transposition:\n");
		for(k = 0; k < dimension; k++)
		{
		      if (circuit_simulation.matrix_sparsity == SPARSE)
		      {
			    printf("line %d contains line %d\n", Ncomplex -> pinv[k], k);
		      }
		      else {
			    printf("line %d contains line %d\n", k, transposition[k]);
		      }
		}
		printf("\n\n");
#endif

}

void CholeskyDecomposition(void)
{
	int k, i, j;
	unsigned int dimension;
	double sum, temp;
#if DEBUG
	int l;
#endif

	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;

	if (circuit_simulation.matrix_sparsity == SPARSE)
	{
		printf( "\n\nCholesky Decomposition...\n\n" );
		S = cs_di_schol( 1, G2 );
		N = cs_di_chol( G2, S );
		cs_di_spfree( G2 );

		if (N == NULL)
		{
			printf( "Error, the matrix must be symmetric & positive definite.\n\n" );
			printf( "Terminating.\n" );
			exit( -1 );
		}

	}
	else
	{
		printf( "\n\nCholesky Decomposition...\n\n" );

		for (k = 0; k < dimension; k++)
		{
			for (sum = 0, j = 0; j < k; j++)
			{
			      if (matrix_G[k * dimension + j] != matrix_G[j * dimension + k])
			      {
					printf( "Error, the matrix must be symmetric & positive definite.\n\n" );
					printf( "Terminating.\n" );
					exit( -1 );
			      }
			      
			      sum += matrix_G[k * dimension + j] * matrix_G[k * dimension + j];				
			}

			if ((temp = matrix_G[k * dimension + k] - sum) < 0)
			{
				printf( "Error, the matrix must be symmetric & positive definite.\n\n" );
				printf( "Terminating.\n" );
				exit( -1 );
			}
			matrix_G[k * dimension + k] = sqrt( temp );

			for (i = k + 1; i < dimension; i++)
			{
				for (sum = 0, j = 0; j < k; j++)
				{
					sum += matrix_G[i * dimension + j] * matrix_G[k * dimension + j];
				}
				matrix_G[k * dimension + i] = matrix_G[i * dimension + k] = (matrix_G[k * dimension + i] - sum) / matrix_G[k * dimension + k];
			}
		}

	}
	
#if DEBUG
		printf("\nCholesky Decomposition\n~~~~~~~~~~~~~~~~~~~~~~~\n\n");
		printf("\nDecomposed Matrix:\n\n");
		for(k = 0; k < dimension; k++)
		{
			for(l = 0; l < dimension; l++)
			{
				if(l == k)
				{
					printf("  \\\\");
				}
				
				if (circuit_simulation.matrix_sparsity == SPARSE)
				{
				      if(l <= k)
				      {
					      printf("%12lf ", getElementAt(N -> L, k, l));
				      }
				      else
				      {
					      printf("%12lf ", getElementAt(N -> L, l, k));
				      }
				}
				else {
				      printf("%12lf ", matrix_G[k * dimension + l]);
				}
				
				if(l == k)
				{
					printf("  \\\\");
				}

			}
			printf("\n\n");
		}

		printf("\n\n");
#endif

}

void CholeskyDecompositionComplex(long int run)
{
	int k, i, j;
	unsigned int dimension;
	double sum, temp;
	double complex sum2;
#if DEBUG
	int l;
	double complex temp_val;
#endif

	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;

	if (circuit_simulation.matrix_sparsity == SPARSE)
	{
		if(run == 0) {
	             printf( "\n\nCholesky Decomposition...\n\n" );
		}
		Scomplex = cs_ci_schol( 1, G2complex );
		Ncomplex = cs_ci_chol( G2complex, Scomplex );
		cs_ci_spfree( G2complex );

		if (Ncomplex == NULL)
		{
			printf( "Error, the matrix must be hermitian & positive definite.\n\n" );
			printf( "Terminating.\n" );
			exit( -1 );
		}
	}
	else
	{
	        if(run == 0) {
		    printf( "\n\nCholesky Decomposition...\n\n" );
                }

		for (k = 0; k < dimension; k++)
		{
			for (sum = 0, j = 0; j < k; j++)
			{
			      if (matrix_Gcomplex[k * dimension + j] != conj(matrix_Gcomplex[j * dimension + k]))
			      {
					printf( "Error, the matrix must be hermitian & positive definite.\n\n" );
					printf( "Terminating.\n" );
					exit( -1 );
			      }
			      
			      sum += creal(conj(matrix_Gcomplex[k * dimension + j]) * matrix_Gcomplex[k * dimension + j]);		
			}
			
			if(cimag(matrix_Gcomplex[k * dimension + k]) != 0) {
			      printf( "Error, the matrix must be hermitian & positive definite.\n\n" );
			      printf( "Terminating.\n" );
			      exit( -1 );
			}

			if ((temp = creal(matrix_Gcomplex[k * dimension + k]) - sum) < 0)
			{
				printf( "Error, the matrix must be hermitian & positive definite.\n\n" );
				printf( "Terminating.\n" );
				exit( -1 );
			}
			matrix_Gcomplex[k * dimension + k] = sqrt( temp );

			for (i = k + 1; i < dimension; i++)
			{
				for (sum2 = 0, j = 0; j < k; j++)
				{
					sum2 += conj(matrix_Gcomplex[i * dimension + j]) * matrix_Gcomplex[k * dimension + j];
				}
				matrix_Gcomplex[k * dimension + i] = matrix_Gcomplex[i * dimension + k] = (matrix_Gcomplex[k * dimension + i] - sum2) / matrix_Gcomplex[k * dimension + k];
			}
		}
	}
	
	#if DEBUG
		printf("\nCholesky Decomposition\n~~~~~~~~~~~~~~~~~~~~~~~\n\n");
		printf("\nDecomposed Matrix:\n\n");
		for(k = 0; k < dimension; k++)
		{
			for(l = 0; l < dimension; l++)
			{
				if(l == k)
				{
					printf("  \\\\");
				}
				
				if (circuit_simulation.matrix_sparsity == SPARSE)
				{
				      if(l <= k)
				      {
					    temp_val = getElementAtComplex(Ncomplex -> L, k, l);
					    printf("(%7.3lf %7.3lfi) ", creal(temp_val), cimag(temp_val));
				      }
				      else
				      {
					    temp_val = getElementAtComplex(Ncomplex -> L, l, k);
					    printf("(%7.3lf %7.3lfi) ", creal(temp_val), cimag(temp_val));
				      }
				}
				else {
				     printf("(%7.3lf %7.3lfi) ", creal(matrix_Gcomplex[k * dimension + l]), cimag(matrix_Gcomplex[k * dimension + l]));
				}
				
				if(l == k)
				{
					printf("  \\\\");
				}

			}
			printf("\n\n");
		}

		printf("\n\n");
#endif

}

void interchange(int from, int to)
{
	int j, temp3;
	unsigned int dimension;
	double temp;

	if (from == to)
	{
		return;
	}

	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;

#pragma omp parallel for default(shared) private(j, temp)
	for (j = 0; j < dimension; j++)
	{
		temp = matrix_G[from * dimension + j];
		matrix_G[from * dimension + j] = matrix_G[to * dimension + j];
		matrix_G[to * dimension + j] = temp;
	}

	temp3 = transposition[from];
	transposition[from] = transposition[to];
	transposition[to] = temp3;

}

void interchangeComplex(int from, int to)
{
	int j, temp3;
	unsigned int dimension;
	double complex temp;

	if (from == to)
	{
		return;
	}

	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;

#pragma omp parallel for default(shared) private(j, temp)
	for (j = 0; j < dimension; j++)
	{
		temp = matrix_Gcomplex[from * dimension + j];
		matrix_Gcomplex[from * dimension + j] = matrix_Gcomplex[to * dimension + j];
		matrix_Gcomplex[to * dimension + j] = temp;
	}

	temp3 = transposition[from];
	transposition[from] = transposition[to];
	transposition[to] = temp3;

}

void forward_substitution(void)
{
	int k, j;
	double a, *vector_temp = NULL;
	unsigned int dimension;

	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;

	vector_y = ( double * ) calloc( dimension, sizeof(double) );
	vector_temp = ( double * ) malloc( dimension * sizeof(double) );

	if (vector_y == NULL || vector_temp == NULL)
	{
		printf( "Could not allocate matrices.\n" );
		printf( "Terminating.\n" );
		exit( -1 );
	}

	for (k = 0; k < dimension; k++)
	{
		if (circuit_simulation.matrix_type == NONSPD)
		{
			vector_temp[k] = vector_b[transposition[k]];
		}
		else
		{
			vector_temp[k] = vector_b[k];
		}
	}

	for (k = 0; k < dimension; k++)
	{
		a = vector_temp[k];
#pragma omp parallel for default(shared) private(j) reduction(+:a)
		for (j = 0; j < k; j++)
		{
			a -= matrix_G[k * dimension + j] * vector_y[j];
		}
		vector_y[k] = a / (circuit_simulation.matrix_type == NONSPD ? 1 : matrix_G[k * dimension + k]);
	}

#if DEBUG
	printf("\nVector y:\n\n");
	for(k = 0; k < dimension; k++)
	{
		printf("%12lf\n", vector_y[k]);
	}

	printf("\n\n");
#endif

	free( vector_temp );

}

void forward_substitution_Complex(void)
{
        unsigned int dimension;
        int k, j;
	double complex a, *vector_temp_complex = NULL;

	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;

	vector_y_complex = ( double complex * ) calloc( dimension, sizeof(double complex) );
	vector_temp_complex = ( double complex * ) malloc( dimension * sizeof(double complex) );

	if (vector_y_complex == NULL || vector_temp_complex == NULL)
	{
		printf( "Could not allocate matrices.\n" );
		printf( "Terminating.\n" );
		exit( -1 );
	}

	for (k = 0; k < dimension; k++)
	{
		if (circuit_simulation.matrix_type == NONSPD)
		{
			vector_temp_complex[k] = vector_b_complex[transposition[k]];
		}
		else
		{
			vector_temp_complex[k] = vector_b_complex[k];
		}
	}

	for (k = 0; k < dimension; k++)
	{
		a = vector_temp_complex[k];
#pragma omp parallel for default(shared) private(j) reduction(+:a)
		for (j = 0; j < k; j++)
		{
			a -= matrix_Gcomplex[k * dimension + j] * vector_y_complex[j];
		}
		vector_y_complex[k] = a / (circuit_simulation.matrix_type == NONSPD ? 1 : matrix_Gcomplex[k * dimension + k]);
	}

#if DEBUG
	printf("\nVector y:\n\n");
	for(k = 0; k < dimension; k++)
	{
		printf("(%7.3lf %7.3lfi)\n", creal(vector_y_complex[k]), cimag(vector_y_complex[k]));
	}

	printf("\n\n");
#endif

	free( vector_temp_complex );

}

void backward_substitution(void)
{
	unsigned int dimension;
	int k, j;
	double a;

	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;

	memset( vector_x, 0, dimension * sizeof(double) );

	for (k = dimension - 1; k >= 0; k--)
	{
		a = vector_y[k];
#pragma omp parallel for default(shared) private(j) reduction(+:a)
		for (j = k + 1; j < dimension; j++)
		{
			a -= matrix_G[k * dimension + j] * vector_x[j];
		}
		vector_x[k] = a / matrix_G[k * dimension + k];
	}


	free( vector_y );

}

void backward_substitution_Complex(void)
{
	unsigned int dimension;
	int k, j;
	double complex a;

	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;

	memset( vector_x_complex, 0, dimension * sizeof(double complex) );

	for (k = dimension - 1; k >= 0; k--)
	{
		a = vector_y_complex[k];
#pragma omp parallel for default(shared) private(j) reduction(+:a)
		for (j = k + 1; j < dimension; j++)
		{
			a -= matrix_Gcomplex[k * dimension + j] * vector_x_complex[j];
		}
		vector_x_complex[k] = a / matrix_Gcomplex[k * dimension + k];
	}

	free( vector_y_complex );

}

void solve_direct_methods()
{
	unsigned int dimension;
#if DEBUG
	int k;
#endif
	double *vector_temp = NULL;

	//printf("\n\nSolving (Forward/Backward) ...\n\n");
	
	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;

	if (circuit_simulation.matrix_sparsity == SPARSE)
	{
		vector_temp = ( double * ) calloc( dimension, sizeof(double) );

		if (vector_temp == NULL)
		{
			printf( "Could not allocate matrices.\n" );
			printf( "Terminating.\n" );
			exit( -1 );
		}

		memcpy( vector_x, vector_b, dimension * sizeof(double) );

		if (circuit_simulation.matrix_type == NONSPD)
		{ /*Forward Backward LU*/
			cs_di_ipvec( N->pinv, vector_x, vector_temp, dimension );
			cs_di_lsolve( N->L, vector_temp );
			cs_di_usolve( N->U, vector_temp );
			cs_di_ipvec( S->q, vector_temp, vector_x, dimension );
		}
		else
		{/*Forward Backward Cholesky*/
			cs_di_ipvec( S->pinv, vector_x, vector_temp, dimension );
			cs_di_lsolve( N->L, vector_temp );
			cs_di_ltsolve( N->L, vector_temp );
			cs_di_pvec( S->pinv, vector_temp, vector_x, dimension );
		}

		free( vector_temp );
	}
	else
	{
		forward_substitution();
		backward_substitution();
	}
	
	#if DEBUG
		printf("\nVector x:\n\n");
		for(k = 0; k < dimension; k++)
		{
			printf("%12lf\n", vector_x[k]);
		}

		printf("\n\n");
        #endif

}


void solve_direct_methods_Complex()
{
	unsigned int dimension;
#if DEBUG
	int k;
#endif
	double complex *vector_temp_complex = NULL;
	
	dimension = circuit_simulation.number_of_nodes + circuit_simulation.group2_elements;

	//printf("\n\nSolving (Forward/Backward) ...\n\n");

	if (circuit_simulation.matrix_sparsity == SPARSE)
	{
		vector_temp_complex = ( double complex * ) calloc( dimension, sizeof(double complex) );

		if (vector_temp_complex == NULL)
		{
			printf( "Could not allocate matrices.\n" );
			printf( "Terminating.\n" );
			exit( -1 );
		}

		memcpy( vector_x_complex, vector_b_complex, dimension * sizeof(double complex) );

		if (circuit_simulation.matrix_type == NONSPD)
		{ /*Forward Backward LU*/
			cs_ci_ipvec( Ncomplex->pinv, vector_x_complex, vector_temp_complex, dimension );
			cs_ci_lsolve( Ncomplex->L, vector_temp_complex );
			cs_ci_usolve( Ncomplex->U, vector_temp_complex );
			cs_ci_ipvec( Scomplex->q, vector_temp_complex, vector_x_complex, dimension );
		}
		else
		{/*Forward Backward Cholesky*/
			cs_ci_ipvec( Scomplex->pinv, vector_x_complex, vector_temp_complex, dimension );
			cs_ci_lsolve( Ncomplex->L, vector_temp_complex );
			cs_ci_ltsolve( Ncomplex->L, vector_temp_complex );
			cs_ci_pvec( Scomplex->pinv, vector_temp_complex, vector_x_complex, dimension );
		}

		free( vector_temp_complex );
	}
	else
	{
		forward_substitution_Complex();
		backward_substitution_Complex();
	}
	
	#if DEBUG
	printf("\nVector x:\n\n");
	for(k = 0; k < dimension; k++)
	{
		printf("(%7.3lf %7.3lfi)\n", creal(vector_x_complex[k]), cimag(vector_x_complex[k]));
	}

	printf("\n\n");
        #endif

}
