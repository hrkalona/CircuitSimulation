#include "circuit_simulation.h" 


void createMnaSystemDC(void) {
  int l, k, i, dimension, temp3;
  struct twoTerminalsElement *current1 = NULL;
  cs *G = NULL;
  
    dimension = number_of_nodes + group2_elements;
    
    if(matrix_sparsity == SPARSE) {
        G = cs_spalloc(dimension, dimension, non_zeroes_G, 1, 1);

	if(G == NULL) {
	    printf("Could not allocate matrices.\n");
	    printf("Terminating.\n");
	    exit(-1);
	}
    }
    else {
	matrix_G = (double *) calloc(dimension * dimension, sizeof(double));
      
	if(matrix_G == NULL) {
	    printf("Could not allocate matrices.\n");
	    printf("Terminating.\n");
	    exit(-1);
	}
    }
    
    memset(vector_b, 0, dimension * sizeof(double));

    printf("\n\nCreating MNA System DC(%dx%d)...\n\n",dimension, dimension);
 
    
    if(matrix_sparsity == SPARSE) {
        for(current1 = head_twoTerminalsElement_list, k = 0, i = 0; head_twoTerminalsElement_list != NULL && current1 != NULL; current1 = current1 -> next) {
	    switch (current1 -> type) {
	      
		case RESISTANCE:

		    if(current1 -> positive_terminal) {
		        G -> i[i] = current1 -> positive_terminal - 1;
			G -> p[i] = current1 -> positive_terminal - 1;
			G -> x[i] = 1 / (current1 -> value);
			i++;
		    }
		    
		    if(current1 -> negative_terminal) {
			G -> i[i] = current1 -> negative_terminal - 1;
			G -> p[i] = current1 -> negative_terminal - 1;
			G -> x[i] = 1 / (current1 -> value);
			i++;
		    }
		    
		    if(current1 -> positive_terminal && current1 -> negative_terminal) {
			G -> i[i] = current1 -> positive_terminal - 1;
			G -> p[i] = current1 -> negative_terminal - 1;
			G -> x[i] = -(1 / (current1 -> value));
			i++;
			
			G -> i[i] = current1 -> negative_terminal - 1;
			G -> p[i] = current1 -> positive_terminal - 1;
			G -> x[i] = -(1 / (current1 -> value));
			i++;
		    }
		    
		    break;
		    
		case CURRENT_SOURCE:
		  
		    if(current1 -> positive_terminal) {
			vector_b[current1 -> positive_terminal - 1] -= current1 -> value;
		    }
		    
		    if(current1 -> negative_terminal) {
			vector_b[current1 -> negative_terminal - 1] += current1 -> value;
		    }
		    
		    break;
		    
		case VOLTAGE_SOURCE:
		case INDUCTOR:
		  
		    temp3 = number_of_nodes + k;
		    
		    if(current1 -> positive_terminal) {
		        G -> i[i] = current1 -> positive_terminal - 1;
			G -> p[i] = temp3;
			G -> x[i] = 1;
			i++;
			
			G -> i[i] = temp3;
			G -> p[i] = current1 -> positive_terminal - 1;
			G -> x[i] = 1;
			i++;
		    }
		    
		    if(current1 -> negative_terminal) {
		        G -> i[i] = current1 -> negative_terminal - 1;
			G -> p[i] = temp3;
			G -> x[i] = -1;
			i++;
			
			G -> i[i] = temp3;
			G -> p[i] = current1 -> negative_terminal - 1;
			G -> x[i] = -1;
			i++;
		    }
		    
		    vector_b[temp3] += current1 -> type == INDUCTOR ? 0 : current1 -> value; 		    
	
		    k++;
		    
		    break;

	    }
	    
	}
	
	G -> nz = non_zeroes_G;
	
	G2 = cs_compress(G);
	cs_spfree(G);
	cs_dupl(G2);
	
	#if DEBUG
	printf("\nMNA System DC\n~~~~~~~~~~~~~\n\n");	
	printf("\nMatrix G:\n\n");
	for(k = 0; k < dimension; k++) {
	    for(l = 0; l < dimension; l++) {
		printf("%12lf ", getElementAt(G2, k, l));
		if(l == number_of_nodes - 1) {
		    printf("   |");
		}
	    }
	    printf("\n");
	    
	    if(k == number_of_nodes - 1) {
		for(i = 0; i < dimension * 14; i++) {
		    printf("_");
		}
		printf("\n\n");
	    }
	    
	}
	#endif
	
	
    }
    else {
	for(current1 = head_twoTerminalsElement_list, k = 0; head_twoTerminalsElement_list != NULL && current1 != NULL; current1 = current1 -> next) {
	    switch (current1 -> type) {
	      
		case RESISTANCE:

		    if(current1 -> positive_terminal) {
			matrix_G[(current1 -> positive_terminal - 1) * dimension + (current1 -> positive_terminal - 1)] += 1 / (current1 -> value);
		    }
		    
		    if(current1 -> negative_terminal) {
			matrix_G[(current1 -> negative_terminal - 1) * dimension + (current1 -> negative_terminal - 1)] += 1 / (current1 -> value);
		    }
		    
		    if(current1 -> positive_terminal && current1 -> negative_terminal) {
			matrix_G[(current1 -> positive_terminal - 1) * dimension + (current1 -> negative_terminal - 1)] -= 1 / (current1 -> value);
			matrix_G[(current1 -> negative_terminal - 1) * dimension + (current1 -> positive_terminal - 1)] -= 1 / (current1 -> value);
		    }
		    
		    break;
		    
		case CURRENT_SOURCE:
		  
		    if(current1 -> positive_terminal) {
			vector_b[current1 -> positive_terminal - 1] -= current1 -> value;
		    }
		    
		    if(current1 -> negative_terminal) {
			vector_b[current1 -> negative_terminal - 1] += current1 -> value;
		    }
		    
		    break;
		    
		case VOLTAGE_SOURCE:
		case INDUCTOR:
		  
		    temp3 = number_of_nodes + k;
		    
		    if(current1 -> positive_terminal) {
			matrix_G[(current1 -> positive_terminal - 1) * dimension + temp3] += 1;
			matrix_G[temp3 * dimension + (current1 -> positive_terminal - 1)] += 1;
		    }
		    
		    if(current1 -> negative_terminal) {
			matrix_G[(current1 -> negative_terminal - 1) * dimension + temp3] -= 1;
			matrix_G[temp3 * dimension + (current1 -> negative_terminal - 1)] -= 1;
		    }
		    
		    
		    vector_b[temp3] += current1 -> type == INDUCTOR ? 0 : current1 -> value;
		    
		    k++;
		    
		    break;

	    }
	    
	}
	

	
	#if DEBUG 
	printf("\nMNA System DC\n~~~~~~~~~~~~~\n\n");

	printf("\nMatrix G:\n\n");
	for(k = 0; k < dimension; k++) {
	    for(l = 0; l < dimension; l++) {
		printf("%12lf ", matrix_G[k * dimension + l]);
		if(l == number_of_nodes - 1) {
		    printf("   |");
		}
	    }
	    printf("\n");
	    
	    if(k == number_of_nodes - 1) {
		for(i = 0; i < dimension * 14; i++) {
		    printf("_");
		}
		printf("\n\n");
	    }
	    
	}
	#endif
    }
    
    
    
    #if DEBUG 
    printf("\n\n\nVector b:\n\n");
    for(k = 0; k < dimension; k++) {
        printf("%12lf ", vector_b[k]);
	printf("\n");
	if(k == number_of_nodes - 1) {
	    printf("_____________\n\n");
	}
    }
    
    printf("\n\n");
    #endif
  
}


