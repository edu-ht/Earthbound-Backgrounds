#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <math.h>

/*
 * Function: distort_frame
 * Parameters:
 *    src  - the source bitmap
 *    dst  - the bitmap to draw the distortion to
 *    t    - the time value (frame number) to compute
 *    type - one of: 0 = horizontal, 1 = horizontal interlaced, 2 = vertical
 * Notes:
 *  Source and destination bitmaps must be 32-bit bitmaps, and must not
 *  be the same bitmap.
 */
//void distort_frame(ALLEGRO_BITMAP *src, ALLEGRO_BITMAP *dst, int t, int type)
void distort_frame(ALLEGRO_BITMAP *src, int t, int type)
{

    // Some hard-coded distortion parameters
    float A = 16.0; // Amplitude
    float F = 0.1;  // Frequency
    float S = 0.1;  // Time scaling
    float C = 1.0;  // Compression (only used for vertical distortion)

    // Get pointers to raw bitmap data
    //int *srcdata = (int *)src->line[0];
    //int *dstdata = (int *)dst->line[0];

    int width = al_get_bitmap_width(src);
    int height = al_get_bitmap_height(src);

    // For each line...
    for (int y = 0; y < height; y++)
    {
        // Calculate the distortion offset for this line
        int offset = A * sinf(F * y + S * t);

        int src_x = 0; // new x position
        int src_y = y; // new y position

        if (type == 0)
            src_x = offset;
        else if (type == 1)
            src_x = (y % 2) ? offset : -offset;
        else if (type == 2)
            src_y = y * C + offset;

        // Wrap the y offset correctly - e.g., -1 should become height-1
        src_y = (src_y + height) % height;

        // Copy the line into the destination with translation
        // for (int x = 0; x < width; x++)
        // {
        //     // Also need to wrap the x offset
        //     src_x = (src_x + width) % width;
        //     //dstdata[y * width + x] = srcdata[src_y * width + src_x];

        //     al_draw_pixel(x, y, al_get_pixel(src, src_x, src_y));

        //     src_x++;
        // }
        src_x = (src_x + width) % width;
        al_draw_bitmap_region(src, src_x, src_y, width, height, 0, y, 0);
    }
}

int main(int argc, char **argv)
{
    al_init();
    al_init_image_addon();
    al_install_keyboard();
    int t = 0;

    // Create new display
    ALLEGRO_DISPLAY *display = al_create_display(255,255);

    // Keyboard
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    ALLEGRO_EVENT ev;
    bool doexit = false;
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    // Load bitmap
    ALLEGRO_BITMAP *bg1 = al_load_bitmap("bg.bmp");

    int mode = 0;

    // Main loop
    while(!doexit)
    {
        al_get_next_event(event_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch(ev.keyboard.keycode)
            {
                case ALLEGRO_KEY_ESCAPE:
                    doexit = true;
                    break;
                case ALLEGRO_KEY_0:
                    mode = 0;
                    break;
                case ALLEGRO_KEY_1:
                    mode = 1;
                    break;
                case ALLEGRO_KEY_2:
                    mode = 2;
                    break;
            }
        }

        //al_draw_bitmap(bg1, 0, 0, 0);
        distort_frame(bg1, t, mode);
        al_flip_display();
        t++;
    }

    // Destroy display and bitmap
    al_destroy_display(display);
    al_destroy_bitmap(bg1);
}