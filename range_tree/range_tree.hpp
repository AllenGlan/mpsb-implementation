#include <common.hpp>
#include <vector>
#include <functional>

#pragma once

struct RangeTree {
  RangeTree *left, *right, *next_level;
  int level;

  // bool (*less_than)(const dPoint2& p1, const dPoint2& p2);
  std::function<bool(const dPoint2& p1, const dPoint2& p2)> less_than;
  dPoint2 minPoint, maxPoint;
  
  
  // Only meaningful for level == 1
  dPoint2* points;
  int numPoints;

  dPoint2* convexHull;
  double* accHullLength;
  int hullSize;
};

RangeTree* build_tree(int numLevels,
		      //bool *(*less_than)(const dPoint2& p1, const dPoint2& p2),
		      std::function<bool(const dPoint2& p1, const dPoint2& p2)>* less_than,
		      const dPoint2 *points, int numPoints);
void range_tree_find_subtrees(RangeTree* t,
			      double minx, double maxx,
			      double miny, double maxy,
			      const dPoint2& halfplane_comp, bool halfplane_over,
			      std::vector<RangeTree*>& res);
void destroy_tree(RangeTree* t);

#if WITH_HCONLIB

#include <HGraf.h>
#include <Winval.h>

extern Winval* win;
extern hg::Canvas* vas;
extern int width, height;

extern double maxx, minx,
  maxy, miny;


void compute_max_dims(const dPoint2* points, int NUM_PTS) ;

int screenX(double x);

int screenY(double y);

void highlight_point(const dPoint2& p1);


void initialize_visualization(Winval* winval, hg::Canvas* canvas,
			      int w, int h);

void clear_canvas() ;
void draw_hull(const dPoint2* points, int num);
void show();

void wait() ;
#endif // WITH_HCONLIB
