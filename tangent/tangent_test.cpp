/*
 * Håkon Flatval 2018
 * 
 * Source file to demonstrate and visualize the workings of 
 * the tangent.c file, which is provided in the paper
 * Computing Common Tangents Without a Separating Line
 * by David Kirkpatrick and Jack Snoeyink.
 */

#ifdef WITH_HCONLIB
#include "HGraf.h"
#include "Winval.h"
#endif // WITH_HCONLIB

#include "tangent.h"

#include <cstdio>
#include <algorithm>
#include <random>


// Double version of dPoint2
typedef struct dPoint2 {
  double x;
  double y;
  double& operator[](int a);
} dPoint2;

double& dPoint2::operator[](int a){
  if( a == 0) {
    return x;
  } else {
    return y;
  }
}

int xToDisplay(double x, int w) {
  return (int)(((x + 2 * 1024)+ 500) / (5. * 1024  + 1000) * w);
}

int yToDisplay(double y, int h) {
  return (int)(((y + 1024) + 500) / (2048  + 1000) * h);
}

bool comp(const dPoint2& p, const dPoint2& q) {
  return p.x < q.x;
}

bool turning_left(const double* p0, const double* p1, const double* p2) {
  double v0[2], v1[2];

  v0[0] = p1[0] - p0[0];
  v0[1] = p1[1] - p0[1];

  v1[0] = p2[0] - p1[0];
  v1[1] = p2[1] - p1[1];

  return v0[0] * v1[1] - v0[1] * v1[0] > 0;
}

bool turning_left(const dPoint2& p0, const dPoint2& p1, const dPoint2& p2) {
  return turning_left((const double*)&p0, (const double*)&p1, (const double*)&p2);
}

void convex_hull(double* source, const int sourceCount,
		 double* dest, int* destCount) {
  dPoint2* sourceA = (dPoint2*) source;
  dPoint2* destA = (dPoint2*) dest;
  
  for(int i = 0; i < sourceCount; i++ ) {
    if(*destCount < 2) {
      destA[*destCount][0] = sourceA[i][0];
      destA[*destCount][1] = sourceA[i][1];
      (*destCount)++;
      continue;
    }
    if(turning_left(destA[(*destCount) - 2], destA[(*destCount) - 1], sourceA[i])) {
      destA[*destCount][0] = sourceA[i][0];
      destA[*destCount][1] = sourceA[i][1];
      (*destCount)++;
    } else {
      (*destCount)--;
      i--; // Do this one again
    }
  }

  // Insert second to last point, to correctly handle case where
  // the hull goes (n - 2) -> (n - 1) -> (n - 2)
  destA[*destCount][0] = sourceA[sourceCount - 2][0];
  destA[*destCount][1] = sourceA[sourceCount - 2][1];
  (*destCount)++;

  for(int i = sourceCount - 3; i >= 0; i--) {
    if(turning_left(destA[(*destCount) - 2], destA[(*destCount) - 1], sourceA[i])) {
      destA[*destCount][0] = sourceA[i][0];
      destA[*destCount][1] = sourceA[i][1];
      (*destCount)++;
    } else {
      (*destCount)--;
      i++;
    }
  }

  // Let's not count first one twice
  (*destCount)--;
}

