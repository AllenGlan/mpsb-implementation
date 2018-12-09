#if WITH_HCONLIB
#include <common.hpp>
#include <HGraf.h>
#include <Winval.h>

#include <drawing.hpp>

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

// NB: Now actually corrects for axis inversion
int screenY(double y) {
  return (int)(50 + (maxy - y) / (maxy - miny) * (height - 100));
}

void highlight_point(const dPoint2& p1, int c) {
  for(int i = -1; i < 2; i++) {
    for(int j = -1; j < 2; j++) {
      (*vas)[(screenY(p1.y) + i)* width + screenX(p1.x) + j] = c;
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

void draw_line(const dPoint2& p1, const dPoint2& p2, int c) {
  hg::drawLine(vas->getData(), width, height,
	       screenX(p1.x),
	       screenY(p1.y),
	       screenX(p2.x),
	       screenY(p2.y),
	       c);
}

void draw_hull(const dPoint2* ps, int num, int c) {
  for(int i = 0; i < num; i++) {
    draw_line(ps[i], ps[(i + 1) % num], c);
  }
}

void show() {
  win->drawBuffer(vas->getData(), width, height);
}


void wait() {
  win->waitForKey();
}
#endif // WITH_HCONLIB
