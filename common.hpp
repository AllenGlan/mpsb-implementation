
#pragma once

// Double version of Point2
struct dPoint2 {
  union {
    struct {double x, y;} ;
    double p[2];
  };
  dPoint2();
  dPoint2(double a, double b);
  double& operator[](int a);
  dPoint2 operator+(const dPoint2& p2) const;
  dPoint2 operator-(const dPoint2& p2) const;
  dPoint2 operator*(float d) const;
  bool operator!=(const dPoint2& p2) const;
  bool operator==(const dPoint2& p2) const;
};

double cross(const dPoint2& p1, const dPoint2& p2);
double dot(const dPoint2& p1, const dPoint2& p2);

bool compare_on_x(const dPoint2& p, const dPoint2& q);
bool turning_left(const double* p0, const double* p1, const double* p2);
bool turning_left(const dPoint2& p0, const dPoint2& p1, const dPoint2& p2);
void convex_hull(dPoint2* source, const int sourceCount,
		 dPoint2* dest, int* destCount);

