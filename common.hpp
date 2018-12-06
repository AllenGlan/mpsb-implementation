
// Double version of Point2
struct dPoint2 {
  union {
    double x, y;
    double p[2];
  };
  dPoint2(double a, double b);
  double& operator[](int a);
  dPoint2 operator+(const dPoint2& p2) const;
  dPoint2 operator-(const dPoint2& p2) const;
  dPoint2 operator*(float d) const;
};

double cross(const dPoint2& p1, const dPoint2& p2);
double dot(const dPoint2& p1, const dPoint2& p2);

bool compare_on_x(const dPoint2& p, const dPoint2& q);
bool turning_left(const double* p0, const double* p1, const double* p2);
bool turning_left(const dPoint2& p0, const dPoint2& p1, const dPoint2& p2);
void convex_hull(double* source, const int sourceCount,
		 double* dest, int* destCount);

