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

#include <common.hpp>

#include "tangent.h"

#include <cstdio>
#include <random>


int xToDisplay(double x, int w) {
  return (int)(((x + 2 * 1024)+ 500) / (5. * 1024  + 1000) * w);
}

int yToDisplay(double y, int h) {
  return (int)(((y + 1024) + 500) / (2048  + 1000) * h);
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

  // Polygons holding convex hulls
  Polygon pc, qc;

  pc.n = 0, qc.n = 0;
  
  convex_hull((dPoint2*) P.v, P.n,
	      (dPoint2*) pc.v, &pc.n);

  convex_hull((dPoint2*) Q.v, Q.n,
	      (dPoint2*) qc.v, &qc.n);

  
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
