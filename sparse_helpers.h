#include "Sparse/CXSparse/Include/cs.h"


/* y = trans(A) * x + y */
int cs_gaTxpy (const cs_di *A, const double *x, double *y);

/* y = hermitian(A) * x + y */
int cs_gaHxpyComplex (const cs_ci *A, const double complex *x, double complex *y);

/* w=alpha*A*v+u */
void ssmxvpv(cs_di *A,double alpha,double *v,double *u,double *w,unsigned int n,unsigned int m);


double getElementAt(const cs_di *A, int i, int j);
double complex getElementAtComplex(const cs_ci *A, int i, int j);
