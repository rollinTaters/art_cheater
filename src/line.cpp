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

void Line::move( const float x, const float y )
{
    m_start.move(x,y);
    m_end.move(x,y);
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

