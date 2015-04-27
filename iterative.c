#include "circuit_simulation.h"


void createInversePreconditioner(int length) {
  int i;
  double temp;
  
    inv_preconditioner = (double *) malloc(length * sizeof(double));
    
    if(inv_preconditioner == NULL) {
        printf("Could not allocate matrices.\n");
	printf("Terminating.\n");
	exit(-1);
    }
  
    if(circuit_simulation.matrix_sparsity == SPARSE) {
	 for(i = 0; i < length; i++) {
	    temp = getElementAt(G2, i, i); // get diagonal element
	    inv_preconditioner[i] = 1 / (temp == 0 ? 1 : temp); // 1 / diagonal element
	} 
    }
    else {
	for(i = 0; i < length; i++) {
	    temp = matrix_G[i * length + i]; // get diagonal element
	    inv_preconditioner[i] = 1 / (temp == 0 ? 1 : temp); // 1 / diagonal element
	}    
    }
    
}

void createInversePreconditionerComplex(int length) {
  int i;
  double complex temp;
  
    inv_preconditioner_complex = (double complex *) malloc(length * sizeof(double complex));
    
    if(inv_preconditioner_complex == NULL) {
        printf("Could not allocate matrices.\n");
	printf("Terminating.\n");
	exit(-1);
    }
  
    if(circuit_simulation.matrix_sparsity == SPARSE) {
	 for(i = 0; i < length; i++) {
	    temp = getElementAtComplex(G2complex, i, i); // get diagonal element
	    inv_preconditioner_complex[i] = 1 / (temp == 0 ? 1 : temp); // 1 / diagonal element
	} 
    }
    else {
	for(i = 0; i < length; i++) {
	    temp = matrix_Gcomplex[i * length + i]; // get diagonal element
	    inv_preconditioner_complex[i] = 1 / (temp == 0 ? 1 : temp); // 1 / diagonal element
	}    
    }
    
}


inline void solve_diagonal_system(double *vector1, double *vector2, double *result, int length) {
  int i;
  
    #pragma omp parallel for default(shared) private(i)
    for(i = 0; i < length; i++) {
        result[i] = vector1[i] * vector2[i];
    }
    
}

inline void solve_diagonal_system_Complex(double complex *vector1, double complex *vector2, double complex *result, int length, int hermitian) {
  int i;
  
    if(hermitian) {
        #pragma omp parallel for default(shared) private(i)
	for(i = 0; i < length; i++) {
	    result[i] = conj(vector1[i]) * vector2[i];
	}
    }
    else {
	#pragma omp parallel for default(shared) private(i)
	for(i = 0; i < length; i++) {
	    result[i] = vector1[i] * vector2[i];
	}
    }
    
}


