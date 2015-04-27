#include "circuit_simulation.h"

/*
 *  b = e_1 + e - (G - 2/h * c) * x ,  e_1 = e
 */
void Trapezoidal(double *vector_e, double *vector_e_1) {
  int dimension;
  
    dimension = number_of_nodes + group2_elements; 
    
    if(circuit_simulation.matrix_sparsity == SPARSE) {
        ssmxvpv(C2, -1.0, vector_x, vector_e_1, vector_e_1, dimension, dimension); //e_1 = e_1 - C * x <=> e_1 = e_1 - (G - C) * x <=> e_1 = e_1 - (G - 2/h * C) * x
    }
    else {
	cblas_dgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, -1.0, matrix_C, dimension, vector_x, 1, 1.0, vector_e_1, 1); // e_1 = e_1 - C * x <=> e_1 = e_1 - (G - 2/h * C) * x
    }
	    
    cblas_daxpy(dimension, 1.0, vector_e, 1, vector_e_1, 1);  // e_1 = e_1 + e <=> e_1 = e_1 - (G - 2/h * C) * x + e
    memcpy(vector_b, vector_e_1, dimension * sizeof(double)); // b = e_1 <=> b = e_1 + e - (G - 2/h * C) * x
    memcpy(vector_e_1, vector_e, dimension * sizeof(double)); // e_1 = e
  
}


/*
 *  b = e + (C / h) * x
 */
void BackwardEuler(double *vector_e) {
  int dimension;
  
    dimension = number_of_nodes + group2_elements; 
  
    if(circuit_simulation.matrix_sparsity == SPARSE) {
	cs_di_gaxpy(C2, vector_x, vector_e); //e = e + C * x <=> e = e + (1/h * C) * x
    }
    else {
	cblas_dgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, 1.0, matrix_C, dimension, vector_x, 1, 1.0, vector_e, 1); //e = e + C * x <=> e = e + (1/h * C) * x
    }
	    
    memcpy(vector_b, vector_e, dimension * sizeof(double)); // b = e <=> b = e + (1/h * C) * x
  
}


/*
 *  x = x - Inverse(C / h) * G * x + Inverse(C / h) * e
 */
/*void ForwardEuler(double *vector_e, double *temp_vec) {
  int dimension;
  
    dimension = number_of_nodes + group2_elements; 
    
    memcpy(temp_vec, vector_x, dimension * sizeof(double));
    
    if(circuit_simulation.matrix_sparsity == SPARSE) {
        ssmxvpv(G2, -1.0, temp_vec, vector_x, vector_x, dimension, dimension); //x = x - G * x <=> x = x -Inverse(C / h) * G * x 
	cs_gaxpy(C2, vector_e, vector_x); //x = x + C * e <=> x = x -Inverse(C / h) * G * x + Inverse(C / h) * e
    }
    else {	
        cblas_dgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, -1.0, matrix_G, dimension, temp_vec, 1, 1.0, vector_x, 1); //x = x - G * x <=> x = x -Inverse(C / h) * G * x
        cblas_dgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, 1.0, matrix_C, dimension, vector_e, 1, 1.0, vector_x, 1); //x = x + C * e <=> x = x -Inverse(C / h) * G * x + Inverse(C / h) * e
    }
    
}*/
