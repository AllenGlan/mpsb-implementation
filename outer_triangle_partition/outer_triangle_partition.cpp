#include <common.hpp>
#include <outer_triangle_partition.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>

// Look into the header file for explanation

// polygon_midpoint defined so that it is not contained in any triangle D_i (see the header)
dPoint2 polygon_midpoint;

// Vector of start angle, will define to be first point in convex hull
dPoint2 angle_zero;

bool lower_angle(const dPoint2& p0, const dPoint2& p1) {
  dPoint2 d1 = p0 - polygon_midpoint;
  dPoint2 d2 = p1 - polygon_midpoint;

  double c1 = cross(angle_zero, d1);
  double c2 = cross(angle_zero, d2);

  if (c1 >=  0 && c2 < 0) {
    return true;
  } else if (c2 >= 0 && c1 < 0) {
    return false;
  } else {
    return cross(d1, d2) >= 0;
  }
}

dPoint2 crossing(const dPoint2& p1, const dPoint2& p2,
		 const dPoint2& q1, const dPoint2& q2) {
  dPoint2 v = p2 - p1;
  dPoint2 u = q2 - q1;
  
  double s = ((p1.x - q1.x) * v.y - (p1.y - q1.y) * v.x) / (u.x * v.y - u.y * v.x);
  return dPoint2(q1.x + s * u.x, q1.y + s * u.y);
}

#if WITH_HCONLIB

Winval* win;
hg::Canvas* vas;

extern int screenX(double);
extern int screenY(double);

int width, height;

void setupVisualization(Winval* w, hg::Canvas* c,
			int ww, int hh) {
  win = w;
  vas = c;
  width = ww;
  height = hh;
}

void drawClear() {
  for(int i = 0; i < width * height; i++) {
    (*vas)[i] = 0;
  }
}

void drawTriangle(const dPoint2& p1, const dPoint2& p2, const dPoint2& p3) {
  static int color = 0xFFFFFF;
  hg::drawLine(vas->getData(), width, height, screenX(p1.x), screenY(p1.y),
	       screenX(p2.x), screenY(p2.y),
	       color);
  hg::drawLine(vas->getData(), width, height, screenX(p2.x), screenY(p2.y),
	       screenX(p3.x), screenY(p3.y),
	       color);
  hg::drawLine(vas->getData(), width, height, screenX(p1.x), screenY(p1.y),
	       screenX(p3.x), screenY(p3.y),
	       color);
}

void drawPoints(const dPoint2* points, int numPoints) {
  for(int i = 0; i < numPoints; i++) {
    (*vas)[width * screenY(points[i].y) + screenX(points[i].x)] = 0x00FF00;
  }
}

void drawPoint(const dPoint2& point) {
  for(int i = -1; i < 2; i++) {
    for(int j = -1; j < 2; j++) {
      (*vas)[width * (screenY(point.y) + i) + screenX(point.x) + j] = 0xFF00FF;
    }
  }
}

void show(){
  win->drawBuffer(vas->getData(), width, height);

  win->waitForKey();

  if(win->isKeyPressed(WK_ESC)) {
    exit(0);
  }
}

#endif // WITH_HCONLIB

// Points in the polygon (convex hull) array must be in counter-clockwise order
// (In a normal coordinate system, where y-axis goes upwards, x-axis right)