void ConjugateGradient(int zero_x) {
  int dimension, iteration = 0, k;
  double temp = 0, rho = 0, rho1 = 0, beta = 0, alpha = 0, temp3 = 0, omega = 0;
  double *vector_r = NULL, *vector_z = NULL, *vector_p = NULL, *vector_q = NULL;
  int max_iterations;
  
    dimension = number_of_nodes + group2_elements;
    
    max_iterations = dimension;
 
    vector_r = (double *) calloc(dimension, sizeof(double));
    vector_z = (double *) malloc(dimension * sizeof(double));
    
    vector_p = (double *) malloc(dimension * sizeof(double));
    vector_q = (double *) malloc(dimension * sizeof(double));
    
  
    if(vector_r == NULL || vector_z == NULL || vector_p == NULL || vector_q == NULL) {
        printf("Could not allocate matrices.\n");
	printf("Terminating.\n");
	exit(-1);
    }
      
    //printf("\n\nSolving (Conjugate Gradient) ...\n\n");

    if(zero_x) {
        memset(vector_x, 0, dimension * sizeof(double)); // x = 0
        memcpy(vector_r, vector_b, dimension * sizeof(double)); // r = b
    }
    else {
	if(circuit_simulation.matrix_sparsity == SPARSE) {
	    cs_di_gaxpy(G2, vector_x, vector_r); // r = G * x
	}
	else {
	    cblas_dgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, 1.0, matrix_G, dimension, vector_x, 1, 0.0, vector_r, 1); // r = G * x
	}
	
	cblas_dscal(dimension, -1.0, vector_r, 1); // r = -r
	cblas_daxpy(dimension, 1.0, vector_b, 1, vector_r, 1); // r = b + r  <=>  r = b - G * x
    }
    
    temp = cblas_ddot(dimension, vector_b, 1, vector_b, 1); // bT * b
    temp = circuit_simulation.iterative_methods.itol * circuit_simulation.iterative_methods.itol *  (temp == 0 ? 1 : temp); // (bT * b) * itol * itol
    
    
    // ||rT * r|| / ||bT * b|| > itol <=> 
    // (||rT * r|| / ||bT * b||)^2 > itol^2 <=> 
    // (rT * r / bT * b) > itol^2 <=>
    // rT * r > (bT * b) * itol * itol
    while((temp3 = cblas_ddot(dimension, vector_r, 1, vector_r, 1)) > temp && iteration < max_iterations) {
        iteration++; 
	
        solve_diagonal_system(inv_preconditioner, vector_r, vector_z, dimension); // z = inv(M) * r
      
        rho = cblas_ddot(dimension, vector_r, 1, vector_z, 1); // rho = rT * z
	
	if(iteration == 1) {
	    memcpy(vector_p, vector_z, dimension * sizeof(double)); // p = z
	}
	else {
	    beta = rho / rho1;
	    
	    cblas_dscal(dimension, beta, vector_p, 1); // p = beta * p
	    cblas_daxpy(dimension, 1.0, vector_z, 1, vector_p, 1); // p = z + p <=> p = z + beta * p
	}
	
	rho1 = rho;
	
	if(circuit_simulation.matrix_sparsity == SPARSE) {
	    memset(vector_q, 0, dimension * sizeof(double));
	    cs_di_gaxpy(G2, vector_p, vector_q); // q = G * p
        }
        else {
	    cblas_dgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, 1.0, matrix_G, dimension, vector_p, 1, 0.0, vector_q, 1); // q = G * p
	}
	
	omega = cblas_ddot(dimension, vector_p, 1, vector_q, 1); // omega = pT * q
	
	alpha = rho / omega;
	
	cblas_daxpy(dimension, alpha, vector_p, 1, vector_x, 1); // x = x + alpha * p
	
	cblas_daxpy(dimension, -alpha, vector_q, 1, vector_r, 1); // r = r - alpha * q

    }
    

    /*if(iteration == dimension && temp3 > temp) {
        printf("Conjugate Gradient method failed to converge.\n");
	printf("Terminating.\n");
	exit(-1);
    }*/
    
    #if DEBUG
    printf("\nConjugate Gradient Result\n~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    printf("\nVector x:\n\n");
    for(k = 0; k < dimension; k++) {
        printf("%12lf\n", vector_x[k]);
    }
    
    printf("\n\n");
    #endif
    
    
    free(vector_r);
    free(vector_z);
    free(vector_p);
    free(vector_q);
  
}

