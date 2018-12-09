#include <common.hpp>
#include <tangent.h>

#include <set>
#include <vector>
#include <algorithm>
#include <cstring>

const std::vector<dPoint2*> *g_hulls;
const std::vector<int> *g_sizes;

std::vector<dPoint2>* g_lefts;
std::vector<dPoint2>* g_rights;

void crop_polygon(dPoint2* hull, int size,
		  double startx, double endx,
		  std::vector<dPoint2>& polygon);

int signum(double a) {
  return (a > 0) - (a < 0);
}

struct CompClass {
  // Polygon p1 is above p2 at their overlap
  bool operator()(int p1, int p2) {
    int a;

    if(p1 == p2) {
      return false;
    }
    bool u;
    if((*g_sizes)[p1] == 1) {
      if((*g_sizes)[p2] == 1) {
	u =  (*g_hulls)[p1][0].y > (*g_hulls)[p2][0].y;
      } else {
	u =  cross((*g_lefts)[p2] - (*g_rights)[p2],
		   (*g_lefts)[p1] - (*g_rights)[p2]) < 0;
      }
    } else if((*g_sizes)[p2] == 1) {
      u = cross((*g_lefts)[p1] - (*g_rights)[p1],
		(*g_lefts)[p2] - (*g_rights)[p1]) > 0;
    } else if( (a = signum(cross((*g_rights)[p1] - (*g_lefts)[p1],
			  (*g_rights)[p2] - (*g_lefts)[p1]))) ==
	signum(cross((*g_rights)[p1] - (*g_lefts)[p1],
		     (*g_lefts)[p2] - (*g_lefts)[p1]))) {
      u = !(((a) + 1) / 2);
    } else { // We assume no crossings, so we can check other case
      // only with one endpoint
      u = !(((signum(cross((*g_lefts)[p2] - (*g_rights)[p2],
			    (*g_rights)[p1] - (*g_rights)[p2]))) + 1) / 2);
    }

    return u;
  }
};

typedef CompClass CompClassUpper;

struct CompClassLower {
// Polygon p1 is below p2 at their overlap
  bool operator()(int p1, int p2) {
    
    int a;

    if(p1 == p2) {
      return false;
    }
    bool u;
    if((*g_sizes)[p1] == 1) {
      if((*g_sizes)[p2] == 1) {
	u =  (*g_hulls)[p1][0].y < (*g_hulls)[p2][0].y;
      } else {
	u =  cross((*g_lefts)[p2] - (*g_rights)[p2],
		   (*g_lefts)[p1] - (*g_rights)[p2]) > 0;
      }
    } else if((*g_sizes)[p2] == 1) {
      u = cross((*g_lefts)[p1] - (*g_rights)[p1],
		(*g_lefts)[p2] - (*g_rights)[p1]) < 0;
    } else if( (a = signum(cross((*g_rights)[p1] - (*g_lefts)[p1],
			  (*g_rights)[p2] - (*g_lefts)[p1]))) ==
	signum(cross((*g_rights)[p1] - (*g_lefts)[p1],
		     (*g_lefts)[p2] - (*g_lefts)[p1]))) {
      u = !(((-a) + 1) / 2);
    } else { // We assume no crossings, so we can check other case
      // only with one endpoint
      u = !(((-signum(cross((*g_lefts)[p2] - (*g_rights)[p2],
			    (*g_rights)[p1] - (*g_rights)[p2]))) + 1) / 2);
    }
    
    return u;
  }
};


int hull_minx_index(std::vector<dPoint2>& hull) {
  int s;
  double mx = 1e100;
  for(int j = 0; j < hull.size(); j++) {
    if(mx > hull[j].x) {
      s = j;
      mx = hull[j].x;
    }
  }
  return s;
}

int hull_maxx_index(std::vector<dPoint2>& hull) {
  double mx = -1e100;
  int e;
  for(int j = 0; j < hull.size(); j++) {
    if(mx < hull[j].x) {
      e = j;
      mx = hull[j].x;
    }
  }
  return e;
}

