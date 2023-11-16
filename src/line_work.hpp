#pragma once
#include "point.hpp"
#include "line.hpp"
#include "mesh.hpp"
#include "SFML/Graphics/Image.hpp"
#include <vector>


template <typename typ>
bool vectorContains( const typ&, const std::vector<typ>& );

void create_line_artwork( int num_points);
void create_line_artwork_v2( int target_num_meshes, float point_search_radius );
void print_gcode();
void draw_to_image( sf::Image& );
