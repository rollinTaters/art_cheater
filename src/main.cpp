#include "point.hpp"
#include "line.hpp"
#include "mesh.hpp"
#include "line_work.hpp"
#include "SFML/Graphics/Image.hpp"
#include <iostream>

// Global scoped program variables, also gets used in other modules
int pixels_per_mm = 10;
float work_area_x = 400;
float work_area_y = 400;
float image_border = 30;


int main()
{
    std::cout<<"Art Cheater v0.0.2\n";
    // seed the pRNG
    srand(time(NULL));


    sf::Image sim_img;
    sim_img.create( (work_area_x + 2*image_border)*pixels_per_mm,
                    (work_area_y + 2*image_border)*pixels_per_mm,
                    sf::Color::White );

    // create the "masterpiece"
    //create_line_artwork( 1000 );
    create_line_artwork_v2( 500, 40 );
    draw_to_image( sim_img );
    //print_gcode();

    // do stuff to image

    sim_img.saveToFile("line_sim_img.jpg");


    return 0;
}
