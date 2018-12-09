#if WITH_HCONLIB
#include <common.hpp>

#include <HGraf.h>
#include <Winval.h>

void compute_max_dims(const dPoint2* points, int NUM_PTS);

int screenX(double x);

// NB: Now actually corrects for axis inversion
int screenY(double y);

void highlight_point(const dPoint2& p1, int c = 0xFF00FF);


void initialize_visualization(Winval* winval, hg::Canvas* canvas,
			      int w, int h);

void clear_canvas();

void draw_line(const dPoint2& p1, const dPoint2& p2, int c = 0xFFFF00);

void draw_hull(const dPoint2* ps, int num, int c = 0xFFFFFF);

void show();


void wait();

#endif // WITH_HCONLIB