void createMnaSystemTransient(void) {
  int l, k, i, j, dimension, temp3;
  struct twoTerminalsElement *current1 = NULL;
  cs *C = NULL;
  
    dimension = number_of_nodes + group2_elements;
    
    if(matrix_sparsity == SPARSE) {
        C = cs_spalloc(dimension, dimension, non_zeroes_C, 1, 1);

	if(C == NULL) {
	    printf("Could not allocate matrices.\n");
	    printf("Terminating.\n");
	    exit(-1);
	}
    }
    else {
	matrix_C = (double *) calloc(dimension * dimension, sizeof(double));
	temp_matrix = (double *) malloc(dimension * dimension * sizeof(double));
      
	if(matrix_C == NULL || temp_matrix == NULL) {
	    printf("Could not allocate matrices.\n");
	    printf("Terminating.\n");
	    exit(-1);
	}
	
	if(diff_method == TRAPEZOIDAL) {
	    temp_matrix2 = (double *) malloc(dimension * dimension * sizeof(double));
	    
	    if(temp_matrix2 == NULL) {
		printf("Could not allocate matrices.\n");
		printf("Terminating.\n");
		exit(-1);
	    }
	  
	}
    }
    
    
    if(method == DIRECT) {
	if(matrix_sparsity == NORMAL) {
	    free(matrix_G);
	    
	    if(matrix_type == NONSPD) {
		  free(transposition);
	    }
	}
	else {
	    cs_sfree(S);
	    cs_nfree(N);
	}
	createMnaSystemDC();
    }
    
    sources = (struct twoTerminalsElement**) malloc((number_of_elements[CURRENT_SOURCE] + group2_elements) * sizeof(struct twoTerminalsElement));
    
    if(sources == NULL) {
	printf("Could not allocate matrices.\n");
	printf("Terminating.\n");
	exit(-1);
    }
    
   
    printf("\n\nCreating MNA System Transient(%dx%d)...\n\n",dimension, dimension);
    
    if(matrix_sparsity == SPARSE) {
        for(current1 = head_twoTerminalsElement_list, k = 0, i = 0, j = 0; head_twoTerminalsElement_list != NULL && current1 != NULL; current1 = current1 -> next) {
	    switch (current1 -> type) {
	      
		case CAPACITOR:

		    if(current1 -> positive_terminal) {
		        C -> i[i] = current1 -> positive_terminal - 1;
			C -> p[i] = current1 -> positive_terminal - 1;
			C -> x[i] = current1 -> value * (diff_method == TRAPEZOIDAL ? 2 / time_step : 1 / time_step);
			i++;
		    }
		    
		    if(current1 -> negative_terminal) {
			C -> i[i] = current1 -> negative_terminal - 1;
			C -> p[i] = current1 -> negative_terminal - 1;
			C -> x[i] = current1 -> value * (diff_method == TRAPEZOIDAL ? 2 / time_step : 1 / time_step);
			i++;
		    }
		    
		    if(current1 -> positive_terminal && current1 -> negative_terminal) {
			C -> i[i] = current1 -> positive_terminal - 1;
			C -> p[i] = current1 -> negative_terminal - 1;
			C -> x[i] = -current1 -> value * (diff_method == TRAPEZOIDAL ? 2 / time_step : 1 / time_step);
			i++;
			
			C -> i[i] = current1 -> negative_terminal - 1;
			C -> p[i] = current1 -> positive_terminal - 1;
			C -> x[i] = -current1 -> value * (diff_method == TRAPEZOIDAL ? 2 / time_step : 1 / time_step);
			i++;
		    }
		    
		    break;
		    
		case CURRENT_SOURCE:
		    sources[j] = current1;
		    j++;
		    
		    if(current1 -> transient != NULL) {
			createTransientSteps(current1 -> transient);
		    }
		    break;
	
		case VOLTAGE_SOURCE:
		case INDUCTOR:
		  
		    temp3 = number_of_nodes + k;
		    
		    if(current1 -> type == INDUCTOR) {
			C -> i[i] = temp3;
			C -> p[i] = temp3;
			C -> x[i] = -current1 -> value * (diff_method == TRAPEZOIDAL ? 2 / time_step : 1 / time_step);
			i++;
		    }
		    
		    sources[j] = current1;
		    j++;
		    
		    if(current1 -> transient != NULL) {
			createTransientSteps(current1 -> transient);
		    }

		    k++;
		    
		    break;

	    }
	    
	}
	
	C -> nz = non_zeroes_C;
	
	C2 = cs_compress(C);
	cs_spfree(C);
	cs_dupl(C2);
	
	
	temp_mat = cs_add(G2, C2, 1, 1); // temp_mat = G + C
	
	if(diff_method == TRAPEZOIDAL) {
	    temp_mat2 = cs_add(G2, C2, 1, -1); // temp_mat2 = G - C
	}
	
	#if DEBUG
	printf("\nMNA System Transient\n~~~~~~~~~~~~~~~~~~~~\n\n");	
	printf("\nMatrix C:\n\n");
	for(k = 0; k < dimension; k++) {
	    for(l = 0; l < dimension; l++) {
		printf("%12lf ", getElementAt(C2, k, l) * (diff_method == TRAPEZOIDAL ? time_step / 2 : time_step));
		if(l == number_of_nodes - 1) {
		    printf("   |");
		}
	    }
	    printf("\n");
	    
	    if(k == number_of_nodes - 1) {
		for(i = 0; i < dimension * 14; i++) {
		    printf("_");
		}
		printf("\n\n");
	    }
	    
	}
	
	printf("\n\n");
	#endif
	
	
    }
    else {
	for(current1 = head_twoTerminalsElement_list, k = 0, j = 0; head_twoTerminalsElement_list != NULL && current1 != NULL; current1 = current1 -> next) {
	    switch (current1 -> type) {
	      
		case CAPACITOR:

		    if(current1 -> positive_terminal) {
			matrix_C[(current1 -> positive_terminal - 1) * dimension + (current1 -> positive_terminal - 1)] += current1 -> value * (diff_method == TRAPEZOIDAL ? 2 / time_step : 1 / time_step);
		    }
		    
		    if(current1 -> negative_terminal) {
			matrix_C[(current1 -> negative_terminal - 1) * dimension + (current1 -> negative_terminal - 1)] += current1 -> value * (diff_method == TRAPEZOIDAL ? 2 / time_step : 1 / time_step);
		    }
		    
		    if(current1 -> positive_terminal && current1 -> negative_terminal) {
			matrix_C[(current1 -> positive_terminal - 1) * dimension + (current1 -> negative_terminal - 1)] -= current1 -> value * (diff_method == TRAPEZOIDAL ? 2 / time_step : 1 / time_step);
			matrix_C[(current1 -> negative_terminal - 1) * dimension + (current1 -> positive_terminal - 1)] -= current1 -> value * (diff_method == TRAPEZOIDAL ? 2 / time_step : 1 / time_step);
		    }
		    
		    break;
		    
		case CURRENT_SOURCE:
		    sources[j] = current1;
		    j++;
		    
		    if(current1 -> transient != NULL) {
			createTransientSteps(current1 -> transient);
		    }
		    
		    break;
		    
		case VOLTAGE_SOURCE:
		case INDUCTOR:
		  
		    temp3 = number_of_nodes + k;
		    
		    if(current1 -> type == INDUCTOR) {
			matrix_C[temp3 * dimension + temp3] -= current1 -> value * (diff_method == TRAPEZOIDAL ? 2 / time_step : 1 / time_step);
		    }
		    else {
			if(current1 -> transient != NULL) {
			    createTransientSteps(current1 -> transient);
			}
		    }

		    sources[j] = current1;
		    j++;

		    k++;
		    
		    break;

	    }
	    
	}
	
	memcpy(temp_matrix, matrix_G, dimension * dimension * sizeof(double)); // temp_matrix = G
	cblas_daxpy(dimension * dimension, 1.0, matrix_C, 1, temp_matrix, 1); // temp_matrix = temp_matrix + C  <=>  temp_matrix = G + C
	
	if(diff_method == TRAPEZOIDAL) {
	    memcpy(temp_matrix2, matrix_G, dimension * dimension * sizeof(double)); // temp_matrix2 = G
	    cblas_daxpy(dimension * dimension, -1.0, matrix_C, 1, temp_matrix2, 1); // temp_matrix2 = temp_matrix2 - C  <=>  temp_matrix = G - C
	}
	
	
	#if DEBUG 
	printf("\nMNA System Transient\n~~~~~~~~~~~~~~~~~~~~\n\n");
	printf("\nMatrix C:\n\n");
	for(k = 0; k < dimension; k++) {
	    for(l = 0; l < dimension; l++) {
		printf("%12lf ", matrix_C[k * dimension + l] * (diff_method == TRAPEZOIDAL ? time_step / 2 : time_step));
		if(l == number_of_nodes - 1) {
		    printf("   |");
		}
	    }
	    printf("\n");
	    
	    if(k == number_of_nodes - 1) {
		for(i = 0; i < dimension * 14; i++) {
		    printf("_");
		}
		printf("\n\n");
	    }
	    
	}
	
	printf("\n\n");
	#endif
	
    }

}



