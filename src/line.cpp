#include "line.hpp"
#include "SFML/Graphics.hpp"
#include <cstdlib>  // for rand()
#include <cmath>    // for sqrt()

/*
   -pick random points
   -mesh points (triangles)
   -pick one point of mesh to be the "focus"
   -draw lines from focus to edges of mesh
   -store those lines in a vector
   -profit

   */

// ======== Global scoped Storage ========
std::vector<Point> v_points;
std::vector<Line> v_lines;
std::vector<Mesh> v_meshes;

// ======== POINT ========
Point::Point( int x, int y ):
    m_x(x), m_y(y)
{   }
Point::Point():
    m_x(rand()), m_y(rand())
{   }

bool Point::operator==( const Point& other ) const
{
    return ( m_x == other.m_x && m_y == other.m_y );
}

int Point::getX() const
{ return m_x; }

int Point::getY() const
{ return m_y; }

int Point::dist_to( const Point& p_other ) const
{   // returns the distance between this and given point
    return (int)std::sqrt( std::pow(m_x - p_other.m_x, 2) + std::pow(m_y - p_other.m_y, 2) );
}


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

int Line::getLen() const
{ // returns lenght of line
    return m_end.dist_to(m_start);
}

Point Line::getStart() const
{ return m_start; }

Point Line::getEnd() const
{ return m_end; }


// ======== MESH ========
Mesh::Mesh( Point p1, Point p2, Point p3 ):
    m_p1(p1), m_p2(p2), m_p3(p3)
{   }

Mesh::Mesh()
{
    // assign random points
    m_p1 = Point();
    m_p2 = Point();
    m_p3 = Point();
}

float Mesh::getArea() const
{
    // using heron's formula
    float len1 = m_p1.dist_to(m_p2);
    float len2 = m_p2.dist_to(m_p3);
    float len3 = m_p3.dist_to(m_p1);
    float s = ( len1 + len2 + len3 )/2.0f;

    // DEBUG
    //std::cout<< "area call; l1:"<< len1 << " l2:" << len2 << " l3:" << len3 << " s: " << s << "\n"
             //<< "returned: " <<(int)std::sqrt( s*(s-len1)*(s-len2)*(s-len3) ) << "\n";

    return std::sqrt( s*(s-len1)*(s-len2)*(s-len3) );
}

bool Mesh::generateInfill( const float target_density )
{
    // dont regenerate the infill (at least for now)
    if( !infill.empty() ) return false;

    // FIXME: currently we are always picking m_p1 as the focus
    Point focus = m_p1;
    Line edge = Line( m_p2, m_p3 );

    /*
    float area = getArea() / 1.2e6; // scaled by a made up number
    if( area == 0 ) return true; // if for some cruel chance we get 3 points in a line (it happened a couple of times)
    float mid_point_dist = focus.dist_to( interpolate( 0.5f, edge ) );
    float area_per_dist = area/mid_point_dist;  // average spread angle of boundary lines

    float tick = area_per_dist / target_density * 100.f;
    std::cout<< "tick: "<<tick<<"\n";
    */

    // DEBUG
    //std::cout<<"\narea: "<<area<<" ratio: "<<area/density<<"\n";

    float tick = 1 - target_density;

    for( float ratio = tick; ratio < 1.0f; ratio += tick )
    {
        infill.emplace_back( focus, interpolate(ratio, edge) );
    }
    return true;
}


// ==== Printing stuff ====
std::ostream& operator<<( std::ostream& os, const Point& p )
{   // printing points on streams
    os << "x:" << p.getX() << " y:" << p.getY() << " ";
    return os;
}
std::ostream& operator<<( std::ostream& os, const Line& l )
{   // printing lines on streams
    os << "start p: " << l.getStart() << " end p: " << l.getEnd()
       << "\nlen: " << l.getLen() << "\n";
    return os;
}
std::ostream& operator<<( std::ostream& os, const Mesh& m )
{   // printing meshes on streams
    os  << "Mesh:"
        << "\np1: " << m.m_p1 << "\tlen 12: "<< m.m_p1.dist_to(m.m_p2)
        << "\np2: " << m.m_p2 << "\tlen 23: "<< m.m_p2.dist_to(m.m_p3)
        << "\np3: " << m.m_p3 << "\tlen 31: "<< m.m_p3.dist_to(m.m_p1)
        << "\n"
        << "area: " << m.getArea() << "\tinfill count: " <<m.infill.size()<<"\n";
    return os;
}


