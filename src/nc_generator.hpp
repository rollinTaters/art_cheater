#pragma once

#include "line.hpp"
#include "mesh.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>


std::ostream& generate_gcode( std::ostream&, const std::vector<Mesh>& v_meshes );
std::ostream& generate_gcode( std::ostream&, const std::vector<Line>& v_lines );

// finds the closest line to the head position and moves on that,
// head up, head down and draw, head back up if requested
std::ostream& moveOnClosestLine( std::ostream&, std::list<Line>& lines, float& HPx, float& HPy, bool keep_head_down_after_move );

template< typename CT >
bool print_gcode( std::string file_name, const std::vector<CT> i_vector )
{
    extern std::string start_string;
    std::fstream fs;
    fs.open( file_name + ".gcode" , std::fstream::out | std::fstream::trunc );

    if( fs.is_open() )
    {
        fs<< start_string;
        generate_gcode( fs, i_vector );
    }else{
        std::cout<< "Cannot open file to write!!\n";
        return false;
    }

    fs.flush(); // to save the file
    fs.close();

    return true;
}