// NB: allPoints will be sorted in here, make sure that does not break anything!
void outer_triangle_partition(dPoint2* allPoints, int numAllPoints,
			      dPoint2* polygon, int numPolygonPoints,
			      std::vector< std::vector<dPoint2> >& returnSets) {
  
  // Compute "midpoint" of convex hull
  /*
   * This isn't entirely trivial, as we want the point to not be inside the triangle formed
   * by any three successive points on the polygon. 
   * The method we will use (for n >= 6), is to find two diagonals crossing about half the points each
   * and use their crossing. We solve the other cases individually.
   */

  if(numPolygonPoints <= 3) {
    returnSets.push_back(std::vector<dPoint2>());
    for(int i = 0; i < numAllPoints; i++) {
      returnSets[0].push_back(allPoints[i]);
    }
    return;
  } else if (numPolygonPoints == 4) {
    // This isn't really optimal, since the midpoint crosses all triangles, but oh well.
    polygon_midpoint = crossing(polygon[0], polygon[2],
				polygon[1], polygon[3]);
  } else if (numPolygonPoints == 5) {
    // Take midpoints on opposite edge of two first vertices
    dPoint2 p1 = (polygon[2] + polygon[3]) * 0.5;
    dPoint2 p2 = (polygon[3] + polygon[4]) * 0.5;
    
    polygon_midpoint = crossing(polygon[0], p1,
				polygon[1], p2);
  } else {
    // Pick four vertices reasonably well spaced
    int n1 = numPolygonPoints / 4,
      n2 = numPolygonPoints / 2,
      n3 = 3 * numPolygonPoints / 4;
    polygon_midpoint = crossing(polygon[0], polygon[n2],
				polygon[n1], polygon[n3]);
  }

  /*
   *Set start angle to first point in polygon
   */
  
  angle_zero = polygon[0] - polygon_midpoint;
  
  /*
   * Sort all points by angle around the midpoint of the polygon
   */
  
  std::sort(allPoints, allPoints + numAllPoints, lower_angle);

  /*
   * The algorithm itself:
   * We will traverse the triangles in counter-clockwise order, and simultaneously
   * traverse the points, which are sorted in counterclockwise order.
   * Two triangles are "active" at a time. We check each point against both triangles.
   * When we find the last point of one of the triangles, we start considering the next
   * triangle.
   */

  returnSets.push_back(std::vector<dPoint2>());
  
  // Polygon[0] should be among first points, but may not be THE first
  // All points ranked before polygon[0] cannot belong to other triangles
  // than the first, since they must lie on a line from the mid to polygon[0]
  int si = 0;
  while(polygon[0] != allPoints[si]) {
    returnSets[0].push_back(allPoints[si]);
    si++;
  }

  returnSets.push_back(std::vector<dPoint2>());
  returnSets[1].push_back(allPoints[si]);

  int currFirstTri = 0; // Index into the two following array, denoting which triangle is first
  int currStopVertices[2] = {1, 2}; // Vertices that will end the currently active triangles
  dPoint2 currSepDirs[2]; // The distance vectors to measure against for each triangle
  double currDists[2]; // The distance measure that must be superceded to be inside the triangle

  
  // Separation line for D_0
  dPoint2 sepLine = polygon[1] - polygon[numPolygonPoints - 1];
  currSepDirs[0] = dPoint2(sepLine.y, -sepLine.x);
  currDists[0] = dot(currSepDirs[0], polygon[numPolygonPoints - 1] - polygon_midpoint);

  // Separation line for D_1
  sepLine = polygon[2] - polygon[0];
  currSepDirs[1] = dPoint2(sepLine.y, -sepLine.x);
  currDists[1] = dot(currSepDirs[1], polygon[0] - polygon_midpoint);

  /*
   * Main loop:
   * For every point in the set, do the following check:
   * Is the point the next anticipated point on the polygon boundary?
   * If so, put this point into the set of the current first triangle, then insert a new set
   * for the triangle two steps ahead, and add the point to that as well.
   * Then, recompute the index of next anticipated hull vertex ( two steps ahead ),
   * and the dist and dir separation measures for that triangle
   * If the point is not the next anticipated one on the polygon boundary,
   * check for containment in the two current triangles
   */
  for(int i = si + 1; i < numAllPoints ; i++ ) {
    
    int polygonVertexIndex = currStopVertices[currFirstTri];
    int lastPolygonVertexIndex = (polygonVertexIndex + numPolygonPoints - 1) % numPolygonPoints;
    int last2PolygonVertexIndex = (polygonVertexIndex + numPolygonPoints - 2) % numPolygonPoints;
    int nextPolygonVertexIndex = (polygonVertexIndex + 1) % numPolygonPoints;
    int next2PolygonVertexIndex = (polygonVertexIndex + 2) % numPolygonPoints;
	
    
#if WITH_HCONLIB
    drawClear();
    drawTriangle(polygon[last2PolygonVertexIndex],
		 polygon[lastPolygonVertexIndex],
		 polygon[polygonVertexIndex]);
    drawPoints(allPoints, numAllPoints);
    drawPoint(allPoints[i]);
		  
#endif // WITH_HCONLIB

    
    if(allPoints[i] == polygon[ currStopVertices[currFirstTri] ]) {
      // This point is the end of a triangle, put it in this triangle and the next
      
      returnSets[ lastPolygonVertexIndex].push_back(allPoints[i]);

      returnSets.push_back(std::vector<dPoint2>());
      returnSets[ nextPolygonVertexIndex ].push_back(allPoints[i]);

      // Compute the values for the next triangle
      currStopVertices[currFirstTri] = next2PolygonVertexIndex;
      dPoint2 sepLine = polygon[ next2PolygonVertexIndex ] - polygon[ polygonVertexIndex ];
      currSepDirs[currFirstTri] = dPoint2(sepLine.y, -sepLine.x);
      currDists[currFirstTri] = dot(currSepDirs[currFirstTri], polygon[ polygonVertexIndex ] - polygon_midpoint);
      
      currFirstTri ^= 1;

#if WITH_HCONLIB
      // Draw the set of points found in the late triangle
      for(int j = 0; j < returnSets[lastPolygonVertexIndex].size(); j ++ ) {
        drawPoint(returnSets[lastPolygonVertexIndex][j]);
      }
#endif // WITH_HCONLIB
      
      
    } else { // Was not next boundary point, check if belongs in the current triangles
      // Use strict inequality to avoid duplicates (and cross fingers with a prayer)
      if ( dot(currSepDirs[currFirstTri], allPoints[i] - polygon_midpoint) > currDists[currFirstTri]) {
	returnSets[ lastPolygonVertexIndex ].push_back(allPoints[i]);
      }

      if ( dot(currSepDirs[currFirstTri ^ 1], allPoints[i] - polygon_midpoint) > currDists[currFirstTri ^ 1] ) {
	returnSets[ polygonVertexIndex ].push_back(allPoints[i]);
      }
    }
    
#if WITH_HCONLIB
    show();
#endif // WITH_HCONLIB
    
  }

  // The last triangle created is a duplicate of the first, empty because of our
  // swift use of modulo. We can safely delete it
  returnSets.pop_back();

  // The (newly) last triangle did not get its second endpoint, polygon[0]:
  returnSets[returnSets.size() - 1].push_back(polygon[0]);


  for(int i = 0; i < returnSets.size(); i++ ) {
    printf("ReturnSet %d size is %d\n", i, returnSets[i].size());
  }
}