void createTransientSteps(struct transientComponent*  transient) {
  int i, k, length;
  double t, time_steps;
  struct ExpImpulse* e = NULL;
  struct SinImpulse* s = NULL;
  struct PulseImpulse* p = NULL;
  struct PWLImpulse* pw = NULL;
  

    time_steps = fin_time / time_step;
  
    length = (int)time_steps + 1;
    
    transient -> vals = (double *) malloc(length * sizeof(double));
    
    if(transient -> vals == NULL) {
	printf("Could not allocate matrices.\n");
	printf("Terminating.\n");
	exit(-1);
    }
    
    switch (transient -> impulse) {
	case EXP:
	    e = transient -> exp;
	    for(i = 0, t = 0; i < length; i++, t += time_step) { //precalculate values for every time step
	        if(t >= 0 && t < e -> td1) {
		    transient -> vals[i] = e -> i1;
		}
		else if(t >= e -> td1 && t < e -> td2) {
		    transient -> vals[i] = e -> i1 + (e -> i2 - e -> i1) * (1 - exp(-(t - e -> td1) / e -> tc1));
		}
		else if(t >= e -> td2) {
		    transient -> vals[i] = e -> i1 + (e -> i2 - e -> i1) * (exp(-(t - e -> td2) / e -> tc2) - exp(-(t - e -> td1) / e -> tc1));
		}
	    }
	    break;
	case SIN:
	    s = transient -> sin;
	    for(i = 0, t = 0; i < length; i++, t += time_step) {
	        if(t >= 0 && t < s -> td) {
		    transient -> vals[i] = s -> i1 + s -> ia * sin((2 * M_PI * s -> ph) / 360);
		}
		else if(t >= s -> td) {
		    transient -> vals[i] = s -> i1 + s -> ia * sin(2 * M_PI * s -> fr * (t - s -> td) + (2 * M_PI * (s -> ph)) / 360) * exp(-(t - s -> td) * s -> df);
		}
	    }
	    break;
	case PULSE:
	    p = transient -> pulse;
	    for(i = 0, t = 0, k = 0; i < length; i++, t += time_step) {
	        if(t >= p -> td + (k + 1) * p -> per) {
		    k++;
		}
		
	        if(t >= 0 && t < p -> td) {
		    transient -> vals[i] = p -> i1;
		}
		else if(t >= p -> td + k * p -> per && t < p -> td + p -> tr + k * p -> per) {
		    transient -> vals[i] = (p -> i2 - p -> i1) / (p -> td + p -> tr + k * p -> per - (p -> td + k * p -> per)) * (t - (p -> td + k * p -> per)) + p -> i1;
		}
		else if(t >= p -> td + p -> tr + k * p -> per && t < p -> td + p -> tr + p -> pw + k * p -> per) {
		    transient -> vals[i] = p -> i2;
		}
		else if(t >= p -> td + p -> tr + p -> pw + k * p -> per && t < p -> td + p -> tr + p -> pw + p -> tf + k * p -> per) {
		    transient -> vals[i] = (p -> i1 - p -> i2) / (p -> td + p -> tr + p -> pw + p -> tf + k * p -> per - (p -> td + p -> tr + p -> pw + k * p -> per)) * (t - (p -> td + p -> tr + p -> pw + k * p -> per)) + p -> i2;
		}
		else if(t >= p -> td + p -> tr + p -> pw + p -> tf + k * p -> per && t <= p -> td + (k + 1) * p -> per) {
		    transient -> vals[i] = p -> i1;
		}
	    }
	    break;
	case PWL:
	    pw = transient -> pwl;
	    pw -> t[pw -> length - 1] = fin_time;
	    for(i = 0, t = 0, k = 0; i < length; i++, t += time_step) {
		for(; k < pw -> length - 1; k++) {
		    if(t >=  pw -> t[k] && t <=  pw -> t[k + 1]) {
			break;
		    }
		}
		
		transient -> vals[i] = ((pw -> i[k + 1] - pw -> i[k]) /  (pw -> t[k + 1] - pw -> t[k])) * (t - pw -> t[k]) + pw -> i[k];
	    }
	    break;
 
    }
  
}



