#include "line.hpp"
#include <cstdlib>  // for rand()
#include <cmath>    // for sqrt()
#include <list>

/*
   -pick random points
   -mesh points (triangles)
   -pick one point of mesh to be the "focus"
   -draw lines from focus to edges of mesh
   -store those lines in a vector
   -profit

   */

// ======== Global scoped Storage ========
std::vector<Mesh> v_meshes;

// ======== POINT ========
Point::Point( float x, float y ):
    m_x(x), m_y(y)
{   }
Point::Point():
    m_x(rand()), m_y(rand())
{   }

bool Point::operator==( const Point& other ) const
{
    return ( m_x == other.m_x && m_y == other.m_y );
}

float Point::getX() const
{ return m_x; }

float Point::getY() const
{ return m_y; }

float Point::dist_to( const Point& p_other ) const
{   // returns the distance between this and given point
    return std::sqrt( std::pow(m_x - p_other.m_x, 2) + std::pow(m_y - p_other.m_y, 2) );
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

float Line::getLen() const
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
{ // target_density is lines per 10mm
    // dont regenerate the infill (at least for now)
    if( !infill.empty() ) return false;

    // FIXME: currently we are always picking m_p1 as the focus
    Point focus = m_p1;
    Line edge = Line( m_p2, m_p3 );

    // DEBUG
    //std::cout<<"\narea: "<<area<<" ratio: "<<area/density<<"\n";

    float tick = (1 - target_density)*100.f /edge.getLen();
    //std::cout<< "tick: "<<tick<<"\n";

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
        std::list<Point>& i_all_points,
        std::list<Point>& i_focus_points,
        std::list<Point>& i_vertex_points,
        Point* output )
{
    output[0] = Point(0,0);
    output[1] = Point(0,0);
    output[2] = Point(0,0);

    // pick 3 points for mesh corners
    for( int mesh_corner_ind = 0; mesh_corner_ind < 3; mesh_corner_ind++ )
    {
        int distance = RAND_MAX;    // #TODO change the value later when adjusting sizes
        // for all points in i_all_points, find nearest
        for( Point all_p : i_all_points )
        {
            // dont compare it with itself
            if( focus_point == all_p) continue;

            // also dont compare with previous picked points
            if( all_p == output[0] ||
                all_p == output[1] ||
                all_p == output[2] ) continue;
            
            // check the distance
            if( focus_point.dist_to( all_p ) < distance )
            {
                distance = focus_point.dist_to( all_p );
                output[mesh_corner_ind] = all_p;
            }
        }
        // do the same shit for vertex points
        for( Point vertex_p : i_vertex_points )
        {
            // dont compare with previous picked points
            if( vertex_p == output[0] ||
                vertex_p == output[1] ||
                vertex_p == output[2] ) continue;
            
            // check the distance
            if( focus_point.dist_to( vertex_p ) < distance )
            {
                distance = focus_point.dist_to( vertex_p );
                output[mesh_corner_ind] = vertex_p;
            }
        }

        if( distance == RAND_MAX )
        {   // if we couldnt find a nearest point, (no more points left)
            // clear the "output"
            output[0] = Point(0,0);
            output[1] = Point(0,0);
            output[2] = Point(0,0);
            return false;
        }else{
            // move the picked point to the vertex points
            i_all_points.remove( output[mesh_corner_ind] );
            i_vertex_points.push_back( output[mesh_corner_ind] );
            // TODO: we may add duplicates of points, wasted cpu cycles here...
        }
    }
    // move the focus point from all points to focus points list
    i_all_points.remove( focus_point );
    i_focus_points.push_back( focus_point );
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

void create_line_artwork( float wa_x, float wa_y )
{
    std::cout<< "line_test function called\n";
    float work_area_x = wa_x;
    float work_area_y = wa_y;

    // containers for points
    std::list<Point> all_points;
    std::list<Point> focus_points;
    std::list<Point> vertex_points;
    Point poynts[3];

    // create some random points
    for( int i=0; i<400; i++ )
    {
        all_points.emplace_back( std::rand()%(int)work_area_x, std::rand()%(int)work_area_y );
    }

    // start creating meshes
    while( !all_points.empty() )
    { // focus point index
        // find nearest 3 points
        if( !find_nearest_mesh_points( all_points.front(), all_points, focus_points, vertex_points, poynts ) ) break;

        // create meshes from those points
        v_meshes.emplace_back( poynts[0], poynts[1], poynts[2] );
    }
    std::cout<< "Created "<< v_meshes.size() << " meshes\n";

    // generate infill for meshes
    for( size_t mi = 0; mi < v_meshes.size(); mi++ )
    { // mesh index
        v_meshes[mi].generateInfill( 0.89f );

        // DEBUG: print infill count
        //std::cout<< "Mesh " << mi << " generated " << v_meshes[mi].infill.size() << " infill lines\n";
        // DEBUG: print mesh info
        //std::cout<< v_meshes[mi] <<"\n";
    }

}

void print_gcode()
{   // prints gcode to stream

    float z_clearance_level = 3.f;

    std::cout<<"G21 ;metric values\n"
             <<"G90 ;absolute positioning\n"
             <<"G28 X0 Y0 ;move/set to xy zero\n"
             <<"G28 Z0 ; move/set to Z zero\n"
             <<"G01 Z"<<z_clearance_level<<" F400 ;move clearance amount up\n";

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
                     <<"G00 Z"<<z_clearance_level<<"\n";
        }
    }
}

void dLine( sf::Image& im, const Line& l, const float thickness, float ppmm )
{   // draws the line on the image using CPU
    Point p1 = l.getStart();
    Point p2 = l.getEnd();
    int im_h = im.getSize().x;
    int im_w = im.getSize().y;
    float x1 = p1.getX()*ppmm;
    float x2 = p2.getX()*ppmm;
    float y1 = p1.getY()*ppmm;
    float y2 = p2.getY()*ppmm;
    float l_len = l.getLen()*ppmm;
    int border = 0.4*ppmm;

    for( int x = std::min(x1, x2)-border; x < std::max(x1, x2)+border; x++ )
    {   // line x span + border
        for( int y = std::min(y1, y2)-border; y < std::max(y1, y2)+border; y++ )
        {   // line y span + border
            if( x < 0 || y < 0 || x > im_w || y > im_h ) continue;
            if( std::abs( (x2-x1) * (y1-y) - (x1-x) * (y2-y1) ) / l_len < (thickness*ppmm/2.f) )
                im.setPixel(x, y, sf::Color::Black );
        }
    }
}

void draw_to_image( sf::Image& im, float ppmm )
{
    for( Mesh m : v_meshes )
    {
        // mesh bounds
        dLine( im, Line(m.m_p1, m.m_p2), 0.7f, ppmm );
        dLine( im, Line(m.m_p2, m.m_p3), 0.7f, ppmm );
        dLine( im, Line(m.m_p3, m.m_p1), 0.7f, ppmm );

        /*
        // infill lines
        for( Line l: m.infill )
        {
            dLine( im, l, 0.5f, ppmm );
        }
        */
    }
}


