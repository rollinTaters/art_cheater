#include "simulation_image.hpp"

// ======== Global scoped Storage ========
extern float work_area_x;
extern float work_area_y;
extern float image_border;
extern int pixels_per_mm;

void dLine( sf::Image& im, const Line& l, const float thickness )
{   // draws the line on the image using CPU
    float ppmm = pixels_per_mm;
    Point p1 = l.getStart();
    Point p2 = l.getEnd();
    int im_h = im.getSize().y;
    int im_w = im.getSize().x;
    float x1 = p1.getX()*ppmm;
    float x2 = p2.getX()*ppmm;
    float y1 = p1.getY()*ppmm;
    float y2 = p2.getY()*ppmm;
    float l_len = l.getLen()*ppmm;
    int border = 0.4*ppmm;

    for( int x = std::min(x1, x2)-border ;
         x < std::max(x1, x2)+border ;
         x++ )
    {   // line x span + border
        for( int y = std::min(y1, y2)-border ;
             y < std::max(y1, y2)+border ;
             y++ )
        {   // line y span + border
            if( x < 0 || y < 0 || x >= im_w || y >= im_h ) continue;
            if( std::abs( (x2-x1) * (y1-y) - (x1-x) * (y2-y1) ) / l_len < (thickness*ppmm/2.f) )
                im.setPixel(x, y, sf::Color::Black );
        }
    }
}

void draw_image_frame( sf::Image& im, const float thickness )
{
    // frame drawing
    Line fr1 = Line(Point(0,0), Point(work_area_x,0)).move(image_border,image_border);
    Line fr2 = Line(Point(0,0), Point(0, work_area_y)).move(image_border,image_border);
    Line fr3 = Line(Point(0,work_area_y), Point(work_area_x, work_area_y)).move(image_border,image_border);
    Line fr4 = Line(Point(work_area_x,0), Point(work_area_x, work_area_y)).move(image_border,image_border);
    dLine( im, fr1, thickness );
    dLine( im, fr2, thickness );
    dLine( im, fr3, thickness );
    dLine( im, fr4, thickness );
}

void draw_to_image( sf::Image& im, std::vector<Mesh>& v_meshes )
{
    // frame drawing
    Line fr1 = Line(Point(0,0), Point(work_area_x,0)).move(image_border,image_border);
    Line fr2 = Line(Point(0,0), Point(0, work_area_y)).move(image_border,image_border);
    Line fr3 = Line(Point(0,work_area_y), Point(work_area_x, work_area_y)).move(image_border,image_border);
    Line fr4 = Line(Point(work_area_x,0), Point(work_area_x, work_area_y)).move(image_border,image_border);
    dLine( im, fr1, 5.0f );
    dLine( im, fr2, 5.0f );
    dLine( im, fr3, 5.0f );
    dLine( im, fr4, 5.0f );

    for( Mesh mesh : v_meshes )
    {
        // create a temporary mesh and move it, so that we have a border around our canvas
        Mesh m = mesh;
        m.move( image_border, image_border );

        // mesh bounds
        dLine( im, Line(m.m_p1, m.m_p2), 0.7f );
        dLine( im, Line(m.m_p2, m.m_p3), 0.7f );
        dLine( im, Line(m.m_p3, m.m_p1), 0.7f );

        // infill lines
        for( Line l: m.infill )
        {
            dLine( im, l, 0.5f );
        }
    }

}

void draw_to_image( sf::Image& im, std::vector<Line>& v_lines )
{
    for( Line layn : v_lines )
    {
        // create a temp line and move it, we may have a border around our image
        Line l = layn;
        l.move( image_border, image_border );
        dLine( im, l, 0.7f );
    }

}
