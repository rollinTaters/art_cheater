#include "line.hpp"
#include "SFML/Graphics.hpp"
#include <iostream>
#include <vector>
#include <list>
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

bool Point::operator==( Point& other )
{
    if( m_x == other.m_x && m_y == other.m_y )
        return true;
    return false;
}

int Point::dist_to( Point p_other )
{   // returns the distance between this and given point
    return std::sqrt( (m_x - p_other.m_x)^2 + (m_y - p_other.m_y)^2 );
}


Line::Line(int sx, int sy, int ex, int ey ):
    m_start_x(sx), m_start_y(sy),
    m_end_x(ex), m_end_y(ey)
{   }

Mesh::Mesh( Point p1, Point p2, Point p3 ):
    m_p1(p1), m_p2(p2), m_p3(p3)
{   }

//FIXME
bool find_nearest_three(
        Point focus_point,
        std::vector<Point> *i_points,
        std::vector<Point> *i_excluded_points )
{
    // create a temp data container
    //Point temp_picked = { Point(), Point(), Point() };
    Point temp_picked[3];
    int distance = RAND_MAX;    // #TODO fix this later when adjusting sizes

    // for all points in i_points, find nearest
    for( int i=0; i < i_points->size(); i++ )
    {
        if( focus_point == (*i_points)[i] ) continue;
        if( focus_point.dist_to( (*i_points)[i] ) < distance )
        {
            distance = focus_point.dist_to( (*i_points)[i] );
            temp_picked[0] = (*i_points)[i];
        }
    }
    std::cout<< "picked:" << temp_picked[0].m_x << " and stuff\n";
    return true;

    // add the found point in a temporary "do not use" points list

    // do the same shit again
    // add the found point in a temporary "do not use" points list

    // and again

    // add the "center" point to the excluded points list
}

int line_test()
{

    std::cout<< "taha in line test function\n";

    // create 50 random points
    for( int i=0; i<50; i++ )
    {
        v_points.emplace_back( std::rand(), std::rand() );
    }


    // a helper vector for keeping track of center("focus") points
    std::vector<Point> excluded_points;
    // find nearest 3 points
    find_nearest_three( v_points[15], &v_points, &excluded_points ); //FIXME
    // create meshes from those points
    Mesh( v_points[0], v_points[1], v_points[2] );
    


    return 0; // default successfull exit code
}

void test()
{

    std::cout<<v_lines[0].m_start_x << std::endl;
    std::cout<< v_lines.size() << std::endl;
}


