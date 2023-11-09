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

std::vector<Point> v_points;
std::vector<Line> v_lines;
std::vector<Mesh> v_meshes;

Point::Point( int x, int y ):
    m_x(x), m_y(y)
{   }
Point::Point():
    m_x(rand()), m_y(rand())
{   }

bool Point::operator==( const Point& other )
{
    if( m_x == other.m_x && m_y == other.m_y )
        return true;
    return false;
}

int Point::dist_to( const Point& p_other )
{   // returns the distance between this and given point
    return std::sqrt( std::pow(m_x - p_other.m_x, 2) + std::pow(m_y - p_other.m_y, 2) );
}


Line::Line(int sx, int sy, int ex, int ey ):
    m_start( Point(sx,sy) ),
    m_end( Point(ex,ey) )
{   }

Line::Line( Point sp, Point ep ):
    m_start(sp), m_end(ep)
{   }

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

bool find_nearest_mesh_points(
        Point focus_point,
        std::vector<Point>& i_points,
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
bool vectorContains( typ& x, std::vector<typ>& i_vector )
{
    for( size_t i = 0; i < i_vector.size(); i++ )
    {
        if( i_vector[i] == x ) return true;
    }
    return false;
}

int line_test()
{

    std::cout<< "line_test function called\n";

    // create 50 random points
    for( int i=0; i<50; i++ )
    {
        v_points.emplace_back( std::rand(), std::rand() );
    }


    // a helper vector for keeping track of center("focus") points
    std::vector<Point> excluded_points;
    Point poynts[3];

    // find nearest 3 points
    find_nearest_mesh_points( v_points[15], v_points, excluded_points, poynts ); //FIXME

    // create meshes from those points
    Mesh( poynts[0], poynts[1], poynts[2] );
    


    return 0; // default successfull exit code
}



