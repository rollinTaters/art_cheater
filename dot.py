# A quick script to generate gcode for creating a "dotted" version of image

from PIL import Image
import random

with Image.open("target_img2.jpg") as img:
    #img.convert(mode="L").show()   #DEBUG, show opened image as greyscale
    img = img.convert(mode="L")     # convert to greyscale
    pixel_array = img.convert(mode="L").load()   # convert to greyscale and load it into a pixel array

# simulate the output as an image for debug reasons
sim_img = Image.new(mode="L", size=img.size, color=255 )





# So here is the game plan;
# loop over the entire image with a kernel to get probability values for each pixel
# TODO: what if we just used the greyscale values as probabilities, do we even need a kernel?
# collapse those probabilities to "dots"
# apply "rules" to weed out "too close" or other bad dots. 

pixels_per_mm = 6  # a scaling factor
target_canvas_density = 0.4 # defines how much of image should be "black/dotted"
min_dot_dist = 2 # mm
canvas_x_max = img.width / pixels_per_mm
canvas_y_max = img.height / pixels_per_mm
points_per_iteration = 1000
max_iterations = 50
saturation_check_dia = 4
min_sat_threshold = 0.02

pen_sizes =     ( 0.3,        0.5,        0.7,       0.1 )
pen_threshold = ( (0.0, 0.3), (0.3, 0.5), (0.5, 0.8), (0.8, 1.0) )

def pick_pen_size( saturation = 0 ):
    # FIXME could be optimized but does it really need to?
    #for idx in pen_sizes.size:
        #if saturation > pen_threshold[idx][0] and saturation <= pen_threshold[idx][1]: return pen_sizes[idx] 

    if saturation > pen_threshold[0][0] and saturation <= pen_threshold[0][1]: return pen_sizes[0] 
    if saturation > pen_threshold[1][0] and saturation <= pen_threshold[1][1]: return pen_sizes[1] 
    if saturation > pen_threshold[2][0] and saturation <= pen_threshold[2][1]: return pen_sizes[2] 
    if saturation > pen_threshold[3][0] and saturation <= pen_threshold[3][1]: return pen_sizes[3] 
    print("pen size pick failed!")
    return pen_sizes[0] # fail mode

def sim_dot( pos = (0,0), size = 0.5 ):
    # puts a simulated dot on the sim_img, note: all values are in mm

    # calculate pixel position of dot
    center_pix_x = pos[0] * pixels_per_mm
    center_pix_y = pos[1] * pixels_per_mm
    p_radius = size/2 * pixels_per_mm

    # do a loop around center pix and color the radius
    lolled = 0
    no_lolled = 0
    for px in range( int(center_pix_x - p_radius), int(center_pix_x + p_radius) ):
        for py in range( int(center_pix_y - p_radius), int(center_pix_y + p_radius) ):
            # dont go out of bounds
            if( px >= img.width or py >= img.height ): continue
            if( (px-center_pix_x)**2 + (py-center_pix_y)**2 < p_radius*p_radius ):
                sim_img.putpixel( (px,py), 0 )
                lolled += 1
            else:
                no_lolled += 1
    # DEBUG
    #print(f"px_marked: {lolled} no_px_marked: {no_lolled}")

def get_area_saturation( pos = (0,0), size = 2 ):
    # returns how "black" the area around the point is
    # calculate pixel position of dot
    center_pix_x = pos[0] * pixels_per_mm
    center_pix_y = pos[1] * pixels_per_mm
    p_radius = size/2 * pixels_per_mm

    acc = 0
    checked_pixels = 1  # start at 1 so we dont div by zero if something fucks up
    # do a loop around center pix and color the radius
    for px in range( int(center_pix_x - p_radius), int(center_pix_x + p_radius) ):
        for py in range( int(center_pix_y - p_radius), int(center_pix_y + p_radius) ):
            # dont go out of bounds
            if( px >= img.width or py >= img.height ): continue
            if( (px-center_pix_x)**2 + (py-center_pix_y)**2 < p_radius*p_radius ):
                acc += 255 - pixel_array[px,py] 
                checked_pixels += 1
    # DEBUG
    #print( f"checked px: {checked_pixels}, saturation: {acc/(255*checked_pixels)}")
    return acc/(255*checked_pixels)

def check_already_dotted( pos = (0,0), size = 0.3 ):
    # calculate pixel position of dot
    center_pix_x = pos[0] * pixels_per_mm
    center_pix_y = pos[1] * pixels_per_mm
    p_radius = size/2 * pixels_per_mm

    # do a loop around center pix and color the radius
    for px in range( int(center_pix_x - p_radius), int(center_pix_x + p_radius) ):
        for py in range( int(center_pix_y - p_radius), int(center_pix_y + p_radius) ):
            # dont go out of bounds
            if( px >= img.width or py >= img.height ): continue
            if( sim_img.getpixel( (px,py) ) < 255) : return True
    return False

    

def rand_pixel_collapse( pix = (0,0), threshold = 300/255 ):
    cur_pixel_val = px[ pix[0], pix[1] ]
    new_pixel_val = 0 if cur_pixel_val > random.randrange(threshold*255) else 255
    sim_img.putpixel( (ix,iy), new_pixel_val )

## # define a look up table for "point" method of image class
## random_numbers_LUT = [ (0 if (ind > random.randrange(300)) else 255) for ind in range(256) ]
## # apply the LUT
## sim_img = img.point(random_numbers_LUT)


# some little info dump
print(f"canvas size: {canvas_x_max}x{canvas_y_max}")

# les do 500 random points
marked_points = 0
for iteration in range(max_iterations):
    for point_num in range(points_per_iteration):
        tx = random.random() * canvas_x_max
        ty = random.random() * canvas_y_max
        #print(f"doin a random point at: {tx},{ty})")
        sat = get_area_saturation((tx,ty), saturation_check_dia)
        pen_size = pick_pen_size(sat)
        if check_already_dotted( (tx,ty), pen_size ): continue
        if sat > min_sat_threshold:
            sim_dot( (tx,ty), pen_size )
            marked_points += 1
    print(f"iteration: {iteration} points marked: {marked_points}/{points_per_iteration*iteration}")

# do visual debugging
img.show()
sim_img.show()
sim_img.save("latest_sim_img","JPEG")

print("dot.")

