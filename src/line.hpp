#pragma once
#include "point.hpp"
#include <ostream>


class Line
{
  public:
    Line( Point, Point );
    Line( int, int, int, int );
    bool operator==( const Line& ) const;

    float getLen() const;
    Point getStart() const;
    Point getEnd() const;
    Line& move( const float, const float );
    bool onOneSide( const Point ) const;
    bool onSameSide( const Point, const Point ) const;

  private:
    Point m_start;
    Point m_end;
};


Point interpolate( const float, const Line );
std::ostream& operator<<( std::ostream& os, const Line& l );

