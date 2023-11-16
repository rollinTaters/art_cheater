#pragma once
#include <ostream>

class Point
{
  public:
    Point( float, float );
    Point();
    bool operator==( const Point& ) const;
    bool operator!=( const Point& ) const;
    Point operator+( const Point& ) const;
    Point operator-( const Point& ) const;

    float getX() const;
    float getY() const;
    float dist_to( const Point& ) const;
    Point& move( const float, const float );

  private:
    float m_x;
    float m_y;
};

Point interpolate( const float, const Point, const Point );
std::ostream& operator<<( std::ostream& os, const Point& p );