void ConjugateGradientComplex(int zero_x) {
  int dimension, iteration = 0, k;
  double complex temp = 0, rho = 0, rho1 = 0, beta = 0, alpha = 0, temp3 = 0, omega = 0;
  double complex *vector_r_complex = NULL, *vector_z_complex = NULL, *vector_p_complex = NULL, *vector_q_complex = NULL;
  int max_iterations;
  double complex var0 = 0.0, var1 = 1.0, varminus1 = -1.0, alpha_minus;
  
    dimension = number_of_nodes + group2_elements;
    
    max_iterations = dimension;
 
    vector_r_complex = (double complex *) calloc(dimension, sizeof(double complex));
    vector_z_complex = (double complex *) malloc(dimension * sizeof(double complex));
    
    vector_p_complex = (double complex *) malloc(dimension * sizeof(double complex));
    vector_q_complex = (double complex *) malloc(dimension * sizeof(double complex));
    
  
    if(vector_r_complex == NULL || vector_z_complex == NULL || vector_p_complex == NULL || vector_q_complex == NULL) {
        printf("Could not allocate matrices.\n");
	printf("Terminating.\n");
	exit(-1);
    }
    
    //printf("\n\nSolving (Conjugate Gradient) ...\n\n");
    
    if(zero_x) {
        memset(vector_x_complex, 0, dimension * sizeof(double complex)); // x = 0
        memcpy(vector_r_complex, vector_b_complex, dimension * sizeof(double complex)); // r = b
    }
    else {
	if(circuit_simulation.matrix_sparsity == SPARSE) {
	    cs_ci_gaxpy(G2complex, vector_x_complex, vector_r_complex); // r = G * x
	}
	else {
	    cblas_zgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, &var1, matrix_Gcomplex, dimension, vector_x_complex, 1, &var0, vector_r_complex, 1); // r = G * x
	}
	
	cblas_zscal(dimension, &varminus1, vector_r_complex, 1); // r = -r
	cblas_zaxpy(dimension, &var1, vector_b_complex, 1, vector_r_complex, 1); // r = b + r  <=>  r = b - G * x
    }
    
    cblas_zdotc_sub(dimension, vector_b_complex, 1, vector_b_complex, 1, &temp); // bH * b
    temp = circuit_simulation.iterative_methods.itol * circuit_simulation.iterative_methods.itol *  (temp == 0 ? 1 : temp); // (bH * b) * itol * itol
    
    
    // ||rH * r|| / ||bH * b|| > itol <=> 
    // (||rH * r|| / ||bH * b||)^2 > itol^2 <=> 
    // (rH * r / bH * b) > itol^2 <=>
    // rH * r > (bH * b) * itol * itol
    cblas_zdotc_sub(dimension, vector_r_complex, 1, vector_r_complex, 1, &temp3); // rH * r
    
    while(creal(temp3) > creal(temp) && iteration < max_iterations) {
        iteration++; 
	
        solve_diagonal_system_Complex(inv_preconditioner_complex, vector_r_complex, vector_z_complex, dimension, 0); // z = inv(M) * r
      
        cblas_zdotc_sub(dimension, vector_r_complex, 1, vector_z_complex, 1, &rho); // rho = rH * z
	
	if(iteration == 1) {
	    memcpy(vector_p_complex, vector_z_complex, dimension * sizeof(double complex)); // p = z
	}
	else {
	    beta = rho / rho1;
	    
	    cblas_zscal(dimension, &beta, vector_p_complex, 1); // p = beta * p
	    cblas_zaxpy(dimension, &var1, vector_z_complex, 1, vector_p_complex, 1); // p = z + p <=> p = z + beta * p
	}
	
	rho1 = rho;
	
	if(circuit_simulation.matrix_sparsity == SPARSE) {
	    memset(vector_q_complex, 0, dimension * sizeof(double complex));
	    cs_ci_gaxpy(G2complex, vector_p_complex, vector_q_complex); // q = G * p
        }
        else {
	    cblas_zgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, &var1, matrix_Gcomplex, dimension, vector_p_complex, 1, &var0, vector_q_complex, 1); // q = G * p
	}
	
	cblas_zdotc_sub(dimension, vector_p_complex, 1, vector_q_complex, 1, &omega); // omega = pH * q
	
	alpha = rho / omega;
	
	cblas_zaxpy(dimension, &alpha, vector_p_complex, 1, vector_x_complex, 1); // x = x + alpha * p
	
	alpha_minus = -alpha;
	
	cblas_zaxpy(dimension, &alpha_minus, vector_q_complex, 1, vector_r_complex, 1); // r = r - alpha * q

	cblas_zdotc_sub(dimension, vector_r_complex, 1, vector_r_complex, 1, &temp3); // rH * r
    }
    

    /*if(iteration == dimension && temp3 > temp) {
        printf("Conjugate Gradient method failed to converge.\n");
	printf("Terminating.\n");
	exit(-1);
    }*/
    
    #if DEBUG
    printf("\nConjugate Gradient Result\n~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    printf("\nVector x:\n\n");
    for(k = 0; k < dimension; k++) {
        printf("(%7.3lf %7.3lfi)\n", creal(vector_x_complex[k]), cimag(vector_x_complex[k]));
    }
    
    printf("\n\n");
    #endif
    
    
    free(vector_r_complex);
    free(vector_z_complex);
    free(vector_p_complex);
    free(vector_q_complex);
  
}



