#include "line.hpp"
#include "SFML/Graphics/Image.hpp"
#include <iostream>

int main()
{
    std::cout<<"Art Cheater v0.0.2\n";
    // seed the pRNG
    srand(time(NULL));

    int pixels_per_mm = 10;
    float work_area_x = 180;
    float work_area_y = 180;
    float image_border = 0;

    sf::Image sim_img;
    sim_img.create( (work_area_x + 2*image_border)*pixels_per_mm,
                    (work_area_y + 2*image_border)*pixels_per_mm,
                    sf::Color::White );

    // create the "masterpiece"
    create_line_artwork( work_area_x, work_area_y );
    draw_to_image( sim_img, pixels_per_mm );
    //print_gcode();

    // do stuff to image

    sim_img.saveToFile("line_sim_img.jpg");


    return 0;
}
