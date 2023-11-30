#pragma once
#include "point.hpp"
#include "line.hpp"
#include "mesh.hpp"
#include "SFML/Graphics/Image.hpp"
#include <vector>


template <typename typ>
bool vectorContains( const typ&, const std::vector<typ>& );

std::vector<Mesh> create_line_artwork( int num_points);
std::vector<Mesh> create_line_artwork_v2( int target_num_meshes, float point_search_radius );
std::vector<Mesh> create_line_artwork_v2( int target_num_meshes, float max_wiggle_amount );
