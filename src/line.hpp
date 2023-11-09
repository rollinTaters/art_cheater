#pragma once
#include <iostream>
#include <vector>

template <typename typ>
bool vectorContains( typ& , std::vector<typ>& );

class Point
{
  public:
    Point( int, int );
    Point();
    bool operator==( const Point& );

    int dist_to( const Point& );

    int m_x;
    int m_y;
};

class Line
{
  public:
    Line( Point, Point );
    Line( int, int, int, int );

    Point m_start;
    Point m_end;
};

class Mesh
{
  public:
    Mesh( Point, Point, Point );
    Mesh();

    Point m_p1;
    Point m_p2;
    Point m_p3;
};

int line_test();
