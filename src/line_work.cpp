#include "line_work.hpp"
#include <cstdlib>  // for rand()
#include <list>
#include <vector>
#include <iostream>
//#include <cmath>    // for sqrt()
//#include <list>

/*
   -pick random points
   -mesh points (triangles)
   -pick one point of mesh to be the "focus"
   -draw lines from focus to edges of mesh
   -store those lines in a vector
   -profit

   */

// ======== Global scoped Storage ========
std::vector<Mesh> v_meshes; // this one is actually local for now
extern float work_area_x;
extern float work_area_y;
extern float image_border;
extern int pixels_per_mm;

template <typename typ>
bool vectorContains( const typ& x, const std::vector<typ>& i_vector )
{
    for( size_t i = 0; i < i_vector.size(); i++ )
    {
        if( i_vector[i] == x ) return true;
    }
    return false;
}


void create_line_artwork( int num_points)
{
    std::cout<< "line_test function called\n";

    // containers for points
    std::list<Point> all_points;
    std::list<Point> focus_points;
    std::list<Point> vertex_points;
    Point poynts[3];

    // create some random points
    for( int i = 0; i < num_points; i++ )
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
        v_meshes[mi].generateInfill( 0.98f );

        // DEBUG: print infill count
        //std::cout<< "Mesh " << mi << " generated " << v_meshes[mi].infill.size() << " infill lines\n";
        // DEBUG: print mesh info
        //std::cout<< v_meshes[mi] <<"\n";
    }

}

void create_line_artwork_v2( int target_num_meshes, float point_search_radius )
{ // point search radius determines the max size of meshes
    // CAUTION: non-optimized code ahead

    // another approach:
    // - generate random mesh inner point (aka mesh focus)
    // - search for existing mesh corners, in a agiven radius
    // - if not found, generate them
    // - add newly created mesh into list


    for( int ind = 0; ind < target_num_meshes; ind++ )
    {
        Point corner[3] = { Point(), Point(), Point() };    // explicitly initialized to be 0,0 points
        // generate new mesh inner point
        Point mip = Point( rand()%((int)(work_area_x)),
                           rand()%((int)(work_area_y)) );
        bool points_ready = false;
        bool request_redo = false;

        for( int p_ind = 0; p_ind < 3; p_ind++ )
        { // for each corner point
            for( Mesh m : v_meshes )
            {
                // check if mesh inner point is already inside a mesh
                if( m.contains( mip ) )
                {   // re do this mesh
                    request_redo = true;
                    break;
                }

                // search for existing mesh corner points we can reuse
                std::uint8_t points_taken = 0;
                for( Point mcp : m.getPoints() )
                { // mcp: mesh corner point
                    if( mip.dist_to( mcp ) < point_search_radius &&
                        corner[0] != mcp && corner[1] != mcp && corner[2] != mcp &&
                        points_taken < 3 )
                    {
                        corner[p_ind] = mcp;
                        points_taken++; // makes sure we grab max 2 points from a single mesh
                    }
                }
            }
            
            if( request_redo ) break; // if we fucked up along the way and need to restart this mesh

            // if a suitable corner has not been found yet, create a new one
            corner[p_ind] = mip + Point( rand()%(int)(point_search_radius), rand()%(int)(point_search_radius) );
            // dont let the new point wander beyond work area bounds
            corner[p_ind] = Point( std::min( corner[p_ind].getX(), work_area_x ),
                                   std::min( corner[p_ind].getY(), work_area_y ) );
            // TODO: check if newly created point lies inside an existing mesh, if so request redo

            // finally declare all corner's points ready for mesh creation
            if( p_ind == 2 ) points_ready = true;
        }

        if( !points_ready )
        {
            // DEBUG:
            std::cout<<"Point pick failed in artwork v2, retrying...\n";

            ind--;
            continue;
        }
        // if we picked all corner points, create new mesh
        v_meshes.emplace_back( corner[0], corner[1], corner[2] );
        // generate infill for the meshes
        v_meshes.back().generateInfill(0.98f);

    }
    std::cout<<" = Line artwork (v2) created with " << v_meshes.size() << " meshes\n";
    return;
}

void print_gcode()
{   // prints gcode to stream

    float z_clearance_level = 3.f;

    std::cout<<"G21 ;metric values\n"
             <<"G90 ;absolute positioning\n"
//             <<"G28 X0 Y0 ;move/set to xy zero\n"
//             <<"G28 Z0 ; move/set to Z zero\n"
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

void dLine( sf::Image& im, const Line& l, const float thickness )
{   // draws the line on the image using CPU
    float ppmm = pixels_per_mm;
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
    int canvas_clearance = 0.6*ppmm;

    for( int x = std::max( std::min(x1, x2)-border, canvas_clearance*ppmm );
         x < std::min( (work_area_x - canvas_clearance)*ppmm, std::max(x1, x2)+border );
         x++ )
    {   // line x span + border, constrained in canvas_clearance dist away from edges
        for( int y = std::max( std::min(y1, y2)-border, canvas_clearance*ppmm );
             y < std::min( (work_area_y - canvas_clearance)*ppmm, std::max(y1, y2)+border );
             y++ )
        {   // line y span + border
            if( x < 0 || y < 0 || x > im_w || y > im_h ) continue;
            if( std::abs( (x2-x1) * (y1-y) - (x1-x) * (y2-y1) ) / l_len < (thickness*ppmm/2.f) )
                im.setPixel(x, y, sf::Color::Black );
        }
    }
}

void draw_to_image( sf::Image& im )
{
    for( Mesh mesh : v_meshes )
    {
        // create a temporary mesh and move it, so that we have a border around our canvas
        Mesh m = mesh;
        m.move( image_border, image_border );

        // mesh bounds
        dLine( im, Line(m.m_p1, m.m_p2), 0.7f );
        dLine( im, Line(m.m_p2, m.m_p3), 0.7f );
        dLine( im, Line(m.m_p3, m.m_p1), 0.7f );

        // infill lines
        for( Line l: m.infill )
        {
            dLine( im, l, 0.5f );
        }
    }
}

