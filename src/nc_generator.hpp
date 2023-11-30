#pragma once

#include "line.hpp"
#include "mesh.hpp"
#include <vector>

void print_gcode( std::vector<Mesh>& v_meshes );
void print_gcode( std::vector<Line>& v_lines );

// finds the closest line to the head position and moves on that,
// head up, head down and draw, head back up if requested
void moveOnClosestLine( std::list<Line>& lines, float& HPx, float& HPy, bool keep_head_down_after_move );
