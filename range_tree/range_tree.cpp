#include <range_tree.hpp>
#include <common.hpp>
#include <tangent.h>

#include <algorithm>
#include <vector>

#include <cstring>

static void combine_convex_hulls(dPoint2* hull1, int n1,
				 dPoint2* hull2, int n2,
				 dPoint2** res, int *resn){
  if(n1 == 1 && n2 == 1) {
    *res = new dPoint2[2];
    (*res)[0] = hull1[0];
    (*res)[1] = hull2[0];
    *resn = 2;
    return;
  }
  
  if(n2 == 1) {
    // NB: We here no longer guarantee non-degenerate convex hulls (some angles may be 180 degrees)
    // HMU if you know how to do that reasonably fast (I'm worried about the case where all points lie
    // on the same line in this if-block)
    int i = 0;
    while(!(cross( hull1[(i + 1) % n1] - hull1[i],
		  hull2[0] - hull1[(i + 1) % n1]) >= 0 &&
	    cross( hull1[(i + 2) % n1] - hull1[(i + 1) % n1],
		   hull2[0] - hull1[(i + 1) % n1]) <= 0)){
      i = (i + 1) % n1;
    }
    i = (i + 1) % n1;

    int j = 0;
    while(!(cross(hull1[(j + 1) % n1] - hull1[j],
		  hull2[0] - hull1[(j + 1)% n1]) <= 0 &&
	    cross( hull1[(j + 2) % n1] - hull1[(j + 1) % n1],
		   hull2[0] - hull1[(i + 1) % n1]) >= 0)) {
      j = (j + 1) % n1;
    }
    j = (j + 1) % n1;

    int s;
    if(i >= j) {
      s = i - j + 2;
    } else {
      s = n1 - (j - i) + 2;
    }

    *res = new dPoint2[s];

    int k = j, u = 0;
    while(k != i) {
      (*res)[u] = hull1[k];
      k = (k + 1) % n1;
      u++;
    }
    
    (*res)[u] = hull1[i];
    (*res)[u + 1] = hull2[0];
    *resn = s;
  } else if (n1 == 1) {
    combine_convex_hulls(hull2, n2,
			 hull1, n1,
			 res, resn);
  } else {
    Polygon P, Q;
    P.n = n1;
    for(int i = 0; i < n1; i++) {
      P.v[i][0] = hull1[i].x;
      P.v[i][1] = hull1[i].y;
    }
    
    Q.n = n2;
    for(int i = 0; i < n2; i++) {
      Q.v[i][0] = hull2[i].x;
      Q.v[i][1] = hull2[i].y;
    }

    Tang(&P, &Q);
    int p1 = P.end % P.n,
      q1 = Q.end % Q.n;
      
    Tang(&Q, &P);
    int p2 = P.end % P.n,
      q2 = Q.end % Q.n;

    // Count the number of nodes in the combined hull
    int i = p2, s = 1;
    while(i != p1) {
      i = (i + 1) % P.n;
      s++;
    }

    i = q1;
    s++;
    
    while(i != q2) {
      i = (i + 1) % Q.n;
      s++;
    }

    // Allocate...
    *resn = s;
    *res = new dPoint2[s];

    // Then trace again to actually put nodes in hull
    
    i = p2, s = 1;
    (*res)[0] = hull1[i];
    while(i != p1) {
      i = (i + 1) % P.n;
      (*res)[s] = hull1[i];
      s++;  
    }

    i = q1;
    (*res)[s] = hull2[i];
    s++;
    
    while(i != q2) {
      i = (i + 1) % Q.n;
      (*res)[s] = hull2[i];
      s++;
    }
  }	   
}