bool find_nearest_mesh_points(
        const Point focus_point,
        const std::vector<Point>& i_points,
        std::vector<Point>& i_excluded_points,
        Point* output )
{

    // pick 3 points for mesh corners
    for( int mesh_corner_ind = 0; mesh_corner_ind < 3; mesh_corner_ind++ )
    {
        int distance = RAND_MAX;    // #TODO change the value later when adjusting sizes
        // for all points in i_points, find nearest
        for( size_t i = 0; i < i_points.size(); i++ )
        {
            // dont compare it with itself
            if( focus_point == i_points[i] ) continue;

            // dont compare with excluded points
            if( vectorContains( i_points[i], i_excluded_points ) ) continue;
            
            // check the distance
            if( focus_point.dist_to( i_points[i] ) < distance )
            {
                distance = focus_point.dist_to( i_points[i] );
                output[mesh_corner_ind] = i_points[i];
            }
        }

        if( distance == RAND_MAX )
        {   // if we couldnt find a nearest point, (no more points left)
            // clear the "output"
            output[0] = Point(0,0);
            output[1] = Point(0,0);
            output[2] = Point(0,0);
            return false;
        }
        else{
            // add the picked point to the excluded points
            i_excluded_points.push_back( output[mesh_corner_ind] );
        }
    }
    i_excluded_points.push_back( focus_point );
    return true;
}

template <typename typ>
bool vectorContains( const typ& x, const std::vector<typ>& i_vector )
{
    for( size_t i = 0; i < i_vector.size(); i++ )
    {
        if( i_vector[i] == x ) return true;
    }
    return false;
}

Point interpolate( const float ratio, const Point p1, const Point p2 )
{
    int delta_x = p2.getX() - p1.getX();
    int delta_y = p2.getY() - p1.getY();

    return Point( p1.getX() + ratio*delta_x,
                  p1.getY() + ratio*delta_y );
}
Point interpolate( const float ratio, const Line l )
{
    return interpolate( ratio, l.getStart(), l.getEnd() );
}

int line_test()
{
    //int lel = std::rand();

    std::cout<< "line_test function called\n";

    // create 170 random points
    for( int i=0; i<170; i++ )
    {
        v_points.emplace_back( std::rand()%5000, std::rand()%5000 );
    }


    // a helper vector for keeping track of center("focus") points
    std::vector<Point> excluded_points;
    Point poynts[3];

    // start creating meshes
    for( size_t fpi = 0; fpi < v_points.size(); fpi++ )
    { // focus point index

        // find nearest 3 points
        if( !find_nearest_mesh_points( v_points[fpi], v_points, excluded_points, poynts ) ) break;

        // create meshes from those points
        v_meshes.emplace_back( poynts[0], poynts[1], poynts[2] );
    }
    std::cout<< "Created "<< v_meshes.size() << " meshes\n";

    // generate infill for meshes
    for( size_t mi = 0; mi < v_meshes.size(); mi++ )
    { // mesh index
        v_meshes[mi].generateInfill( 0.89f );
        std::cout<< "Mesh " << mi << " generated " << v_meshes[mi].infill.size() << " infill lines\n";
        std::cout<< v_meshes[mi] <<"\n";
    }

    //print_gcode();

    return 0; // default successfull exit code
}

void print_gcode()
{   // prints gcode to stream

    std::cout<<"G21 ;metric values\n"
             <<"G90 ;absolute positioning\n"
             <<"G28 X0 Y0 ;movei/set to xy zero\n"
             <<"G28 Z0 ; move/set to Z zero\n"
             <<"G01 Z3 F400 ;move 3mm up\n";

    for( size_t m_ind = 0; m_ind < v_meshes.size(); m_ind++ )
    { // for each mesh, m_ind
        std::cout<<";== mesh "<<m_ind<<" ==\n";
        for( size_t i_ind = 0; i_ind < v_meshes[m_ind].infill.size(); i_ind++ )
        {   // for each infill line in mesh
            Line l = v_meshes[m_ind].infill[i_ind];
            std::cout<<";mesh_"<<m_ind<<" infill_"<<i_ind<<"\n"
                     <<"G01 X"<<l.getEnd().getX()<<" Y"<<l.getEnd().getY()<<"\n"
                     <<"G01 Z0\n"
                     <<"G01 X"<<l.getStart().getX()<<" Y"<<l.getStart().getY()<<"\n"
                     <<"G00 Z3\n";
        }
    }
}




