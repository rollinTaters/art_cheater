#pragma once
#include "point.hpp"
#include "line.hpp"
#include "mesh.hpp"
#include "SFML/Graphics/Image.hpp"
#include <vector>

void dLine( sf::Image&, const Line&, const float thickness );

void draw_image_frame( sf::Image& im, const float thickness );

void draw_to_image( sf::Image&, std::vector<Mesh>& );
void draw_to_image( sf::Image&, std::vector<Line>& );