void BiConjugateGradient(int zero_x) {
  int dimension, iteration = 0, k;
  double temp = 0, rho = 0, rho1 = 0, beta = 0, alpha = 0, temp3 = 0, omega = 0;
  double *vector_r = NULL, *vector_r2 = NULL, *vector_z = NULL, *vector_z2 = NULL, *vector_p = NULL, *vector_p2 = NULL, *vector_q = NULL, *vector_q2 = NULL;
  int max_iterations;
  
    dimension = number_of_nodes + group2_elements;    
    
    max_iterations = 2 * dimension;
 
    vector_r = (double *) calloc(dimension, sizeof(double));
    vector_r2 = (double *) malloc(dimension * sizeof(double));
    vector_z = (double *) malloc(dimension * sizeof(double));
    vector_z2 = (double *) malloc(dimension * sizeof(double));
    vector_p = (double *) malloc(dimension * sizeof(double));
    vector_p2 = (double *) malloc(dimension * sizeof(double));
    vector_q = (double *) malloc(dimension * sizeof(double));
    vector_q2 = (double *) malloc(dimension * sizeof(double));
    
      
    if(vector_r == NULL || vector_r2 == NULL || vector_z == NULL || vector_z2 == NULL || vector_p == NULL || vector_p2 == NULL || vector_q == NULL || vector_q2 == NULL) {
        printf("Could not allocate matrices.\n");
	printf("Terminating.\n");
	exit(-1);
    }
    
    //printf("\n\nSolving (Bi-Conjugate Gradient) ...\n\n");
    
    if(zero_x) {
        memset(vector_x, 0, dimension * sizeof(double)); // x = 0
        memcpy(vector_r, vector_b, dimension * sizeof(double)); // r = b
    }
    else {
	if(circuit_simulation.matrix_sparsity == SPARSE) {
	    cs_di_gaxpy(G2, vector_x, vector_r); // r = G * x
	}
	else {
	    cblas_dgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, 1.0, matrix_G, dimension, vector_x, 1, 0.0, vector_r, 1); // r = G * x
	}
    
	cblas_dscal(dimension, -1.0, vector_r, 1); // r = -r
	cblas_daxpy(dimension, 1.0, vector_b, 1, vector_r, 1); // r = b + r  <=>  r = b - G * x
    }
    
    memcpy(vector_r2, vector_r, dimension * sizeof(double)); // r2 = r
    
    
    temp = cblas_ddot(dimension, vector_b, 1, vector_b, 1); // bT * b
    temp = circuit_simulation.iterative_methods.itol * circuit_simulation.iterative_methods.itol *  (temp == 0 ? 1 : temp);  // (bT * b) * itol * itol
    

    // ||rT * r|| / ||bT * b|| > itol <=> 
    // (||rT * r|| / ||bT * b||)^2 > itol^2 <=> 
    // (rT * r / bT * b) > itol^2 <=>
    // rT * r > (bT * b) * itol * itol
    while((temp3 = cblas_ddot(dimension, vector_r, 1, vector_r, 1)) > temp && iteration < max_iterations) {
        iteration++; 

        solve_diagonal_system(inv_preconditioner, vector_r, vector_z, dimension);  // z = inv(M) * r
	solve_diagonal_system(inv_preconditioner, vector_r2, vector_z2, dimension); // z2 = inv(M)T * r2 <=> z2 = inv(M) * r2
	
	rho = cblas_ddot(dimension, vector_r2, 1, vector_z, 1); // rho = r2T * z
	
	if(fabs(rho) < circuit_simulation.iterative_methods.EPS) {  // rho = 0, error
	    //printf("\nDivision by zero detected.\n");
	    //printf("Terminating.\n");
	    //exit(-1);
	    circuit_simulation.iterative_methods.division_by_zero = 1;
	}
	
	if(iteration == 1) {
	    memcpy(vector_p, vector_z, dimension * sizeof(double)); // p = z
	    memcpy(vector_p2, vector_z2, dimension * sizeof(double)); // p2 = z2
	}
	else {
	    beta = rho / rho1;
	    
	    cblas_dscal(dimension, beta, vector_p, 1);  // p = beta * p
	    cblas_daxpy(dimension, 1.0, vector_z, 1, vector_p, 1); // p = z + p <=> p = z + beta * p
	    
	    cblas_dscal(dimension, beta, vector_p2, 1); // p2 = beta * p2
	    cblas_daxpy(dimension, 1.0, vector_z2, 1, vector_p2, 1); // p2 = z2 + p2 <=> p2 = z2 + beta * p2
	}
	
	rho1 = rho;
	
	if(circuit_simulation.matrix_sparsity == SPARSE) {
	    memset(vector_q, 0, dimension * sizeof(double));
	    cs_di_gaxpy(G2, vector_p, vector_q); // q = G * p
	    memset(vector_q2, 0, dimension * sizeof(double));
	    cs_gaTxpy(G2, vector_p2, vector_q2); // q2 = GT * p2
        }
        else {
	    cblas_dgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, 1.0, matrix_G, dimension, vector_p, 1, 0.0, vector_q, 1); // q = G * p
	    cblas_dgemv(CblasRowMajor, CblasTrans, dimension, dimension, 1.0, matrix_G, dimension, vector_p2, 1, 0.0, vector_q2, 1); // q2 = G * p2
	}   
	
	omega = cblas_ddot(dimension, vector_p2, 1, vector_q, 1);  // omega = p2T * q
	
	if(fabs(omega) < circuit_simulation.iterative_methods.EPS) { // omega = 0, error
	    //printf("\nDivision by zero detected.\n");
	    //printf("Terminating.\n");
	    //exit(-1);
	    circuit_simulation.iterative_methods.division_by_zero = 1;
	}
	
	alpha = rho / omega;
	
	cblas_daxpy(dimension, alpha, vector_p, 1, vector_x, 1); // x = x + alpha * p
	cblas_daxpy(dimension, -alpha, vector_q, 1, vector_r, 1); // r = r - alpha * q
	cblas_daxpy(dimension, -alpha, vector_q2, 1, vector_r2, 1); // r2 = r2 - alpha * q2

    }
    
    /*if(iteration == dimension && temp3 > temp) {
        printf("Conjugate Gradient method failed to converge.\n");
	printf("Terminating.\n");
	exit(-1);
    }*/
    
    #if DEBUG
    printf("\nBiConjugate Gradient Result\n~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    printf("\nVector x:\n\n");
    for(k = 0; k < dimension; k++) {
        printf("%12lf\n", vector_x[k]);
    }
    
    printf("\n\n");
    #endif
    
    
    free(vector_r);
    free(vector_r2);
    free(vector_z);
    free(vector_z2);
    free(vector_p);
    free(vector_p2);
    free(vector_q);
    free(vector_q2);
  
}


