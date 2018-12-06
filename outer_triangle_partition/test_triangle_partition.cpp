#if WITH_HCONLIB

#include <HGraf.h>
#include <Winval.h>

#endif // WITH_HCONLIB

#include "outer_triangle_partition.hpp"

#include <random>
#include <algorithm>

#define NUM_PTS 132

dPoint2 points[NUM_PTS];
dPoint2 hull[NUM_PTS + 2]; // Add some padding for extra elements

const int width = 600, height = 600;

double maxx, minx,
  maxy, miny;


void compute_max_dims() {
  maxx = minx = points[0].x;
  maxy = miny = points[0].y;
  
  for(int i = 0; i < NUM_PTS; i++) {
    maxx = std::max(maxx, points[i].x);
    maxy = std::max(maxy, points[i].y);
    minx = std::min(minx, points[i].x);
    miny = std::min(miny, points[i].y);
  }
}

int screenX(double x) {
  return (int)(50 + (x - minx) / (maxx - minx) * (width - 100));
}

int screenY(double y) {
  return (int)(50 + (y - miny) / (maxy - miny) * (height - 100));
}

int main(int argc, const char ** argv) {

  if(argc >= 2) {
    srand(atoi(argv[1]));
  } else {
    srand(12345);
  }

#if WITH_HCONLIB
  Winval win(width, height);
  hg::Canvas vas(width, height);

  for(int i = 0; i < width * height; i++) {
    vas[i] = 0;
  }
  
#endif // WITH_HCONLIB
  
  for(int i = 0; i < NUM_PTS; i++ ){
    points[i] = dPoint2((rand() % 23113) ^ 1321,
			(rand() % 34082) ^ 1443);
  }

  compute_max_dims();

  int hullSize = 0;
  convex_hull(points, NUM_PTS,
	      hull, &hullSize);

#if WITH_HCONLIB
  setupVisualization(&win, &vas,
		     width, height);
#endif // WITH_HCONLIB


  std::vector<std::vector<dPoint2> > sets;
  outer_triangle_partition(points, NUM_PTS,
			   hull, hullSize,
			   sets);
  
#if WITH_HCONLIB
  
  
  for(int i = 0; i < NUM_PTS; i++) {
    vas[screenY(points[i].y) * width + screenX(points[i].x)] = 0x00FF00;
  }

  for(int i = 0; i < hullSize - 1; i++) {
    hg::drawLine(vas.getData(), width, height,
		 screenX(hull[i].x), screenY(hull[i].y),
		 screenX(hull[i + 1].x), screenY(hull[i + 1].y),
		 0xFF00FF);
		 
  }

  // Draw loop, listen for exit/escape key
  while(win.isOpen() && !win.isKeyPressed(WK_ESC)) {
    win.drawBuffer(vas.getData(), width, height);
    win.sleepMilliseconds(50);
  }

#endif // WITH_HCONLIB
  
}