int main(int argc, const char ** argv) {

  // Setup window, canvas and random number generator
  const int w = 600, h = 600;

  if(argc > 1) {
    srand(atoi(argv[1]));
  }else{
    srand(12345);
  }
  
#ifdef WITH_HCONLIB
  Winval win(w, h);
  hg::Canvas vas(w, h);

  for(int i = 0; i < w * h; i++) {
    vas[i] = 0;
  }
#endif // WITH_HCONLIB
  
  // Fill in the polygons
  Polygon P, Q;

  P.n = 31; // MAXPTS / 5;
  Q.n = 16; // MAXPTS / 5 ;

  for(int i = 0; i < P.n; i++) {
    P.v[i][0] = ((rand() % 2048) - 2 * 1024) ;
    P.v[i][1] = ((rand() % 2048) - 1024) ;
  }

  for(int i = 0; i < Q.n; i++) {
    Q.v[i][0] = ((rand() % 2048) + 1024) ;
    Q.v[i][1] = ((rand() % 2048) - 1024);
  }

  // Compute convex hulls
  
  // Sort points by x coordinate
  std::sort((dPoint2*)(P.v), (dPoint2*)(P.v) + P.n, comp);
  std::sort((dPoint2*)(Q.v), (dPoint2*)(Q.v) + Q.n, comp);

  // Polygons holding convex hulls
  Polygon pc, qc;

  pc.n = 0, qc.n = 0;
  
  convex_hull((double*) P.v, P.n,
	      (double*) pc.v, &pc.n);

  convex_hull((double*) Q.v, Q.n,
	      (double*) qc.v, &qc.n);

  
  // Compute common tangent
  Tang(&pc, &qc);

  // The rest is visualizing the result (which won't happen if you don't use HCONLIB)

#ifdef WITH_HCONLIB
  // Draw tangent
  int px = xToDisplay(pc.v[pc.end % pc.n][0], w),
    py = yToDisplay(pc.v[pc.end % pc.n][1], h),
    qx = xToDisplay(qc.v[qc.end % qc.n][0], w),
    qy = yToDisplay(qc.v[qc.end % qc.n][1], h);
  hg::drawLine(vas.getData(), w, h, px, py, qx, qy, 0xFFFFFF);

  // Draw points in P
  for(int i = 0; i < P.n; i++) {
    int x = xToDisplay(P.v[i][0], w);
    int y = yToDisplay(P.v[i][1], h);

    vas[y * w + x] = 0xFFFFFF;
  }

  // Draw points in Q
  for(int i = 0; i < Q.n; i++) {
    int x = xToDisplay(Q.v[i][0], w);
    int y = yToDisplay(Q.v[i][1], h);

    vas[y * w + x] = 0xFFFFFF;
  }
  
  // Draw convex hull of pc
  for(int i = 0; i < pc.n; i++ ) {
    int x = xToDisplay(pc.v[i][0], w);
    int y = yToDisplay(pc.v[i][1], h);

    if(i < pc.n - 1) {
      hg::drawLine(vas.getData(), w, h, x, y, xToDisplay(pc.v[i + 1][0], w),
		   yToDisplay(pc.v[i + 1][1], h),
		   0x00FF00);
    }
  }
  // Also draw last line to complete polygon
  hg::drawLine(vas.getData(), w, h, xToDisplay(pc.v[pc.n - 1][0], w),
	       yToDisplay(pc.v[pc.n - 1][1], h),
	       xToDisplay(pc.v[0][0], w),
	       yToDisplay(pc.v[0][1], h),
	       0x00FF00);
					      

  // Draw Convex hull  of qc
  for(int i = 0; i < qc.n; i++ ) {
    int x = xToDisplay(qc.v[i][0], w);
    int y = yToDisplay(qc.v[i][1], h);

    if(i < qc.n - 1) {
      hg::drawLine(vas.getData(), w, h, x, y, xToDisplay(qc.v[i + 1][0], w),
		   yToDisplay(qc.v[i + 1][1], h),
		   0x00CCFF);
    }
  }
  // Also draw last line
  hg::drawLine(vas.getData(), w, h, xToDisplay(qc.v[qc.n - 1][0], w),
	       yToDisplay(qc.v[qc.n - 1][1], h),
	       xToDisplay(qc.v[0][0], w),
	       yToDisplay(qc.v[0][1], h),
	       0x00CCFF);
  

  
  // Draw loop, listen for exit/escape key
  while(win.isOpen() && !win.isKeyPressed(WK_ESC)) {
    win.drawBuffer(vas.getData(), w, h);
    win.sleepMilliseconds(50);
  }
#endif // WITH_HCONLIB
}
