/*
 * A small computation performed in relations to the Minimum Perimeter-Sum Bipartition:
 * Given 18001 points distributed with regular spaces around a square, computes how unique angles
 * the lines between each pair of the points have 

 * Lore:
 * Originally, this was written in Haskell, due to its built-in Rational type
 * It ran for an hour, consuming nearly all of my 16 GB combined physical and virtual RAM
 * before I halted it manually. Afraid that an implementation in Python would give similar
 * difficulties, the choice fell on trusty C++
 */

#include <iostream>
#include <set>
#include <algorithm>
#include <vector>

struct Rational{
    long long int a, b;
    
    Rational(long long int a, long long int b) {
        this->a = a; this->b = b;
    }

    Rational(long long int a) {
        this->a = a; this->b = 1;
    }
};

long long int gcd(long long int a, long long int b) {
    if(a == 0) {
        return b;
    } else {
        return gcd (b % a, a);
    }
}   

Rational operator+(const Rational& a, const Rational& b) {
    Rational c(b.b * a.a + a.b * b.a, b.b * a.b);
    long long int gc = gcd(c.a, c.b);
    
    c.a /= gc;
    c.b /= gc;

    return c;
}

Rational operator-(const Rational& a, const Rational& b) {
    return a + Rational(-b.a, b.b);
}

Rational operator/(const Rational& a, const Rational& b) {
    Rational c(a.a * b.b, a.b * b.a);
    return c;
}

bool operator<(const Rational& a, const Rational& b) {
    return a.a * b.b < a.b * b.a;
}



std::pair<Rational, Rational>& normalize(std::pair<Rational, Rational>& a) {
    if(a.second.a == 0) {
        a.first.a = 1;
        a.first.b = 1;
        a.second.a = 0;
        a.second.b = 1;
    } else {
        a = std::make_pair(a.first / a.second, Rational(1, 1));
    }
    return a;
}

long long int num_steps = 18001;

std::vector<std::pair<Rational, Rational> > points;

std::set<std::pair<Rational, Rational> > line_ratios;
int main() {
    Rational step_dist = Rational(4, num_steps);

    int side = 0; 
    Rational step(0);
    for(int i = 0; i < num_steps; i++) {
        if(side == 0) {
            std::pair<Rational, Rational> p(Rational(0), step);
            points.push_back(p);
        } else if(side == 1) {
            std::pair<Rational, Rational> p(step, Rational(1));
            points.push_back(p);
        } else if(side == 2) {
            std::pair<Rational, Rational> p(Rational(1), Rational(1) - step);
            points.push_back(p);
        } else if(side == 3) {
            std::pair<Rational, Rational> p(Rational(1) - step, Rational(0));
            points.push_back(p);
        }

        step = step + step_dist;
        if(Rational(1) < step) {
            step = step - Rational(1);
            side++;
        }
    }

    for(int i = 0; i < num_steps; i++) {
        for(int j = i + 1; j < num_steps; j++) {
            std::pair<Rational, Rational> p(points[i].first - points[j].first , points[i].second - points[j].second);
            line_ratios.insert(normalize(p));
        }
    }
    std::cout << line_ratios.size() << std::endl;

    return 0;
    
}
