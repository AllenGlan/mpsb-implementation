#if WITH_HCONLIB

#include <HGraf.h>
#include <Winval.h>

#endif // WITH_HCONLIB

#include "outer_triangle_partition.hpp"

#include <random>
#include <algorithm>

// For performance analysis
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>

#define NUM_PTS_MAX (100000)
#define GEN_PTS (0)
#define PTS_FILE ("pts_file.csv")
#define PERF_FILE ("perf_file.csv")

dPoint2 points[NUM_PTS_MAX];
dPoint2 hull[NUM_PTS_MAX + 2]; // Add some padding for extra elements

const int width = 600, height = 600;

double maxx, minx,
  maxy, miny;


void compute_max_dims(int num_pts) {
  maxx = minx = points[0].x;
  maxy = miny = points[0].y;
  
  for(int i = 0; i < num_pts; i++) {
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

  if (GEN_PTS) {
    // random points generation
    for(int i = 0; i < NUM_PTS_MAX; i++ ){
      points[i] = dPoint2((rand() % 23113) ^ 1321,
  			(rand() % 34082) ^ 1443);
    }
    
    // output points to file
    std::ofstream pts_file;
    pts_file.open(PTS_FILE);
    for (int i = 0; i < NUM_PTS_MAX; i++){
      pts_file << points[i].x << "," << points[i].y << std::endl;
    }
    pts_file.close();
  }

  int num_pts = 0;

  // read points from file
  std::string line;
  std::ifstream pts_file PTS_FILE;
  if (pts_file.is_open()) {
    for (int i = 0; i < NUM_PTS_MAX; i++){
      if (getline(pts_file, line)) {
        std::stringstream ss(line);
        std::vector<double> pnt;
        while(ss.good()) {
          std::string coord;
          getline(ss, coord, ',');
          pnt.push_back(std::stod(coord));
        }
        points[i] = dPoint2(pnt[0], pnt[1]);
        num_pts++;
      } else {
        break;
      }
    }
    pts_file.close();
  } else {
    std::cout << "Unable to open " << PTS_FILE << "\n";
  }

  compute_max_dims(num_pts);

  int hullSize = 0;

  auto convex_hull_start = std::clock();
  convex_hull(points, num_pts,
	      hull, &hullSize);
  auto convex_hull_stop = std::clock();
  double convex_hull_duration = 1000.0 * (convex_hull_stop - convex_hull_start) / CLOCKS_PER_SEC;

#if WITH_HCONLIB
  setupVisualization(&win, &vas,
		     width, height);
#endif // WITH_HCONLIB

  std::vector<std::vector<dPoint2> > sets;

  auto outer_triangle_start = std::clock();
  outer_triangle_partition(points, num_pts,
			  hull, hullSize,
			  sets);
  auto outer_triangle_stop = std::clock();
  double outer_triangle_duration = 1000.0 * (outer_triangle_stop - outer_triangle_start) / CLOCKS_PER_SEC;

#if WITH_HCONLIB
  
  for(int i = 0; i < num_pts; i++) {
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

  // write and print performance information
  std::ofstream perf_file;
  perf_file.open(PERF_FILE);
  perf_file << "convex_hull" << "," << convex_hull_duration << std::endl;
  perf_file << "outer_triangle_partition" << "," << outer_triangle_duration << std::endl;
  perf_file.close();

  std::cout << "Time: Convex hull: " << convex_hull_duration << " ms" << std::endl;
  std::cout << "Time: Outer triangle partition: " << outer_triangle_duration << " ms" << std::endl;
}
