#include "mesh.hpp"
#include <cmath>    // sqrt





// ==== Printing stuff ====
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

Mesh& Mesh::move( const float x, const float y )
{
    m_p1 = m_p1 + Point(x,y);
    m_p2 = m_p2 + Point(x,y);
    m_p3 = m_p3 + Point(x,y);
    for( Line& l : infill )
    {
        l.move(x,y);
    }
    return *this;
}

bool Mesh::contains( const Point p ) const
{
    return ( Line(m_p1,m_p2).onSameSide(m_p3, p) &&
             Line(m_p2,m_p3).onSameSide(m_p1, p) &&
             Line(m_p3,m_p1).onSameSide(m_p2, p) );
}

std::vector<Point> Mesh::getPoints() const
{
    std::vector<Point> tempVec;
    tempVec.push_back( m_p1 );
    tempVec.push_back( m_p2 );
    tempVec.push_back( m_p3 );
    return tempVec;
}