void construct_new_polygons_upper(const std::vector<dPoint2*>& hulls,
				  const std::vector<int>& sizes,
				  const std::vector<std::pair<double, int> >& events,
				  std::vector< std::vector<dPoint2> >& new_polygons) {
  bool lower_hull = 0;
  double last_switch_x;

  std::set<int, CompClassUpper> currActive;
  
  int currTop = -1;
  for(int i = 0; i < events.size(); i++) {
    std::pair<double, int> p = events[i];
    
    int changed = 0;
    if((p.second < 0) ^ lower_hull) { // If lower hull, deletions are insertions
      // Insertion
      int ind;
      if(lower_hull) {
	ind = p.second - 1;
      } else {
	ind = -p.second - 1;
      }
      currActive.insert(ind);
      
      // Find and check if curr top changed
      if((*(currActive.begin())) != currTop) {
	changed = 1;
	if(currTop >= 0) {
	  
	  new_polygons.push_back(std::vector<dPoint2>());
	  if(lower_hull) {
	    crop_polygon(hulls[currTop], sizes[currTop],
			 p.first, last_switch_x,
			 new_polygons[new_polygons.size() - 1]);
	  } else {
	    crop_polygon(hulls[currTop], sizes[currTop],
			 last_switch_x, p.first,
			 new_polygons[new_polygons.size() - 1]);
	  }
	}
      }
    } else {
      // Deletion
      
      int ind;
      if(lower_hull) {
	ind = -p.second - 1;
      } else {
	ind = p.second - 1;
      }
      currActive.erase(currActive.find(ind));

      if( currActive.begin() == currActive.end() ||
	  *(currActive.begin()) != currTop) {
	changed = 1;
	new_polygons.push_back(std::vector<dPoint2>());
	if(lower_hull) {
	  crop_polygon(hulls[currTop], sizes[currTop],
		       p.first, last_switch_x,
		       new_polygons[new_polygons.size() - 1]);
	} else {
	  crop_polygon(hulls[currTop], sizes[currTop],
		       last_switch_x, p.first,
		       new_polygons[new_polygons.size() - 1]);
	}
	
      }
    }
    
    if(changed && currActive.begin() != currActive.end()) {
      currTop = *(currActive.begin());
      last_switch_x = p.first;
    } else if (changed) {
      currTop = -1;
    }
    
  }
}

void construct_new_polygons_lower(const std::vector<dPoint2*>& hulls,
				  const std::vector<int>& sizes,
				  const std::vector<std::pair<double, int> >& events,
				  std::vector< std::vector<dPoint2> >& new_polygons) {
  bool lower_hull = 1;
  double last_switch_x;

  std::set<int, CompClassLower> currActive;
  
  int currTop = -1;
  for(int i = 0; i < events.size(); i++) {
    std::pair<double, int> p = events[i];
    int changed = 0;
    
    if((p.second < 0) ^ lower_hull) { // If lower hull, deletions are insertions
      // Insertion
      int ind;
      if(lower_hull) {
	ind = p.second - 1;
      } else {
	ind = -p.second - 1;
      }
      currActive.insert(ind);
      
      
      // Find and check if curr top changed
      if((*(currActive.begin())) != currTop) {
	changed = 1;
	if(currTop >= 0) {
	  
	  new_polygons.push_back(std::vector<dPoint2>());
	  if(lower_hull) {
	    crop_polygon(hulls[currTop], sizes[currTop],
			 p.first, last_switch_x,
			 new_polygons[new_polygons.size() - 1]);
	  } else {
	    crop_polygon(hulls[currTop], sizes[currTop],
			 last_switch_x, p.first,
			 new_polygons[new_polygons.size() - 1]);
	  }
	}
      }
    } else {
      // Deletion
      
      int ind;
      if(lower_hull) {
	ind = -p.second - 1;
      } else {
	ind = p.second - 1;
      }
      currActive.erase(currActive.find(ind));

      if( currActive.begin() == currActive.end() ||
	  *(currActive.begin()) != currTop) {
	changed = 1;
	new_polygons.push_back(std::vector<dPoint2>());
	if(lower_hull) {
	  crop_polygon(hulls[currTop], sizes[currTop],
		       p.first, last_switch_x,
		       new_polygons[new_polygons.size() - 1]);
	} else {
	  crop_polygon(hulls[currTop], sizes[currTop],
		       last_switch_x, p.first,
		       new_polygons[new_polygons.size() - 1]);
	}
      }
    }
    
    if(changed && currActive.begin() != currActive.end()) {
      currTop = *(currActive.begin());
      last_switch_x = p.first;
    } else if (changed) {
      currTop = -1;
    }
    
  }
}