void BiConjugateGradientComplex(int zero_x) {
  int dimension, iteration = 0, k;
  double complex temp = 0, rho = 0, rho1 = 0, beta = 0, alpha = 0, temp3 = 0, omega = 0;
  double complex *vector_r_complex = NULL, *vector_r2_complex = NULL, *vector_z_complex = NULL, *vector_z2_complex = NULL, *vector_p_complex = NULL, *vector_p2_complex = NULL, *vector_q_complex = NULL, *vector_q2_complex = NULL;
  int max_iterations;  
    double complex var0 = 0.0, var1 = 1.0, varminus1 = -1.0, conj_beta, alpha_minus, conj_alpha_minus;
    
    dimension = number_of_nodes + group2_elements;    
    
    max_iterations = 2 * dimension;
 
    vector_r_complex = (double complex *) calloc(dimension, sizeof(double complex));
    vector_r2_complex = (double complex *) malloc(dimension * sizeof(double complex));
    vector_z_complex = (double complex *) malloc(dimension * sizeof(double complex));
    vector_z2_complex = (double complex *) malloc(dimension * sizeof(double complex));
    vector_p_complex = (double complex *) malloc(dimension * sizeof(double complex));
    vector_p2_complex = (double complex *) malloc(dimension * sizeof(double complex));
    vector_q_complex = (double complex *) malloc(dimension * sizeof(double complex));
    vector_q2_complex = (double complex *) malloc(dimension * sizeof(double complex));
    
      
    if(vector_r_complex == NULL || vector_r2_complex == NULL || vector_z_complex == NULL || vector_z2_complex == NULL || vector_p_complex == NULL || vector_p2_complex == NULL || vector_q_complex == NULL || vector_q2_complex == NULL) {
        printf("Could not allocate matrices.\n");
	printf("Terminating.\n");
	exit(-1);
    }
    
    //printf("\n\nSolving (Bi-Conjugate Gradient) ...\n\n");
   
     
    if(zero_x) {
      	memset(vector_x_complex, 0, dimension * sizeof(double complex)); // x = 0
        memcpy(vector_r_complex, vector_b_complex, dimension * sizeof(double complex)); // r = b
    }
    else {
        if(circuit_simulation.matrix_sparsity == SPARSE) {
	    cs_ci_gaxpy(G2complex, vector_x_complex, vector_r_complex); // r = G * x
	}
	else {
	    cblas_zgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, &var1, matrix_Gcomplex, dimension, vector_x_complex, 1, &var0, vector_r_complex, 1); // r = G * x
	}
    
	cblas_zscal(dimension, &varminus1, vector_r_complex, 1); // r = -r
	cblas_zaxpy(dimension, &var1, vector_b_complex, 1, vector_r_complex, 1); // r = b + r  <=>  r = b - G * x
    }
    
    memcpy(vector_r2_complex, vector_r_complex, dimension * sizeof(double complex)); // r2 = r
    
    
    cblas_zdotc_sub(dimension, vector_b_complex, 1, vector_b_complex, 1, &temp); // bH * b
    temp = circuit_simulation.iterative_methods.itol * circuit_simulation.iterative_methods.itol *  (temp == 0 ? 1 : temp);  // (bH * b) * itol * itol
    

    // ||rH * r|| / ||bH * b|| > itol <=> 
    // (||rH * r|| / ||bH * b||)^2 > itol^2 <=> 
    // (rH * r / bH * b) > itol^2 <=>
    // rH * r > (bH * b) * itol * itol
    
    cblas_zdotc_sub(dimension, vector_r_complex, 1, vector_r_complex, 1, &temp3); // rH * r
    
    while(creal(temp3) > creal(temp) && iteration < max_iterations) {
        iteration++; 

        solve_diagonal_system_Complex(inv_preconditioner_complex, vector_r_complex, vector_z_complex, dimension, 0);  // z = inv(M) * r
	solve_diagonal_system_Complex(inv_preconditioner_complex, vector_r2_complex, vector_z2_complex, dimension, 1); // z2 = inv(M)H * r2 <=> z2 = conj(inv(M)) * r2
	
	cblas_zdotc_sub(dimension, vector_r2_complex, 1, vector_z_complex, 1, &rho); // rho = r2H * z
	
	if(cabs(rho) < circuit_simulation.iterative_methods.EPS) {  // rho = 0, error
	    //printf("\nDivision by zero detected.\n");
	    //printf("Terminating.\n");
	    //exit(-1);
	    circuit_simulation.iterative_methods.division_by_zero = 1;
	}
	
	if(iteration == 1) {
	    memcpy(vector_p_complex, vector_z_complex, dimension * sizeof(double complex)); // p = z
	    memcpy(vector_p2_complex, vector_z2_complex, dimension * sizeof(double complex)); // p2 = z2
	}
	else {
	    beta = rho / rho1;
	    
	    cblas_zscal(dimension, &beta, vector_p_complex, 1);  // p = beta * p
	    cblas_zaxpy(dimension, &var1, vector_z_complex, 1, vector_p_complex, 1); // p = z + p <=> p = z + beta * p
	    
	    conj_beta = conj(beta);
	    
	    cblas_zscal(dimension, &conj_beta, vector_p2_complex, 1); // p2 = conj(beta) * p2
	    cblas_zaxpy(dimension, &var1, vector_z2_complex, 1, vector_p2_complex, 1); // p2 = z2 + p2 <=> p2 = z2 + conj(beta) * p2
	}
	
	rho1 = rho;
	
	if(circuit_simulation.matrix_sparsity == SPARSE) {
	    memset(vector_q_complex, 0, dimension * sizeof(double complex));
	    cs_ci_gaxpy(G2complex, vector_p_complex, vector_q_complex); // q = G * p
	    memset(vector_q2_complex, 0, dimension * sizeof(double complex));
	    cs_gaHxpyComplex(G2complex, vector_p2_complex, vector_q2_complex); // q2 = GH * p2
        }
        else {
	    cblas_zgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, &var1, matrix_Gcomplex, dimension, vector_p_complex, 1, &var0, vector_q_complex, 1); // q = G * p
	    cblas_zgemv(CblasRowMajor, CblasConjTrans, dimension, dimension, &var1, matrix_Gcomplex, dimension, vector_p2_complex, 1, &var0, vector_q2_complex, 1); // q2 = GH * p2
	}   
	
	cblas_zdotc_sub(dimension, vector_p2_complex, 1, vector_q_complex, 1, &omega);  // omega = p2H * q
	
	if(cabs(omega) < circuit_simulation.iterative_methods.EPS) { // omega = 0, error
	    //printf("\nDivision by zero detected.\n");
	    //printf("Terminating.\n");
	    //exit(-1);
	    circuit_simulation.iterative_methods.division_by_zero = 1;
	}
	
	alpha = rho / omega;
	
	cblas_zaxpy(dimension, &alpha, vector_p_complex, 1, vector_x_complex, 1); // x = x + alpha * p
	
	alpha_minus = -alpha;
	cblas_zaxpy(dimension, &alpha_minus, vector_q_complex, 1, vector_r_complex, 1); // r = r - alpha * q
	
	conj_alpha_minus = -conj(alpha);
	cblas_zaxpy(dimension, &conj_alpha_minus, vector_q2_complex, 1, vector_r2_complex, 1); // r2 = r2 - conj(alpha) * q2

	cblas_zdotc_sub(dimension, vector_r_complex, 1, vector_r_complex, 1, &temp3); // rH * r
    }
    
    /*if(iteration == dimension && temp3 > temp) {
        printf("Conjugate Gradient method failed to converge.\n");
	printf("Terminating.\n");
	exit(-1);
    }*/
    
    #if DEBUG
    printf("\nBiConjugate Gradient Result\n~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    printf("\nVector x:\n\n");
    for(k = 0; k < dimension; k++) {
        printf("(%7.3lf %7.3lfi)\n", creal(vector_x_complex[k]), cimag(vector_x_complex[k]));
    }
    
    printf("\n\n");
    #endif
    
    
    free(vector_r_complex);
    free(vector_r2_complex);
    free(vector_z_complex);
    free(vector_z2_complex);
    free(vector_p_complex);
    free(vector_p2_complex);
    free(vector_q_complex);
    free(vector_q2_complex);
  
}
