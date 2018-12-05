/* main.h   Jack Snoeyink    Common tangent without a separating line
 *
 */
#include <stdio.h>
#include <math.h>

#define MAXPTS 2000              /* Maximum number of points per polyline */
#define EPSILON 1.0e-13         /* Approximation of zero */
#define SQRhf M_SQRT1_2
#define TWO_PI 2*M_PI

typedef double COORD;
typedef COORD POINT[2];         /* Most data is Cartesian points */
typedef COORD HOMOG[3];         /* Some partial calculations are Homogeneous */

#define XX 0
#define YY 1
#define WW 2

typedef struct Polygon { 
  int n,                        /* Number of vertices in polygon */
  st, end,			/* Tangent is in $(\st, \end]$ */
  tent,                         /* Index of tentative refinement if $\tent \ne \st$ */
  ccw,				/* 1 = ccw  -1 = cw */
  wrap;                         /* Boolean indicates wraparound */
  HOMOG tang;			/* Tangent $\tau_\tent$ when refined ($\tent\ne\st$) */
  POINT v[MAXPTS];
} Polygon;

#define DET2x2(p, q, i, j) ((p)[i]*(q)[j] - (p)[j]*(q)[i]) /* Determinants */
#define DET2(p, q) DET2x2(p,q, XX,YY) 
#define DET3C(p, q, r) DET2(q,r) - DET2(p,r) + DET2(p,q)

#define CROSSPROD_2CCH(p, q, r) /* 2-d Cartesian to Homogeneous cross product */\
  (r)[XX] = - (q)[YY] + (p)[YY];\
  (r)[YY] =   (q)[XX] - (p)[XX]; (r)[WW] = DET2(p,q); 

#define CROSSPROD_2SCCH(s, p, q, r) /* 2-d cartesian to homog cross product with sign */\
  (r)[XX] = s * (- (q)[YY] + (p)[YY]); \
  (r)[YY] = s * (  (q)[XX] - (p)[XX]);  (r)[WW] = s * DET2(p,q);

#define DOTPROD_2CH(p, q)       /* 2-d Cartesian to Homogeneous dot product */\
  ((q)[WW] + (p)[XX]*(q)[XX] + (p)[YY]*(q)[YY])

#define ASSIGN_H(p, op, q)	/* Homogeneous assignment */\
  (p)[WW] op (q)[WW];  (p)[XX] op (q)[XX];  (p)[YY] op (q)[YY];

#define LEFT(x) (pr(x, " LEFT=%g ") > EPSILON) /* Sidedness tests */
#define RIGHT(x) (pr(x, " RT=%g ") < -EPSILON)
#define LEFT_PL(p, l) LEFT(DOTPROD_2CH(p, l))
#define RIGHT_PL(p, l) RIGHT(DOTPROD_2CH(p, l))
#define LEFT_PPP(p, q, r) LEFT(DET3C(p, q, r))
#define RIGHT_PPP(p, q, r) RIGHT(DET3C(p, q, r))

extern int gr_verbose, verbose; /* debugging */
extern double pr(double u, const char* c);

void Tang(Polygon* P, Polygon* Q);
