
#include <drawing.hpp> 

#include <combine_multiple_hulls.cpp> // Commiting war crimes because that is what we are now
#include <range_tree.hpp>


#include <functional>


#define NUM_PTS 40
dPoint2 points[NUM_PTS];

// Borrowed from range_tree_test.cpp
int main(int argc, const char** argv) {
  if(argc > 1) {
    srand(atoi(argv[1]));
  } else {
    srand(12345);
  }

  for(int i = 0; i <NUM_PTS; i++) {
    points[i] = dPoint2((rand() % 13141),
			(rand() % 12419));
  }
  
#if WITH_HCONLIB
  const int width = 600, height = 600;

  Winval win(width, height);
  hg::Canvas vas(width, height);
  
  for(int i = 0; i < width* height ; i++) {
    vas[i] = 0;
  }

  compute_max_dims(points, NUM_PTS);

  initialize_visualization(&win, &vas, width, height);

#endif // WITH_HCONLIB
  

  std::function<bool(const dPoint2& p1, const dPoint2& p2)> x_comparator =
    [](const dPoint2& p1, const dPoint2& p2) {return p1.x < p2.x;};
  std::function<bool(const dPoint2& p1, const dPoint2& p2)> y_comparator =
    [](const dPoint2& p1, const dPoint2& p2) {return p1.y < p2.y;};

  dPoint2 halfplane_dir(10.0, 10.0);

  std::function<bool(const dPoint2& p1, const dPoint2& p2)> halfplane_comparator =
    [=](const dPoint2& p1, const dPoint2& p2) {
    return dot(halfplane_dir, p1) < dot(halfplane_dir, p2); };

  std::function<bool(const dPoint2& p1, const dPoint2& p2)> comparators[3] = {
    x_comparator,
    y_comparator,
    halfplane_comparator
  };

  // Don't forget to sort!!

  std::sort(points, points + NUM_PTS, comparators[0]);
  
  RangeTree* tree = build_tree(3, comparators, points, NUM_PTS);

  std::vector<RangeTree*> subtrees;

  range_tree_find_subtrees(tree,
			   0, 15000,
			   0, 15000,
			   dPoint2(5000, 7000), false,
			   subtrees);
  
  std::vector<dPoint2*> hulls;
  std::vector<int> hullSizes;
  std::vector<double*> hullLengths;

  std::vector<dPoint2> convex_hull;
  std::vector<double> lengths;
  for(int i = 0; i < subtrees.size(); i++) {
    hulls.push_back(subtrees[i]->convexHull);
    hullSizes.push_back(subtrees[i]->hullSize);
    hullLengths.push_back(subtrees[i]->accHullLength);
  }

#if 1
  combine_multiple_convex_hulls(hulls,
				hullLengths,
				hullSizes,
				convex_hull, lengths);
#endif // comment
  
#if WITH_HCONLIB

  for(int i = 0; i < NUM_PTS; i++) {
    vas[screenX(points[i].x) + screenY(points[i].y) * width] = 0x00FF00;
  }
  
  
  for(int i = 0; i< subtrees.size(); i++) {
    for(int j = 0; j < subtrees[i]->numPoints; j++) {
      highlight_point(subtrees[i]->points[j]);
    }
    
    draw_hull(subtrees[i]->convexHull, subtrees[i]->hullSize, 0xFFFFFF);
    // vas[screenY(subtrees[i]->convexHull[0].y) * width + screenX(subtrees[i]->convexHull[0].x)] = 0xFFFFFF; 
  }
  
  show();
  wait();

  draw_hull(convex_hull.data(), convex_hull.size(), 0x0000FF);



  // Draw loop, listen for exit/escape key
  while(win.isOpen() && !win.isKeyPressed(WK_ESC)) {
    win.drawBuffer(vas.getData(), width, height);
    win.sleepMilliseconds(50);
  }

  destroy_tree(tree);

#endif // WITH_HCONLIB
				
}