void LUDecomposition(void) {
  int k, dimension, i, m, l, j;
  
  
    dimension = number_of_nodes + group2_elements;
    
       
    if(matrix_sparsity == SPARSE) {
        printf("\n\nLU Decomposition...\n\n");
	S = cs_sqr(2, G2, 0);
	N = cs_lu(G2, S, 1);
	cs_spfree(G2);
	
	if(N == NULL) {
	    printf("Error, the matrix cannot be inverted.\n\n");
	    exit(-1); 
	}
	
	#if DEBUG
	printf("\nLU Decomposition\n~~~~~~~~~~~~~~~~~\n\n");
	printf("\nDecomposed Matrix:\n\n");
	for(k = 0; k < dimension; k++) {
	    for(l = 0; l < dimension; l++) {
	        if(l == k) {
		    printf("  \\\\");
		}
		if(l >= k) {
		    printf("%12lf ", getElementAt(N -> U, k, l)); 
		}
		else {
		    printf("%12lf ", getElementAt(N -> L, k, l));
		}
	    }
	    printf("\n\n");
	}
	
	printf("\n\n");
	
	printf("\nLine Transposition:\n");
	for(k = 0; k < dimension; k++) {
	    printf("line %d contains line %d\n", N -> pinv[k], k);
	}
	printf("\n\n");
	#endif
    }
    else {
	transposition = (int *) malloc(dimension * sizeof(int));
	
	if(transposition == NULL) {
	    printf("Could not allocate matrices.\n");
	    printf("Terminating.\n");
	    exit(-1);
	}
	
	for(i = 0; i < dimension; i++) {
	    transposition[i] = i; 
	}
	
	printf("\n\nLU Decomposition...\n\n");
	
	for(k = 0; k < dimension - 1; k++) {
	    for(m = i = k; i < dimension; i++) {
		if(fabs(matrix_G[i * dimension + k]) > fabs(matrix_G[m * dimension + k])) {
		    m = i;
		}
	    }
			    
	    interchange(m, k);
	    
	    if(fabs(matrix_G[k * dimension + k]) == 0) {
		printf("Error, the matrix cannot be inverted.\n\n");
		exit(-1); 
	    }
	    
	    for(i = k + 1; i < dimension; i++) {  
		matrix_G[i * dimension + k] /= matrix_G[k * dimension + k];
		for(j = k + 1; j < dimension; j++) {
		    matrix_G[i * dimension + j] += -matrix_G[i * dimension + k] * matrix_G[k * dimension + j];
		}
	    }    
	}
	
	#if DEBUG
	printf("\nLU Decomposition\n~~~~~~~~~~~~~~~~~\n\n");

	printf("\nDecomposed Matrix:\n\n");
	for(k = 0; k < dimension; k++) {
	    for(l = 0; l < dimension; l++) {
	      if(l == k) {
		    printf("  \\\\");
		}
		printf("%12lf ", matrix_G[k * dimension + l]);
		
	    }
	    printf("\n\n");
	}
	
	
	printf("\nLine Transposition:\n");
	for(k = 0; k < dimension; k++) {
	    printf("line %d contains line %d\n", k, transposition[k]);
	}
	
	printf("\n\n");
	#endif  
    }

      
}

