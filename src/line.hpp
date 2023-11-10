#pragma once
#include <iostream>
#include <vector>

template <typename typ>
bool vectorContains( const typ&, const std::vector<typ>& );

class Point
{
  public:
    Point( int, int );
    Point();
    bool operator==( const Point& ) const;

    int getX() const;
    int getY() const;
    int dist_to( const Point& ) const;

  private:
    int m_x;
    int m_y;
};

class Line
{
  public:
    Line( Point, Point );
    Line( int, int, int, int );
    bool operator==( const Line& ) const;

    int getLen() const;
    Point getStart() const;
    Point getEnd() const;

  private:
    Point m_start;
    Point m_end;
};

class Mesh
{
  public:
    Mesh( Point, Point, Point );
    Mesh();

    float getArea() const;
    bool generateInfill( const float );

    Point m_p1;
    Point m_p2;
    Point m_p3;
    std::vector<Line> infill;
};

Point interpolate( const float, const Point, const Point );
Point interpolate( const float, const Line );

int line_test();
void print_gcode();
