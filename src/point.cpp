#include "point.hpp"
#include <cmath> // sqrt

// ======== POINT ========
Point::Point( float x, float y ):
    m_x(x), m_y(y)
{   }
Point::Point():
    m_x(0.0f), m_y(0.0f)
{   }

bool Point::operator==( const Point& other ) const
{
    return ( m_x == other.m_x && m_y == other.m_y );
}

bool Point::operator!=( const Point& other ) const
{
    return !( *this == other );
}

Point Point::operator+( const Point& other ) const
{
    return Point( m_x + other.getX(), m_y + other.getY() );
}

Point Point::operator-( const Point& other ) const
{
    return Point( m_x - other.getX(), m_y - other.getY() );
}

float Point::getX() const
{ return m_x; }

float Point::getY() const
{ return m_y; }

float Point::dist_to( const Point& p_other ) const
{   // returns the distance between this and given point
    return std::sqrt( std::pow(m_x - p_other.m_x, 2) + std::pow(m_y - p_other.m_y, 2) );
}

Point& Point::move( const float x, const float y )
{
    m_x += x;
    m_y += y;
    return *this;
}

// ==== Standalone Functions ====

Point interpolate( const float ratio, const Point p1, const Point p2 )
{
    int delta_x = p2.getX() - p1.getX();
    int delta_y = p2.getY() - p1.getY();

    return Point( p1.getX() + ratio*delta_x,
                  p1.getY() + ratio*delta_y );
}

// ==== Printing stuff ====
std::ostream& operator<<( std::ostream& os, const Point& p )
{   // printing points on streams
    os << "x:" << p.getX() << " y:" << p.getY() << " ";
    return os;
}