// NB!! points* must be sorted beforehand, also on top level (e.g. sorted on x-coordinate)
// The provided less_than-functions are the comparison functions for all levels in order
RangeTree* build_tree(int numLevels,
		      // bool (*less_than[])(const dPoint2& p1, const dPoint2& p2),
		      std::function<bool(const dPoint2& p1, const dPoint2& p2)>* less_than,
		      const dPoint2 *points, int numPoints){
  RangeTree* tn = new RangeTree();
  tn->level = numLevels;
  tn->left = NULL;
  tn->right = NULL;
  tn->next_level = NULL;
  tn->less_than = less_than[0];
  tn->minPoint = points[0];
  tn->maxPoint = points[numPoints - 1];
  
  if(numLevels == 1 && numPoints <= 1) {
    if(numPoints == 1) {
      tn->points = new dPoint2[1];
      tn->points[0] = points[0];
      
      tn->convexHull = new dPoint2[1];
      tn->convexHull[0] = tn->points[0];
    }
    
    tn->numPoints = numPoints;
    tn->hullSize = numPoints;
    
    return tn;
  }

  if(numPoints >= 2) {
    tn->left = build_tree(numLevels, less_than, points, numPoints / 2);
    tn->right = build_tree(numLevels, less_than, points + (numPoints / 2), numPoints - (numPoints / 2));
  }

  if(numLevels == 1) {
    // Here, we know that numPoints >= 2

    tn->points = new dPoint2[numPoints];
    memcpy(tn->points, points, sizeof(dPoint2)* numPoints);
    tn->numPoints = numPoints;

    combine_convex_hulls(tn->left->convexHull, tn->left->hullSize,
			 tn->right->convexHull, tn->right->hullSize,
			 &(tn->convexHull), &(tn->hullSize));

    
  } else {
    // Here, numLevels > 1, numPoints can be whatever
    dPoint2* tempPoints = new dPoint2[numPoints];
    memcpy(tempPoints, points, sizeof(dPoint2) * numPoints);
    std::sort(tempPoints, tempPoints + numPoints, less_than[1]);
    tn->next_level = build_tree(numLevels - 1, less_than + 1,
				tempPoints, numPoints);
    delete[] tempPoints;	
  }

  return tn;
}

void destroy_tree(RangeTree* t) {
  if(t->left) {
    destroy_tree(t->left);
  }
  if(t->right) {
    destroy_tree(t->right);
  }
  
  if(t->level > 1) {
    destroy_tree(t->next_level);
  }

  if(t->level == 1) {
    delete[] t->points;
    delete[] t->convexHull;
  }

  delete t;
}

// Inclusive 
void range_tree_find_subtrees_x(RangeTree* t,
				double minx, double maxx,
				std::vector<RangeTree*>& res) {
  if(t->minPoint.x >= minx && t->maxPoint.x <= maxx) {
    res.push_back(t);
    return;
  }
  
  if(t->minPoint.x > maxx || t->maxPoint.x < minx) {
    return;
  }

  range_tree_find_subtrees_x(t->left, minx, maxx, res);
  range_tree_find_subtrees_x(t->right, minx, maxx, res);
}

// Inclusive 
void range_tree_find_subtrees_y(RangeTree* t,
				double miny, double maxy,
				std::vector<RangeTree*>& res) {
  if(t->minPoint.y >= miny && t->maxPoint.y <= maxy) {
    res.push_back(t);
    return;
  }

  if(t->minPoint.y > maxy || t->maxPoint.y < miny) {
    return;
  }
  
  range_tree_find_subtrees_y(t->left, miny, maxy, res);
  range_tree_find_subtrees_y(t->right, miny, maxy, res);
}

// Inclusive for under, exclusive for over
void range_tree_find_subtrees_halfplane(RangeTree* t,
				        const dPoint2& halfplane_comp, bool over,
					std::vector<RangeTree*>& res) {

  
  if(over) {
    if(t->less_than(halfplane_comp, t->minPoint)) {
      res.push_back(t);
      return;
    }

    if(!t->less_than(halfplane_comp, t->maxPoint)) {
      return;
    }

    range_tree_find_subtrees_halfplane(t->left,
				       halfplane_comp, over,
				       res);
    range_tree_find_subtrees_halfplane(t->right,
				       halfplane_comp, over,
				       res);
  } else {
    
    if(!t->less_than(halfplane_comp, t->maxPoint)) {
      res.push_back(t);
      return;
    }

    if(t->less_than(halfplane_comp, t->minPoint)) {
      return;
    }

    range_tree_find_subtrees_halfplane(t->left,
				       halfplane_comp, over,
				       res);
    range_tree_find_subtrees_halfplane(t->right,
				       halfplane_comp, over,
				       res);
  }
}

// Searching on x-coordinate, y-coordinate and halfplane-coordinate
void range_tree_find_subtrees(RangeTree* t,
			      double minx, double maxx,
			      double miny, double maxy,
			      const dPoint2& halfplane_comp, bool halfplane_over,
			      std::vector<RangeTree*>& res) {
  std::vector<RangeTree*> l1_trees;
  std::vector<RangeTree*> l2_trees;
  range_tree_find_subtrees_x(t, minx, maxx, l1_trees);
  
  for(int i = 0; i < l1_trees.size(); i++ ) {
    range_tree_find_subtrees_y(l1_trees[i]->next_level,
			       miny, maxy,
			       l2_trees);
  }

  for(int i = 0; i < l2_trees.size(); i++) {
    range_tree_find_subtrees_halfplane(l2_trees[i]->next_level,
				       halfplane_comp, halfplane_over,
				       res);
  }
}
