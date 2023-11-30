#include "nc_generator.hpp"
#include <iostream>
#include <string>
#include <list>


// Temporary global data
std::string start_string = 
    "G21 ;metric values\n"
    "G90 ;absolute positioning\n";
    //"G28 X0 Y0 ;move/set to xy zero\n"
    //"G28 Z0 ; move/set to Z zero\n" ;

// global variables for general nc use
float z_clearance_level = 3.f;
float HPx = 0; // head position x
float HPy = 0; // head position y

void print_gcode( std::vector<Mesh>& v_meshes )
{   // prints gcode to stream

    std::cout<< start_string;
    std::cout<<"G01 Z"<<z_clearance_level<<" F400 ;move clearance amount up\n";

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
            HPx = l.getStart().getX();
            HPy = l.getStart().getY();
        }
    }
}

void print_gcode( std::vector<Line>& v_lines )
{
    float target_x;
    float target_y;

    std::cout<< start_string;
    std::cout<<"G01 Z"<<z_clearance_level<<" F400 ;move clearance amount up\n";

    // we need to quickly add/remove lines from memory, convert it to list
    std::list<Line> leyn;
    for( Line& l : v_lines )
    {
        leyn.emplace_back( l );
    }

    // find the closest line and draw it, then enter the loop
    moveOnClosestLine( leyn, HPx, HPy, true );

    while( !leyn.empty() )
    {
        bool line_drawn = false;

        // check to see if end/start coincides with any other line
        for( auto it = leyn.begin(); it != leyn.end(); it++ )
        {
            Point s = it->getStart();
            Point e = it->getEnd();
            if( s.getX() == HPx && s.getY() == HPy )
            { // we are at start point of line
                std::cout<< "G01 X"<< e.getX() << " Y" << e.getY() << "\n";
                HPx = e.getX();
                HPy = e.getY();
                it = leyn.erase(it);
                line_drawn = true;
            }
            else if( e.getX() == HPx && e.getY() == HPy )
            {
                std::cout<< "G01 X"<< s.getX() << " Y" << s.getY() << "\n";
                HPx = s.getX();
                HPy = s.getY();
                it = leyn.erase(it);
                line_drawn = true;
            }
        }
        
        if( line_drawn ) continue;
        // we couldnt find a connected line, draw closest to you
        moveOnClosestLine( leyn, HPx, HPy, true );
    }
    // raise head after operation complete
    std::cout<<"G01 Z"<<z_clearance_level<<" F400 ;move clearance amount up\n";
    std::cout<<"; lines drawing complete\n";

}

void moveOnClosestLine( std::list<Line>& lines, float& HPx, float& HPy, bool keep_head_down_after_move )
{
    Line closest_line;
    float closest_dist = 1e10;
    Point closest_point;
    float target_x;
    float target_y;

    for( auto it = lines.begin(); it != lines.end(); it++ )
    {
        if( it->getStart().dist_to( Point(HPx,HPy) ) < closest_dist )
        {
            closest_line = *it;
            closest_dist = it->getStart().dist_to( Point(HPx,HPy) );
            closest_point = it->getStart();
        }else if( it->getEnd().dist_to( Point(HPx,HPy) ) < closest_dist )
        {
            closest_line = *it;
            closest_dist = it->getEnd().dist_to( Point(HPx,HPy) );
            closest_point = it->getEnd();
        }
    }

    // raise head
    std::cout<<"G01 Z"<<z_clearance_level<<" F400 ;move clearance amount up\n";
    // move to line start
    std::cout<<"G01 X"<< closest_point.getX() <<" Y"<< closest_point.getY() << "\n";
    // find the other end of the closest line
    if( closest_point == closest_line.getStart() )
    {
        target_x = closest_line.getEnd().getX();
        target_y = closest_line.getEnd().getY();
    }else{
        target_x = closest_line.getStart().getX();
        target_y = closest_line.getStart().getY();
    }
    // update head position
    HPx = target_x;
    HPy = target_y;
    // head down
    std::cout<<"G01 Z0\n";
    // draw line
    std::cout<<"G01 X"<< target_x <<" Y"<< target_y <<"\n";
    // raise head if asked for
    if( !keep_head_down_after_move )
        std::cout<< "G00 Z"<< z_clearance_level <<"\n";
    // remove line from list
    lines.remove( closest_line );
}

