#pragma once
#include "SFML/Graphics/Image.hpp"
#include <iostream>
#include <vector>

template <typename typ>
bool vectorContains( const typ&, const std::vector<typ>& );

class Point
{
  public:
    Point( float, float );
    Point();
    bool operator==( const Point& ) const;

    float getX() const;
    float getY() const;
    float dist_to( const Point& ) const;

  private:
    float m_x;
    float m_y;
};

class Line
{
  public:
    Line( Point, Point );
    Line( int, int, int, int );
    bool operator==( const Line& ) const;

    float getLen() const;
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

void create_line_artwork( float, float );
void print_gcode();
void draw_to_image( sf::Image&, float );
