#pragma once

class Point
{
  public:
    Point( int, int );
    Point();
    bool operator==( Point& );
    int m_x;
    int m_y;

    int dist_to( Point );
};

class Line{
  public:
    int m_start_x;
    int m_start_y;

    int m_end_x;
    int m_end_y;

    Line( int, int, int, int );
};

class Mesh
{
  public:
    Mesh( Point, Point, Point );

    Point m_p1;
    Point m_p2;
    Point m_p3;
};

int line_test();
void test();
