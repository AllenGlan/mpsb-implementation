#include <common.hpp>

#include <vector>

#ifdef WITH_HCONLIB
#include <Winval.h>
#include <HGraf.h>
#endif // WITH_HCONLIB

/* 
 * Håkon Flatval, 2018: outer_triangle_partition.hpp/cpp
 * 
 * In the algorithm by Abrahamsen et al, we will have to solve the following subproblem:
 * 
 * Given a convex polygon with vertices P_0, P_1 ... P_n, define D_i to be the triangle between 
 * P_(i - 1), P_i, P_(i + 1), with indices mod n, where P_i is defined to be on the outside of D_i, 
 * while the two other vertices are inside. Notice now that any point, both on the hull 
 * and inside the polygon, can at most belong to two of the triangles D_i.
 * The problem is to create the sets T_i, the set of points (among all points), that are inside D_i
 * for each triangle index i.
 */

void outer_triangle_partition(dPoint2* allPoints, int numAllPoints,
			      dPoint2* polygon, int numPolygonPoints,
			      std::vector< std::vector<dPoint2> >& returnSets);

#ifdef WITH_HCONLIB
void setupVisualization(Winval* win, hg::Canvas* vas,
			int ww, int hh);
#endif // WITH_HCONLIB
