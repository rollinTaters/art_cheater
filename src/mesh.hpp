#pragma once
#include "point.hpp"
#include "line.hpp"
#include <ostream>
#include <list>
#include <vector>

bool find_nearest_mesh_points(
        const Point focus_point,
        std::list<Point>& i_all_points,
        std::list<Point>& i_focus_points,
        std::list<Point>& i_vertex_points,
        Point* output );

class Mesh
{
  public:
    Mesh( Point, Point, Point );
    Mesh();

    float getArea() const;
    bool generateInfill( const float );
    void move( const float, const float );
    bool contains( const Point ) const;
    std::vector<Point> getPoints() const;

    Point m_p1;
    Point m_p2;
    Point m_p3;
    std::vector<Line> infill;
};

std::ostream& operator<<( std::ostream& os, const Mesh& m );
