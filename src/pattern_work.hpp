#pragma once

#include "point.hpp"
#include "line.hpp"
#include "simulation_image.hpp"
#include <vector>
#include <cmath>

void put_pattern(
        std::vector<Line>& layns,
        const Point center_point,
        const float radi_outer = 24.f,
        const float width_arm = 9.f,
        const int num_arms = 10 );

std::vector<Line> create_pattern_artwork( );