// Construct upper / lower hull
void construct_half_hull(const std::vector<dPoint2*>& hulls,
			 const std::vector<double*>& lengths,
			 const std::vector<int> sizes,
			 std::vector<std::pair<double, int> >& events,
			 std::vector<dPoint2>& res,
			 std::vector<double>& resLength,
			 bool lower_hull) {
  
  // Remember to sort events correctly
  if(!lower_hull) {
    std::sort(events.begin(), events.end());
  } else {
    std::sort(events.rbegin(), events.rend());
  }
  
  

  // God have mercy
  std::vector< std::vector<dPoint2> > new_polygons;

  if(lower_hull) {
    construct_new_polygons_lower(hulls,
				 sizes,
				 events,
				 new_polygons);
  } else {
    construct_new_polygons_upper(hulls,
				 sizes,
				 events,
				 new_polygons);
  }

  // Go through all new polygons, pick the ones conserving convexity

  std::vector< int > chosen_polygons;
  std::vector< std::pair< int, int> > bridge_indices;
  
  // Now, go through every new polygon and check for convexity of bridges going from
  // point to point. Basically convex hull using polygons instead of points
  
  // New polygons should already be sorted
  for(int i = 0; i < new_polygons.size(); i++) {
    Polygon P, Q, R;
    if( chosen_polygons.size() < 2) {
      chosen_polygons.push_back(i);
      
      if(chosen_polygons.size() == 2) {
	P.n = new_polygons[chosen_polygons[0]].size();
	memcpy(P.v, new_polygons[chosen_polygons[0]].data(),
	       new_polygons[chosen_polygons[0]].size() * sizeof(dPoint2));
	
	Q.n = new_polygons[chosen_polygons[1]].size();
	memcpy(Q.v, new_polygons[chosen_polygons[1]].data(),
	       new_polygons[chosen_polygons[1]].size() * sizeof(dPoint2));
	Tang(&Q, &P);
	
	bridge_indices.push_back(std::make_pair(P.end % P.n, Q.end % Q.n));

      }
      continue;
    }
    
    // Note to self: Use std::copy_n() next time..
    int l = chosen_polygons.size();
    P.n = new_polygons[chosen_polygons[l - 2]].size();
    memcpy(P.v, new_polygons[chosen_polygons[l - 2]].data(),
	   new_polygons[chosen_polygons[l - 2]].size() * sizeof(dPoint2));
    Q.n = new_polygons[chosen_polygons[l - 1]].size();
    memcpy(Q.v, new_polygons[chosen_polygons[l - 1]].data(),
	   new_polygons[chosen_polygons[l - 1]].size() * sizeof(dPoint2));
    R.n = new_polygons[i].size();
    memcpy(R.v, new_polygons[i].data(), new_polygons[i].size() * sizeof(dPoint2));

    Tang(&Q, &P);
    int p1 = P.end % P.n, q1 = Q.end % Q.n;
    Tang(&R, &Q);
    int q2 = Q.end % Q.n, r1 = R.end % R.n;

    if(cross(((dPoint2*)Q.v)[q1] - ((dPoint2*)P.v)[p1],
	     ((dPoint2*)R.v)[r1] - ((dPoint2*)Q.v)[q2]) >= 0) {
      chosen_polygons.pop_back();
      bridge_indices.pop_back();
      i--;
    } else {
      chosen_polygons.push_back(i);
      bridge_indices.push_back(std::make_pair(q2, r1));
    }

  }
  
  // Then go through chosen polygons, construct hull

  // Now, go through the polygons found to form a convex hull and actually trace out the hull itself
  int accLength = 0;
  for(int i = 0; i < chosen_polygons.size(); i++) {
    int ind = chosen_polygons[i];
    int s, e;
    // Determine starting point on current polygon (extreme x for first)
    if( i == 0 ) {
      if(lower_hull) {
	s = hull_maxx_index(new_polygons[ind]);
      } else {
	s = hull_minx_index(new_polygons[ind]);
      }
    } else {
      s = bridge_indices[i - 1].second;
    }

    // Determine ending point on current polygon
    if(i == chosen_polygons.size() - 1) {
      if(lower_hull) {
	e = hull_minx_index(new_polygons[ind]);
      } else {
	e = hull_maxx_index(new_polygons[ind]);
      }
    } else {
      e = bridge_indices[i].first;
    }

    // Trace boundary between points
    while(s != e) {
      // Tracing backwards (I just realized we have to reverse the entire thing in the end)
      // Wow. GG.
      int next = (s + new_polygons[ind].size() - 1) % new_polygons[ind].size();
      res.push_back(new_polygons[ind][s]);
      resLength.push_back(accLength);

      accLength += length(new_polygons[ind][s] - new_polygons[ind][next]);
      
      s = next;
    }

    res.push_back(new_polygons[ind][s]);
    resLength.push_back(accLength);
    
    if(i != chosen_polygons.size() - 1) {
      accLength += length(new_polygons[ind][e] -
			  new_polygons[chosen_polygons[i + 1]][bridge_indices[i].second]);
    }
  }
}