void CholeskyDecomposition(void) {
  int k, i, l, j, dimension;
  double sum, temp;
  
    dimension = number_of_nodes + group2_elements;
    
    if(matrix_sparsity == SPARSE) {
        printf("\n\nCholesky Decomposition...\n\n");
        S = cs_schol(1, G2);
	N = cs_chol(G2, S);
	cs_spfree(G2);
	
	if(N == NULL) {
	    printf("Error, the matrix must be symmetric & positive definite.\n\n");
	    exit(-1);
	}
        
	#if DEBUG
	printf("\nCholesky Decomposition\n~~~~~~~~~~~~~~~~~~~~~~~\n\n");
	printf("\nDecomposed Matrix:\n\n");
	for(k = 0; k < dimension; k++) {
	    for(l = 0; l < dimension; l++) {
		if(l == k) {
		    printf("  \\\\");
		}
		if(l <= k) {
		    printf("%12lf ", getElementAt(N -> L, k, l));
		}
		else {
		    printf("%12lf ", getElementAt(N -> L, l, k));
		}
		if(l == k) {
		    printf("  \\\\");
		}
		
	    }
	    printf("\n\n");
	}
	
	printf("\n\n");
	#endif
    }
    else {
        printf("\n\nCholesky Decomposition...\n\n");
	for(k = 0; k < dimension; k++) {
	    for(sum = 0, j = 0; j < k; j++) {
	      sum += matrix_G[k * dimension + j] * matrix_G[k * dimension + j];
	    }
	    
	    if((temp = matrix_G[k * dimension + k] - sum) < 0) {
		printf("Error, the matrix must be symmetric & positive definite.\n\n");
		exit(-1);
	    }
	    matrix_G[k * dimension + k] = sqrt(temp);

	    
	    for(i = k + 1; i < dimension; i++) {
		for(sum = 0, j = 0; j < k; j++) {
		    sum += matrix_G[i * dimension + j] * matrix_G[k * dimension + j];
		}
		matrix_G[k * dimension + i] = matrix_G[i * dimension + k] = (matrix_G[k * dimension + i] - sum) / matrix_G[k * dimension + k];
	    }
	}
	
	#if DEBUG
	printf("\nCholesky Decomposition\n~~~~~~~~~~~~~~~~~~~~~~~\n\n");

	printf("\nDecomposed Matrix:\n\n");
	for(k = 0; k < dimension; k++) {
	    for(l = 0; l < dimension; l++) {
		if(l == k) {
		    printf("  \\\\");
		}
		printf("%12lf ", matrix_G[k * dimension + l]);
		if(l == k) {
		    printf("  \\\\");
		}
		
	    }
	    printf("\n\n");
	}
	
	printf("\n\n");
	
	#endif  
    }
    
    
}


