/* nosep.c   Jack Snoeyink     Common tangent without a separating line
 */

/*
  December 5th 2018 - Håkon Flatval: Minor changes to remove dependence on old graphics libraries
*/

#include "tangent.h"

#define Pv(m) P->v[m]           /* Indexing into polygon vertices */
#define Qv(m) Q->v[m]
#define Pvr(m) P->v[(m) % P->n] /* Indexing mod $n$ */
#define Qvr(m) Q->v[(m) % Q->n]

#define CCW(x) (x->ccw == 1) /* Is $x$ oriented counterclockwise? */
#define DONE(x) ((x->end - x->tent) == x->ccw) /* Any candidates left? */
#define REFINED(x) (x->st != x->tent) /* Is $x$ refined? */

#define NO_DISC     -1           /* Actions in \op Refine() */
#define DISC_END     0
#define DISC_START   1

void Refine(Polygon* P, Polygon* Q)
/* We refine polygon $P$ checking against $Q$.  
We can assume that more than one candidates exist in $(\tentP,\enP\,]$
and the invariants hold. */
{
  HOMOG q0pm, mtang;
  register int mid, left_base, action = NO_DISC;
  register COORD *pm, *pm1, *qend, *qt;
  
  mid = P->tent + (P->end - P->tent) / 2; /* Check \mid\ point.
                                            Round towards \tentP\ */
  pm = Pvr(mid);
  pm1 = Pvr(mid + P->ccw);
  CROSSPROD_2SCCH(P->ccw, pm, pm1, mtang); /* Generate $\tau_\mid$ */
  CROSSPROD_2CCH(Qv(0), pm, q0pm);
  left_base = RIGHT_PL(Pv(0), q0pm); 


  /* debugging */
  /* if (verbose) {
    printf("Refine %x %x %3d %3d %3d %3d \n", 
	   P, Q, P->st, P->tent, mid, P->end);
    fflush(stdout);
    }*/
  
  if (REFINED(Q) && !LEFT_PL(pm, Q->tang)) {
    qt = Qvr(Q->tent);
    if (CCW(Q) ^ LEFT_PPP(Pv(0), qt, pm)) /* Check $\sigma_\tentQ$ */
      Q->st = Q->tent;          /* Certify tentative to $Q$ */
    else {
      Q->end = Q->tent; 
      Q->tent = Q->st;          /* Revoke tentative to $Q$ */
      P->st = P->tent;	/* Certify tentatve on $P$ (if refined) */
    }
  }

  qend = Qvr(Q->end);
  qt = Qvr(Q->tent);

  if (P->wrap && (left_base ^ (mid > P->n))) /* Is $P$ wrapped around? */
    action = !left_base;
  else if (!LEFT_PL(Qv(0), mtang)) /* Can we be tangent w.r.t $q_0$? */
    action = left_base;
  else if (!LEFT_PL(qend, mtang)) /* Be tangent w.r.t $q_\enQ$? */
    action = LEFT_PL(qend, q0pm);
  else if (REFINED(Q) && !LEFT_PL(qt, mtang)) /* Be tangent w.r.t $q_\tentQ$? */
    action = LEFT_PL(qt, q0pm);
  
  if (action == NO_DISC) {
    P->tent = mid;              /* We tentatively refine at \mid */        
    ASSIGN_H(P->tang, =, mtang)
    }
  else if (CCW(P) ^ action)
    P->st = P->tent = mid;      /* A discard at \stP\ occurred */
  else
    P->end = mid;               /* A discard at \enP\ occurred */
}



void Tang(Polygon*P, Polygon*Q)
{                               /* Compute a tangent from $P$ to $Q$ */
  register int n1 = Q->n - 1;

  P->ccw = 1; P->st = P->tent = 0; P->end = P->n; /* Initialize $P$ */
  CROSSPROD_2CCH(Pv(0), Pv(1), P->tang);
  if (P->wrap = LEFT_PL(Qv(0), P->tang)) /* Wrap $P$ initially */
    { P->tent = P->n; P->end += P->n; }

  Q->ccw = -1; Q->st = Q->tent = Q->n; Q->end = 0; /* Initialize $Q$ */
  CROSSPROD_2CCH(Qv(n1), Qv(0), Q->tang);
  if (Q->wrap = LEFT_PL(Pv(0), Q->tang)) /* Wrap $Q$ initially */
    Q->st += Q->n; 

  while (!DONE(P) || !DONE(Q)) {
    if (!DONE(P)) Refine(P, Q);
    if (!DONE(Q)) Refine(Q, P);
  }                             /* Finished. $\enQ$ and $\enP$ indicate tangent */
}

double pr(double r, const char* u) {
return r;
}
