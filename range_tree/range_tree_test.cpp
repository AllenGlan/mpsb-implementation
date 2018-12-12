#include <random>
#include <algorithm>

#include <range_tree.hpp>
#include <common.hpp>
#include <drawing.hpp>

// For performance analysis
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>

#define PTS_FILE ("pts_file.csv")
#define PERF_FILE ("perf_file.csv")

#define NUM_PTS_MAX 100000
#define FIND_REPEATS 100000

dPoint2 points[NUM_PTS_MAX];


int main(int argc, const char** argv) {
  // random point generation
  /*
  if(argc > 1) {
    srand(atoi(argv[1]));
  } else {
    srand(12345);
  }

  for(int i = 0; i <NUM_PTS_MAX; i++) {
    points[i] = dPoint2((rand() % 13141),
			(rand() % 12419));
  }
  */

  // read points from file
  int num_pts = 0;

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
  
#if WITH_HCONLIB
  const int width = 600, height = 600;

  Winval win(width, height);
  hg::Canvas vas(width, height);
  
  for(int i = 0; i < width* height ; i++) {
    vas[i] = 0;
  }

  compute_max_dims(points, num_pts);

  initialize_visualization(&win, &vas, width, height);

#endif // WITH_HCONLIB

  std::function<bool(const dPoint2& p1, const dPoint2& p2)> x_comparator =
    [](const dPoint2& p1, const dPoint2& p2) {return p1.x < p2.x;};
  std::function<bool(const dPoint2& p1, const dPoint2& p2)> y_comparator =
    [](const dPoint2& p1, const dPoint2& p2) {return p1.y < p2.y;};

  dPoint2 halfplane_dir(1.0, 1.0);

  std::function<bool(const dPoint2& p1, const dPoint2& p2)> halfplane_comparator =
    [=](const dPoint2& p1, const dPoint2& p2) {
    return dot(halfplane_dir, p1) < dot(halfplane_dir, p2); };

  std::function<bool(const dPoint2& p1, const dPoint2& p2)> comparators[3] = {
    x_comparator,
    y_comparator,
    halfplane_comparator
  };

  // Don't forget to sort!!

  std::sort(points, points + num_pts, comparators[0]);
  
  auto build_start = std::chrono::high_resolution_clock::now();
  RangeTree* tree = build_tree(3, comparators, points, num_pts);
  auto build_stop = std::chrono::high_resolution_clock::now();
  double build_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(build_stop - build_start).count();
  
  std::vector<RangeTree*> subtrees;

  auto find_start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < FIND_REPEATS; i++) {
    range_tree_find_subtrees(tree,
	  		   -500, 500,
	  		   -500, 500,
	  		   dPoint2(0, 0), true,
	  		   subtrees);
  }
  auto find_stop = std::chrono::high_resolution_clock::now();
  double find_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(find_stop - find_start).count() / FIND_REPEATS;

#if WITH_HCONLIB

  for(int i = 0; i < num_pts; i++) {
    vas[screenX(points[i].x) + screenY(points[i].y) * width] = 0x00FF00;
  }
  
  
  for(int i = 0; i< subtrees.size(); i++) {
    for(int j = 0; j < subtrees[i]->numPoints; j++) {
      highlight_point(subtrees[i]->points[j]);
      
      // win.drawBuffer(vas.getData(), width, height);
      // printf("Drew %f, %f\n", subtrees[i]->points[j].x, subtrees[i]->points[j].y);
      // win.waitForKey();
      
    }
    
    for(int j = 0; j < subtrees[i]->hullSize; j++) {
      hg::drawLine(vas.getData(), width, height,
		   screenX(subtrees[i]->convexHull[j].x),
		   screenY(subtrees[i]->convexHull[j].y),
		   screenX(subtrees[i]->convexHull[(j + 1) % subtrees[i]->hullSize].x),
		   screenY(subtrees[i]->convexHull[(j + 1) % subtrees[i]->hullSize].y),
		   0xFFFFFF);
    }
    vas[screenY(subtrees[i]->convexHull[0].y) * width + screenX(subtrees[i]->convexHull[0].x)] = 0xFFFFFF;
  }


  // Draw loop, listen for exit/escape key
  while(win.isOpen() && !win.isKeyPressed(WK_ESC)) {
    win.drawBuffer(vas.getData(), width, height);
    win.sleepMilliseconds(50);
  }

  destroy_tree(tree);

#endif // WITH_HCONLIB

  // write and print performance information
  std::ofstream perf_file;
  perf_file.open(PERF_FILE);
  perf_file << "build" << "," << build_duration << std::endl;
  perf_file << "find" << "," << find_duration << std::endl;
  perf_file.close();

  std::cout << "Time: Build: " << build_duration << " ns" << std::endl;
  std::cout << "Time: Find: " << find_duration << " ns" << std::endl;
}