#include "line_work.hpp"
#include <cstdlib>  // for rand()
#include <list>
#include <vector>
#include <iostream>
#include <cmath>    // for floor()
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


std::vector<Mesh> create_line_artwork( int num_points)
{
    std::vector<Mesh> v_meshes;
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
    return v_meshes;

}

std::vector<Mesh> create_line_artwork_v2( int target_num_meshes, float point_search_radius )
{ // point search radius determines the max size of meshes
    // CAUTION: non-optimized code ahead

    // another approach:
    // - generate random mesh inner point (aka mesh focus)
    // - search for existing mesh corners, in a agiven radius
    // - if not found, generate them
    // - add newly created mesh into list
    std::vector<Mesh> v_meshes;

    int retry_count = 0;

    for( int ind = 0; ind < target_num_meshes; ind++ )
    {
        Point corner[3] = { Point(), Point(), Point() };    // explicitly initialized to be 0,0 points
        // generate new mesh inner point
        Point mip = Point( rand()%((int)(work_area_x)),
                           rand()%((int)(work_area_y)) );
        bool points_ready = false;
        bool request_redo = false;

        for( int p_ind = 0; p_ind < 3; p_ind++ )
        { // mesh corner point creation/selection
            for( size_t mesh_loop_ind = 0; mesh_loop_ind < v_meshes.size() && !points_ready; mesh_loop_ind++ )
            { // mesh corner reuse & mesh intersection check
                Mesh m = v_meshes[mesh_loop_ind];
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
                        if(p_ind < 3) p_ind++; // dont override if we have a point selected at p_ind
                        if(p_ind >= 2){ points_ready = true; break; }  // dont overflow corner[]
                    }
                }
            }
            
            if( request_redo ) break; // if we fucked up along the way and need to restart this mesh

            // if a suitable corner has not been found yet, create a new one
            corner[p_ind] = mip +
                            Point( rand()%(int)(point_search_radius),
                                   rand()%(int)(point_search_radius) ) -
                            Point( point_search_radius/2.f,
                                   point_search_radius/2.f );
            // dont let the new point wander beyond work area bounds
            corner[p_ind] = Point( std::min( corner[p_ind].getX(), work_area_x ),
                                   std::min( corner[p_ind].getY(), work_area_y ) );
            corner[p_ind] = Point( std::max( corner[p_ind].getX(), 0.f ),
                                   std::max( corner[p_ind].getY(), 0.f ) );

            // check if newly created point lies inside an existing mesh, if so request redo
            for( Mesh m : v_meshes )
            {
                if( m.contains( corner[p_ind] ) )
                {
                    request_redo = true;
                    break;
                }
            }

            if( request_redo ) break; // if we fucked up along the way and need to restart this mesh

            // finally declare all corner's points ready for mesh creation
            if( p_ind >= 2 ) points_ready = true;
        }

        // Verbose output
        if( retry_count%1000 == 1 && retry_count != 1 ) std::cout<<"trying real hard...\n";
        if( retry_count > 1e6 ) break;  // max iteration stopping hardpoint

        if( !points_ready )
        {
            // DEBUG:
            //std::cout<<"Point pick failed in artwork v2, retrying...\n";
            retry_count++;
            ind--;
            continue;
        }
        // if we picked all corner points, create new mesh
        v_meshes.emplace_back( corner[0], corner[1], corner[2] );
        // generate infill for the meshes
        v_meshes.back().generateInfill(0.98f);

    }
    std::cout<<" = Line artwork (v2) created with " << v_meshes.size() << " meshes = \ntook "<<retry_count<<" failed meshing attempts\n";
    return v_meshes;
}

std::vector<Mesh> create_line_artwork_v3( int target_num_meshes, float max_wiggle_amount )
{
    std::vector<Mesh> v_meshes;
    // Temp data storage
    std::vector<Point> fixme;

    /// XXX XXX XXX

    // find out num vertices for each axis
    float area_per_mesh = work_area_x * work_area_y / (float)target_num_meshes;
    int num_vert_x = (int)floor( work_area_x / area_per_mesh );
    int num_vert_y = (int)floor( work_area_y / area_per_mesh );
    float spacing_vertices_x = work_area_x / num_vert_x;
    float spacing_vertices_y = work_area_y / num_vert_y;
    for( float cx = spacing_vertices_x/2; cx < work_area_x; cx += spacing_vertices_x )
    {
        // TODO offset even/odd lines by spacing_vertices to avoid linear grid
        for( float cy = spacing_vertices_y/2; cy < work_area_y; cy += spacing_vertices_y )
        {
            // add a vertice point
        }
    }


    return v_meshes;
}



