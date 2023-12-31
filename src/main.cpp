#include "point.hpp"
#include "line.hpp"
#include "mesh.hpp"
#include "simulation_image.hpp"
#include "line_work.hpp"
#include "pattern_work.hpp"
#include "nc_generator.hpp"
#include "SFML/Graphics/Image.hpp"
#include <iostream>
#include <string>
#include <ctime> // adding time to file names

// Global scoped program variables, also gets used in other modules
int pixels_per_mm = 10;
float work_area_x = 400;
float work_area_y = 400;
float image_border = 30;


int main()
{
    std::cout<<"Art Cheater v0.0.4\n";
    // seed the pRNG
    srand(time(NULL));

    // local time finding to get a time string
    char time_string[std::size("yymmdd_hhmm")];
    std::time_t curr_time = std::time(nullptr);
    std::strftime( time_string, std::size(time_string), "%y%m%d_%H%M", std::localtime(&curr_time));

    std::string nc_file_name = "artwork_" + std::string(time_string);


    sf::Image sim_img;
    sim_img.create( (work_area_x + 2*image_border)*pixels_per_mm,
                    (work_area_y + 2*image_border)*pixels_per_mm,
                    sf::Color::White );

    // create the "masterpiece"
    //create_line_artwork( 1000 );
    //create_line_artwork_v2( 500, 40 );
    std::vector<Line> pattern_aw = create_pattern_artwork();

    draw_image_frame( sim_img, 0.7f );
    draw_to_image( sim_img, pattern_aw );
    if( print_gcode( nc_file_name, pattern_aw ) )
        std::cout<<"NC file saved as: "<<nc_file_name<<"\n";

    // do stuff to image
    sim_img.saveToFile("line_sim_img.jpg");


    return 0;
}