void interchange(int from, int to) {
  int j, dimension, temp3;
  double temp;
   
    if(from == to) {
        return;
    }
    
    dimension = number_of_nodes + group2_elements;
    
    #pragma omp parallel for default(shared) private(j, temp)
    for(j = 0; j < dimension; j++) {
        temp = matrix_G[from * dimension + j];
        matrix_G[from * dimension + j] = matrix_G[to * dimension + j];
	matrix_G[to * dimension + j] = temp;
    }
 
    temp3 = transposition[from];
    transposition[from] = transposition[to];
    transposition[to] = temp3;
    
}


void forward_substitution(void) {
  int dimension, k, j;
  double a, *vector_temp = NULL;
  
    dimension = number_of_nodes + group2_elements;

    
    vector_y = (double *) calloc(dimension, sizeof(double));
    vector_temp = (double *) malloc(dimension * sizeof(double));
    
    if(vector_y == NULL || vector_temp == NULL) {
        printf("Could not allocate matrices.\n");
	printf("Terminating.\n");
	exit(-1);
    }
    
    for(k = 0; k < dimension; k++) {
        if(matrix_type == NONSPD) {
	    vector_temp[k] = vector_b[transposition[k]];
	}
	else {
	    vector_temp[k] = vector_b[k];
	}
    }
    
    for(k = 0; k < dimension; k++) {
        a = vector_temp[k];
	#pragma omp parallel for default(shared) private(j) reduction(+:a) 
        for(j = 0; j < k; j++) {
            a -= matrix_G[k * dimension + j] * vector_y[j]; 
        }
        vector_y[k] = a / (matrix_type == NONSPD ? 1 : matrix_G[k * dimension + k]);
    }
    

    #if DEBUG
    printf("\nVector y:\n\n");
    for(k = 0; k < dimension; k++) {
        printf("%12lf\n", vector_y[k]);
    }
    
    printf("\n\n");
    #endif
    
    free(vector_temp);
  
}

void backward_substitution(void) {
  int dimension, k, j;
  double a;
  
    dimension = number_of_nodes + group2_elements;
    
    memset(vector_x, 0, dimension * sizeof(double));

    for(k = dimension - 1; k >= 0; k--) {
        a = vector_y[k];
	#pragma omp parallel for default(shared) private(j) reduction(+:a) 
        for(j = k + 1; j < dimension; j++) {
            a -= matrix_G[k * dimension + j] * vector_x[j]; 
        }
        vector_x[k] = a / matrix_G[k * dimension + k];
    }
    
    #if DEBUG
    printf("\nVector x:\n\n");
    for(k = 0; k < dimension; k++) {
        printf("%12lf\n", vector_x[k]);
    }
    
    printf("\n\n");
    #endif
    
    free(vector_y);
  
}


void solve_direct_methods(void) {
  int dimension, k;
  double *vector_temp = NULL;
  
    printf("\n\nSolving (Forward/Backward)...\n\n");
    
    if(matrix_sparsity == SPARSE) {
	dimension = number_of_nodes + group2_elements;
	
	vector_temp = (double *) calloc(dimension, sizeof(double));
	
	if(vector_temp == NULL) {
	    printf("Could not allocate matrices.\n");
	    printf("Terminating.\n");
	    exit(-1);
	}
	
	
	memcpy(vector_x, vector_b, dimension * sizeof(double));
	
	if(matrix_type == NONSPD) {        /*Forward Backward LU*/    
	    cs_ipvec(N -> pinv, vector_x, vector_temp, dimension);
	    cs_lsolve(N -> L, vector_temp);
	    cs_usolve(N -> U, vector_temp);
	    cs_ipvec(S -> q, vector_temp, vector_x, dimension);
	}
	else {/*Forward Backward Cholesky*/
	    cs_ipvec(S -> pinv, vector_x, vector_temp, dimension);
	    cs_lsolve(N -> L, vector_temp);
	    cs_ltsolve(N -> L, vector_temp);
	    cs_pvec(S -> pinv, vector_temp, vector_x, dimension);
	}

	#if DEBUG
	printf("\nVector x:\n\n");
	for(k = 0; k < dimension; k++) {
	    printf("%12lf\n", vector_x[k]);
	}
	
	printf("\n\n");
	#endif
	
	free(vector_temp); 
    }
    else {
        forward_substitution();
	backward_substitution();
    }
     
}


