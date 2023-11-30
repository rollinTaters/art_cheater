#include "line.hpp"
#include <iostream>

// ======== LINE ========
Line::Line(int sx, int sy, int ex, int ey ):
    m_start( Point(sx,sy) ),
    m_end( Point(ex,ey) )
{   }

Line::Line( Point sp, Point ep ):
    m_start(sp), m_end(ep)
{   }

Line::Line()
{   }

bool Line::operator==( const Line& other ) const
{
    return ( m_start == other.m_start && m_end == other.m_end );
}

float Line::getLen() const
{ // returns lenght of line
    return m_end.dist_to(m_start);
}

Point Line::getStart() const
{ return m_start; }

Point Line::getEnd() const
{ return m_end; }

Line& Line::move( const float x, const float y )
{
    m_start.move(x,y);
    m_end.move(x,y);
    return *this;
}

Line& Line::move( const Point& p )
{
    return move( p.getX(), p.getY() );
}

Line& Line::rotate( const float ang )
{
    m_start.rotate(ang);
    m_end.rotate(ang);
    return *this;
}

bool Line::onOneSide( const Point p ) const
{
    // dont ask..
    float x2 = m_end.getX();
    float x1 = m_start.getX();
    float y2 = m_end.getY();
    float y1 = m_start.getY();
    float x = p.getX();
    float y = p.getY();
    return ((x2-x1) * (y1-y) - (x1-x) * (y2-y1)) < 0;
}

bool Line::onSameSide( const Point p1, const Point p2 ) const
{
    return !(onOneSide(p1) != onOneSide(p2));
}



// ==== Printing stuff ====
std::ostream& operator<<( std::ostream& os, const Line& l )
{   // printing lines on streams
    os << "start p: " << l.getStart() << " end p: " << l.getEnd()
       << "\nlen: " << l.getLen() << "\n";
    return os;
}

Point interpolate( const float ratio, const Line l )
{
    return interpolate( ratio, l.getStart(), l.getEnd() );
}

