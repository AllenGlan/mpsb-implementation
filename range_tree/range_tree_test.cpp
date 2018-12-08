#include <random>
#include <algorithm>

#include <range_tree.hpp>
#include <common.hpp>


#if WITH_HCONLIB

#include <HGraf.h>
#include <Winval.h>

Winval* win;
hg::Canvas* vas;
int width, height;

double maxx, minx,
  maxy, miny;


void compute_max_dims(const dPoint2* points, int NUM_PTS) {
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

void highlight_point(const dPoint2& p1) {
  for(int i = -1; i < 2; i++) {
    for(int j = -1; j < 2; j++) {
      (*vas)[(screenY(p1.y) + i)* width + screenX(p1.x) + j] = 0xFF00FF;
    }
  }
}


void initialize_visualization(Winval* winval, hg::Canvas* canvas,
			      int w, int h) {
  win = winval;
  vas = canvas;
  width = w, height = h;
}

void clear_canvas() {
  for(int i = 0; i< width * height; i++) {
    (*vas)[i] = 0;
  }
}

void draw_hull(const dPoint2* ps, int num) {
  for(int i = 0; i < num; i++) {
    hg::drawLine(vas->getData(), width, height,
		 screenX(ps[i].x),
		 screenY(ps[i].y),
		 screenX(ps[(i + 1)%num].x),
		 screenY(ps[(i + 1)%num].y),
		 0xFFFFFF);
  }
}

void show() {
  win->drawBuffer(vas->getData(), width, height);
}


void wait() {
  win->waitForKey();
}
#endif // WITH_HCONLIB


#define NUM_PTS 6000

dPoint2 points[NUM_PTS];


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

  dPoint2 halfplane_dir(21.0, 10.0);

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
			   4000, 15000,
			   5000, 8000,
			   dPoint2(6000, 10000), true,
			   subtrees);

#if WITH_HCONLIB

  for(int i = 0; i < NUM_PTS; i++) {
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
}