void combine_multiple_convex_hulls(const std::vector<dPoint2*>& hulls,
				   const std::vector<double*>& lengths,
				   const std::vector<int>& sizes,
				   std::vector<dPoint2>& res, std::vector<double>& resLength) {
  // If there is only one hull in the set, return it
  if(hulls.size() == 1) {
    for(int i = 0; i < sizes[0]; i++) {
      res.push_back(hulls[0][i]);
      resLength.push_back(lengths[0][i]); 
    }
    return;
  }
  
  g_hulls = &hulls;
  g_sizes = &sizes;

  std::vector<dPoint2> lefts;
  std::vector<dPoint2> rights;

  g_lefts = &lefts;
  g_rights = &rights;
  
  // Start events stored as positive indices, ends as negative (1-indexed to avoid problems with 0)
  std::vector<std::pair<double, int> > events;

  // Create a pair of events for each polygon, also store the leftmost and rightmost
  // points for each
  
  // NB: This loop has complexity m - the number of vertices. But this should only take klogm..
  // , k being the number of polygons
  for(int i = 0; i < hulls.size(); i++) {
    double maxx = -1e100, minx = 1e100;
    int maxi, mini;
    for(int j = 0; j < sizes[i]; j++) {
      if(maxx < hulls[i][j].x) {
	maxi = j;
	maxx = hulls[i][j].x;
      }
      if(minx > hulls[i][j].x) {
	mini = j;
	minx = hulls[i][j].x;
      }
    }
    events.push_back(std::make_pair(minx, -(i + 1)));
    events.push_back(std::make_pair(maxx, (i + 1)));

    lefts.push_back(hulls[i][mini]);
    rights.push_back(hulls[i][maxi]);
  }

  construct_half_hull(hulls, lengths,
		      sizes,
		      events,
		      res, resLength,
		      false);
  
  res.pop_back();
  resLength.pop_back();
  
  construct_half_hull(hulls, lengths,
		      sizes,
		      events,
		      res, resLength,
		      true);

  res.pop_back();
  resLength.pop_back();

  // res.reverse();
  // resLength.reverse(); 
}

// Violently violates the expected running time... Oh well
void crop_polygon(dPoint2* hull, int size,
		  double startx, double endx,
		  std::vector<dPoint2>& polygon) {
  for(int i = 0; i < size; i++) {
    int next = (i + 1) % size;
    // Insert intermediate points if the edge this -> next crosses bounds
    if(hull[i].x < startx && hull[next].x > startx) {
      
      double frac = (startx - hull[i].x) / (hull[next].x - hull[i].x);
      polygon.push_back(hull[i] * (1 - frac) + hull[next] * frac);
      
    } else if (hull[i].x > endx && hull[next].x < endx) {
      
      double frac = (endx - hull[next].x) / (hull[i].x - hull[next].x);
      polygon.push_back(hull[i] * frac + hull[next] * (1 - frac));			
    }

    // Insert point itself if within bounds
    if(hull[i].x >= startx && hull[i].x <= endx) {
      polygon.push_back(hull[i]);
    }

    // Crosses bounds (again) (no reason to put this down here, but whatever
    if ( hull[i].x < endx && hull[next].x > endx) {
      
      double frac = (endx - hull[i].x) / (hull[next].x - hull[i].x);
      polygon.push_back(hull[i] * (1 - frac) + hull[next] * frac);


    } else if ( hull[i].x > startx && hull[next].x < startx) {
      double frac = (startx - hull[next].x) / (hull[i].x - hull[next].x);
      polygon.push_back(hull[next] * (1 - frac) + hull[i] * frac);
    }
  }
}
