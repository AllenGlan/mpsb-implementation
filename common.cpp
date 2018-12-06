#include <algorithm>
#include <common.hpp>

// dPoint2 functions and operators

dPoint2::dPoint2(double x, double y) {
  this->x = x;
  this->y = y;
}

double& dPoint2::operator[](int a){
  return this->p[a];
}

dPoint2 dPoint2::operator+(const dPoint2& p2) const {
  return dPoint2(x + p2.x, y + p2.y);
}

dPoint2 dPoint2::operator-(const dPoint2& p2) const {
  return dPoint2(x - p2.x, y - p2.y);
}

dPoint2 dPoint2::operator*(float d) const {
  return dPoint2(d * x, d * y);
}

double cross(const dPoint2& p1, const dPoint2& p2) {
  return p1.x * p2.y - p1.y * p2.x;
}

double dot(const dPoint2& p1, const dPoint2& p2) {
  return p1.x * p2.x + p1.y * p2.y;
}


// Geometric utilities

bool compare_on_x(const dPoint2& p, const dPoint2& q) {
  return p.x < q.x;
}

bool turning_left(const double* p0, const double* p1, const double* p2) {
  double v0[2], v1[2];

  v0[0] = p1[0] - p0[0];
  v0[1] = p1[1] - p0[1];

  v1[0] = p2[0] - p1[0];
  v1[1] = p2[1] - p1[1];

  return v0[0] * v1[1] - v0[1] * v1[0] > 0;
}

bool turning_left(const dPoint2& p0, const dPoint2& p1, const dPoint2& p2) {
  return turning_left((const double*)&p0, (const double*)&p1, (const double*)&p2);
}

void convex_hull(double* source, const int sourceCount,
		 double* dest, int* destCount) {
  dPoint2* sourceA = (dPoint2*) source;
  dPoint2* destA = (dPoint2*) dest;

  std::sort(sourceA, sourceA + sourceCount, compare_on_x);
  
  for(int i = 0; i < sourceCount; i++ ) {
    if(*destCount < 2) {
      destA[*destCount][0] = sourceA[i][0];
      destA[*destCount][1] = sourceA[i][1];
      (*destCount)++;
      continue;
    }
    if(turning_left(destA[(*destCount) - 2], destA[(*destCount) - 1], sourceA[i])) {
      destA[*destCount][0] = sourceA[i][0];
      destA[*destCount][1] = sourceA[i][1];
      (*destCount)++;
    } else {
      (*destCount)--;
      i--; // Do this one again
    }
  }

  // Insert second to last point, to correctly handle case where
  // the hull goes (n - 2) -> (n - 1) -> (n - 2)
  destA[*destCount][0] = sourceA[sourceCount - 2][0];
  destA[*destCount][1] = sourceA[sourceCount - 2][1];
  (*destCount)++;

  for(int i = sourceCount - 3; i >= 0; i--) {
    if(turning_left(destA[(*destCount) - 2], destA[(*destCount) - 1], sourceA[i])) {
      destA[*destCount][0] = sourceA[i][0];
      destA[*destCount][1] = sourceA[i][1];
      (*destCount)++;
    } else {
      (*destCount)--;
      i++;
    }
  }

  // Let's not count first one twice
  (*destCount)--;
}

