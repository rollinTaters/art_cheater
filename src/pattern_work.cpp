#include "pattern_work.hpp"

extern float work_area_x;
extern float work_area_y;

std::vector<Line> create_pattern_artwork( )
{
    // Data
    std::vector<Line> v_lines;

    float pattern_size = 50.f;
    int x_number_of_patterns = floor( work_area_x / pattern_size );
    int y_number_of_patterns = floor( work_area_y / pattern_size );
    float x_pattern_spacing = work_area_x / x_number_of_patterns;
    float y_pattern_spacing = work_area_y / y_number_of_patterns;

    for( float cx = x_pattern_spacing/2; cx <= (work_area_x); cx += x_pattern_spacing ){
    for( float cy = y_pattern_spacing/2; cy <= (work_area_y); cy += y_pattern_spacing ){
        put_pattern( v_lines, Point(cx,cy), (pattern_size/2)-3 );
    } }

    return v_lines;
}

void put_pattern(
        std::vector<Line>& layns,
        const Point center_point,
        const float radi_outer,
        const float width_arm,
        const int num_arms )
{
    // derived parameters
    float angle_seperation = 2*PI / (float)num_arms;
    float theta = 1.5 * angle_seperation;
    float radi_inner = width_arm / ( 2*sin(theta) );

    // ARM GENERATION
    for( int arm_ind = 0; arm_ind < num_arms; arm_ind++ )
    {
        // create points
        Point p1( 0, radi_inner );
        Point p2( 0, radi_outer );

        p1.move( radi_inner * sin(theta),
                 -radi_inner * (1-cos(theta)) );
        p2.move( radi_inner * sin(theta),
                 -radi_inner * (1-cos(theta)) );

        Point p3( 0, radi_outer );

        Point p4( -p2.getX(), p2.getY() );
        Point p5( -p1.getX(), p1.getY() );

        // link points to lines
        Line l1( p1, p2 );
        Line l2( p2, p3 );
        Line l3( p3, p4 );
        Line l4( p4, p5 );

        l1.rotate( arm_ind * angle_seperation );
        l2.rotate( arm_ind * angle_seperation );
        l3.rotate( arm_ind * angle_seperation );
        l4.rotate( arm_ind * angle_seperation );

        l1.move( center_point );
        l2.move( center_point );
        l3.move( center_point );
        l4.move( center_point );

        layns.push_back( l1 );
        layns.push_back( l2 );
        layns.push_back( l3 );
        layns.push_back( l4 );
    }
}