void createInversePreconditioner(double *inv_preconditioner, int length) {
  int i;
  double temp;
  
    if(matrix_sparsity == SPARSE) {
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


void ConjugateGradient(void) {
  int dimension, iteration = 0, k;
  double temp, rho, rho1, beta, alpha, temp3;
  double *vector_r = NULL, *vector_z = NULL, *inv_preconditioner = NULL, *vector_p = NULL, *vector_q = NULL;
  
    dimension = number_of_nodes + group2_elements;
    
 
    vector_r = (double *) calloc(dimension, sizeof(double));
    vector_z = (double *) malloc(dimension * sizeof(double));
    inv_preconditioner = (double *) malloc(dimension * sizeof(double));
    vector_p = (double *) malloc(dimension * sizeof(double));
    vector_q = (double *) malloc(dimension * sizeof(double));
    
  
    if(vector_r == NULL || vector_z == NULL || inv_preconditioner == NULL || vector_p == NULL || vector_q == NULL) {
        printf("Could not allocate matrices.\n");
	printf("Terminating.\n");
	exit(-1);
    }
    

    printf("\n\nSolving (Conjugate Gradient)...\n\n");
    
    createInversePreconditioner(inv_preconditioner, dimension); // inv(M) = 1 / diag(G)
    
    if(matrix_sparsity == SPARSE) {
        cs_gaxpy(G2, vector_x, vector_r); // r = G * x
    }
    else {
        cblas_dgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, 1.0, matrix_G, dimension, vector_x, 1, 0.0, vector_r, 1); // r = G * x
    }
    
    cblas_dscal(dimension, -1.0, vector_r, 1); // r = -r
    cblas_daxpy(dimension, 1.0, vector_b, 1, vector_r, 1); // r = b + r  <=>  r = b - G * x
    
    
    temp = cblas_ddot(dimension, vector_b, 1, vector_b, 1); // bT * b
    temp = itol * itol *  (temp == 0 ? 1 : temp); // (bT * b) * itol * itol
    
    
    // ||rT * r|| / ||bT * b|| > itol <=> 
    // (||rT * r|| / ||bT * b||)^2 > itol^2 <=> 
    // (rT * r / bT * b) > itol^2 <=>
    // rT * r > (bT * b) * itol * itol
    while((temp3 = cblas_ddot(dimension, vector_r, 1, vector_r, 1)) > temp && iteration < dimension) {
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
	
	if(matrix_sparsity == SPARSE) {
	    memset(vector_q, 0, dimension * sizeof(double));
	    cs_gaxpy(G2, vector_p, vector_q); // q = G * p
        }
        else {
	    cblas_dgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, 1.0, matrix_G, dimension, vector_p, 1, 0.0, vector_q, 1); // q = G * p
	}
	
	alpha = rho / cblas_ddot(dimension, vector_p, 1, vector_q, 1); // alpha = rho / (pT * q)
	
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
    free(inv_preconditioner);
    free(vector_p);
    free(vector_q);
  
}


void BiConjugateGradient(void) {
  int dimension, iteration = 0, k;
  double temp, rho, rho1, beta, alpha, temp3, omega;
  double EPS = 1e-14;
  double *vector_r = NULL, *vector_r2 = NULL, *vector_z = NULL, *vector_z2 = NULL, *inv_preconditioner = NULL, *vector_p = NULL, *vector_p2 = NULL, *vector_q = NULL, *vector_q2 = NULL;
  
    dimension = number_of_nodes + group2_elements;    
 
    vector_r = (double *) calloc(dimension, sizeof(double));
    vector_r2 = (double *) malloc(dimension * sizeof(double));
    vector_z = (double *) malloc(dimension * sizeof(double));
    vector_z2 = (double *) malloc(dimension * sizeof(double));
    inv_preconditioner = (double *) malloc(dimension * sizeof(double));
    vector_p = (double *) malloc(dimension * sizeof(double));
    vector_p2 = (double *) malloc(dimension * sizeof(double));
    vector_q = (double *) malloc(dimension * sizeof(double));
    vector_q2 = (double *) malloc(dimension * sizeof(double));
    
      
    if(vector_r == NULL || vector_r2 == NULL || vector_z == NULL || vector_z2 == NULL || inv_preconditioner == NULL || vector_p == NULL || vector_p2 == NULL || vector_q == NULL || vector_q2 == NULL) {
        printf("Could not allocate matrices.\n");
	printf("Terminating.\n");
	exit(-1);
    }
    
    printf("\n\nSolving (Bi-Conjugate Gradient)...\n\n");
    
    createInversePreconditioner(inv_preconditioner, dimension);  // inv(M) = 1 / diag(G)
    
    if(matrix_sparsity == SPARSE) {
        cs_gaxpy(G2, vector_x, vector_r); // r = G * x
    }
    else {
        cblas_dgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, 1.0, matrix_G, dimension, vector_x, 1, 0.0, vector_r, 1); // r = G * x
    }
 
    cblas_dscal(dimension, -1.0, vector_r, 1); // r = -r
    cblas_daxpy(dimension, 1.0, vector_b, 1, vector_r, 1); // r = b + r  <=>  r = b - G * x
    
    memcpy(vector_r2, vector_r, dimension * sizeof(double)); // r2 = r
    
    
    temp = cblas_ddot(dimension, vector_b, 1, vector_b, 1); // bT * b
    temp = itol * itol *  (temp == 0 ? 1 : temp);  // (bT * b) * itol * itol
    

    // ||rT * r|| / ||bT * b|| > itol <=> 
    // (||rT * r|| / ||bT * b||)^2 > itol^2 <=> 
    // (rT * r / bT * b) > itol^2 <=>
    // rT * r > (bT * b) * itol * itol
    while((temp3 = cblas_ddot(dimension, vector_r, 1, vector_r, 1)) > temp && iteration < dimension) {
        iteration++; 

        solve_diagonal_system(inv_preconditioner, vector_r, vector_z, dimension);  // z = inv(M) * r
	solve_diagonal_system(inv_preconditioner, vector_r2, vector_z2, dimension); // z2 = inv(M)T * r2 <=> z2 = inv(M) * r2
	
	rho = cblas_ddot(dimension, vector_z, 1, vector_r2, 1); // rho = zT * r2
	
	if(fabs(rho) < EPS) {  // rho = 0, error
	    printf("Division by zero detected.\n");
	    printf("Terminating.\n");
	    exit(-1);
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
	
	if(matrix_sparsity == SPARSE) {
	    memset(vector_q, 0, dimension * sizeof(double));
	    cs_gaxpy(G2, vector_p, vector_q); // q = G * p
	    memset(vector_q2, 0, dimension * sizeof(double));
	    cs_gaTxpy(G2, vector_p2, vector_q2); // q2 = G * p2
        }
        else {
	    cblas_dgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, 1.0, matrix_G, dimension, vector_p, 1, 0.0, vector_q, 1); // q = G * p
	    cblas_dgemv(CblasRowMajor, CblasTrans, dimension, dimension, 1.0, matrix_G, dimension, vector_p2, 1, 0.0, vector_q2, 1); // q2 = G * p2
	}   
	
	omega = cblas_ddot(dimension, vector_p2, 1, vector_q, 1);  // omega = p2T * q
	
	if(fabs(omega) < EPS) { // omega = 0, error
	    printf("Division by zero detected.\n");
	    printf("Terminating.\n");
	    exit(-1);
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
    free(inv_preconditioner);
    free(vector_p);
    free(vector_p2);
    free(vector_q);
    free(vector_q2);
  
}

void solve_diagonal_system(double *vector1, double *vector2, double *result, int length) {
  int i;
  
    #pragma omp parallel for default(shared) private(i)
    for(i = 0; i < length; i++) {
        result[i] = vector1[i] * vector2[i];
    }
    
}

void Trapezoidal(double *vector_e, double *vector_e_1, double *temp_vec) {
  int dimension;
  
    dimension = number_of_nodes + group2_elements; 
    
    if(matrix_sparsity == SPARSE) {
	memset(temp_vec, 0, dimension * sizeof(double));
	cs_gaxpy(C2, vector_x, temp_vec); //temp_vec = C * x <=> temp_vec = (G - C) * x <=> temp_vec = (G - 2/h * C) * x
    }
    else {
	cblas_dgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, 1.0, matrix_C, dimension, vector_x, 1, 0.0, temp_vec, 1); //temp_vec = C * x <=> temp_vec = (G - C) * x <=> temp_vec = (G - 2/h * C) * x
    }
	    
    cblas_daxpy(dimension, 1.0, vector_e, 1, vector_e_1, 1);  // e_1 = e_1 + e
    cblas_daxpy(dimension, -1.0, temp_vec, 1, vector_e_1, 1); // e_1 = e_1 - temp_vec
    memcpy(vector_b, vector_e_1, dimension * sizeof(double)); // b = e_1 <=> b = e_1 + e - (G - 2/h * C) * x
    memcpy(vector_e_1, vector_e, dimension * sizeof(double)); // e_1 = e
  
}

void BackwardEuler(double *vector_e, double *temp_vec) {
  int dimension;
  
    dimension = number_of_nodes + group2_elements; 
  
    if(matrix_sparsity == SPARSE) {
	memset(temp_vec, 0, dimension * sizeof(double));
	cs_gaxpy(C2, vector_x, temp_vec); //temp_vec = C * x <=> temp_vec = (1/h * C) * x
    }
    else {
	cblas_dgemv(CblasRowMajor, CblasNoTrans, dimension, dimension, 1.0, matrix_C, dimension, vector_x, 1, 0.0, temp_vec, 1); //temp_vec = C * x <=> temp_vec = (1/h * C) * x
    }
	    
    cblas_daxpy(dimension, 1.0, temp_vec, 1, vector_e, 1); // e = e + temp_vec
    memcpy(vector_b, vector_e, dimension * sizeof(double)); // b = e <=> b = e + (1/h * C) * x
  
}


