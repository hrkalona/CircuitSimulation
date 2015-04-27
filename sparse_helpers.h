#include "Sparse/CXSparse/Include/cs.h"


/* y = trans(A) * x + y */
int cs_gaTxpy (const cs_di *A, const double *x, double *y);

/* y = hermitian(A) * x + y */
int cs_gaHxpyComplex (const cs_ci *A, const double complex *x, double complex *y);

/* w=A*v */
void smatrxvect(cs_di *A,double *v,double *w,unsigned int n,unsigned int m);

/* w=alpha*A*v */
void ssmxv(cs_di *A,double alpha,double *v,double *w,unsigned int n,unsigned int m);

/* w=trans(A)*v */
void smatrtxvect(cs_di *A,double *v,double *w,unsigned int n,unsigned int m);

/* w=alpha*A*v+u */
void ssmxvpv(cs_di *A,double alpha,double *v,double *u,double *w,unsigned int n,unsigned int m);

/* w=alpha*A*v+beta*u */
void ssmxvpsv(cs_di *A,double alpha,double *v,double *u,double beta,double *w,unsigned int n,unsigned int m);

double getElementAt(const cs_di *A, int i, int j);
double complex getElementAtComplex(const cs_ci *A, int i, int j);
