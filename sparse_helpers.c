#include "sparse_helpers.h"
#include <string.h>
/* y = AT*x+y */
int cs_gaTxpy (const cs_di *A, const double *x, double *y)
{
    int p, i, n, *Ap, *Ai;
    double *Ax ;
    
    if (!CS_CSC (A) || !x || !y) return (0) ;       /* check inputs */
    n = A->n ; Ap = A->p ; Ai = A->i ; Ax = A->x ;
    for(i = 0; i < n; i++) {
	for(p = Ap[i]; p < Ap[i + 1]; p++) {
	    y[i] += Ax[p] * x[Ai[p]]; 
	}
    }
    
    return (1) ;
}


/* y = AH*x+y */
int cs_gaHxpyComplex (const cs_ci *A, const double complex *x, double complex *y)
{
    int p, i, n, *Ap, *Ai;
    double complex *Ax ;
    
    if (!CS_CSC (A) || !x || !y) return (0) ;       /* check inputs */
    n = A->n ; Ap = A->p ; Ai = A->i ; Ax = A->x ;
    for(i = 0; i < n; i++) {
	for(p = Ap[i]; p < Ap[i + 1]; p++) {
	    y[i] += conj(Ax[p]) * x[Ai[p]]; 
	}
    }
    
    return (1) ;
}


double getElementAt(const cs_di *A, int i, int j) {
  
    int p, *Ai, *Ap;
    
    Ai = A -> i; Ap = A -> p;
    for(p = Ap[j]; p < Ap[j + 1]; p++) {
	if(Ai[p] == i) {
	    return A -> x[p];
	}
    }
    
    return 0;
  
}

double complex getElementAtComplex(const cs_ci *A, int i, int j) {
  
    int p, *Ai, *Ap;
    
    Ai = A -> i; Ap = A -> p;
    for(p = Ap[j]; p < Ap[j + 1]; p++) {
	if(Ai[p] == i) {
	    return A -> x[p];
	}
    }
    
    return 0;
  
}


/* w=A*v */
void smatrxvect(cs_di *A,double *v,double *w,unsigned int n,unsigned int m) {
	int j,p;

	memset(w,0,n*sizeof(double));

	for(j=0;j<n;j++)
		for(p=A->p[j];p<A->p[j+1];p++)
			w[A->i[p]]=w[A->i[p]]+A->x[p]*v[j];
}


/* w=alpha*A*v */
void ssmxv(cs_di *A,double alpha,double *v,double *w,unsigned int n,unsigned int m) {
	int j,p;

	memset(w,0,n*sizeof(double));

	for(j=0;j<n;j++)
		for(p=A->p[j];p<A->p[j+1];p++)
			w[A->i[p]]=w[A->i[p]]+A->x[p]*alpha*v[j];
}


/* w=trans(A)*v */
void smatrtxvect(cs_di *A,double *v,double *w,unsigned int n,unsigned int m) {
	int j,p;

	for(j=0;j<m;j++)
	{
		w[j]=0.0;
		for(p=A->p[j];p<A->p[j+1];p++)
			w[j]=w[j]+A->x[p]*v[A->i[p]];

	}
}

/* w=alpha*A*v+u */
void ssmxvpv(cs_di *A,double alpha,double *v,double *u,double *w,unsigned int n,unsigned int m) {
	int j,p;

	memcpy(w,u,n*sizeof(double));

	for(j=0;j<n;j++)
		for(p=A->p[j];p<A->p[j+1];p++)
			w[A->i[p]]=w[A->i[p]]+A->x[p]*alpha*v[j];
}



/* w=alpha*A*v+beta*u */
void ssmxvpsv(cs_di *A,double alpha,double *v,double *u,double beta,double *w,unsigned int n,unsigned int m) {
		int j,p;

		for(j=0;j<n;j++)
			w[j]=beta*u[j];
		for(j=0;j<n;j++)
			for(p=A->p[j];p<A->p[j+1];p++)
				w[A->i[p]]=w[A->i[p]]+A->x[p]*alpha*v[j];

}