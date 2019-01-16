/*
 * Quick-and-dirty battle animation distorter in Allegro
 *  - Mr. Accident :3
 */
#include <allegro5/allegro.h>
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
void distort_frame(BITMAP* src, BITMAP* dst, int t, int type)
{
	ASSERT(source != dest);
	ASSERT(source != NULL && dest != NULL);
	ASSERT(source->w == dest->w && source->h == dest->h);
	ASSERT(type = 0 || type == 1 || type == 3);

	// Some hard-coded distortion parameters
	float A = 16.0;		// Amplitude
	float F = 0.1;		// Frequency
	float S = 0.1;		// Time scaling
	float C = 1.0;		// Compression (only used for vertical distortion)

	// Get pointers to raw bitmap data
	int* srcdata = (int*)src->line[0];
	int* dstdata = (int*)dst->line[0];

	int width = src->w;
	int height = src->h;

	// For each line...
	for(int y = 0; y < height; y++)
	{
		// Calculate the distortion offset for this line
		int offset = A * sinf(F * y + S * t);

		int src_x = 0;	// new x position
		int src_y = y;	// new y position

		if(type == 0)
			src_x = offset;
		else if(type == 1)
			src_x = (y % 2)? offset : -offset;
		else if(type == 2)
			src_y = y * C + offset;

		// Wrap the y offset correctly - e.g., -1 should become height-1
		src_y = (src_y + height) % height;

		// Copy the line into the destination with translation
		for(int x = 0; x < width; x++)
		{
			// Also need to wrap the x offset
			src_x = (src_x + width) % width;
			dstdata[y * width + x] = srcdata[src_y * width + src_x];
			src_x++;
		}
	}
}


/*
 * Game timer control
 */
volatile int game_time = 0;
void game_timer() {
	game_time++;
}
END_OF_FUNCTION(game_timer)

volatile int fps = 0;
volatile int frame_count = 0;
void fps_timer()
{
	fps = frame_count;
	frame_count = 0;
}
END_OF_FUNCTION(fps_timer)


/*
 * The entry point of the program
 */
int main(int argc, char** argv)
{
	allegro_init();
	install_timer();
	install_keyboard();

	// Set up game timers
	install_int_ex(game_timer, BPS_TO_TIMER(60));
	install_int_ex(fps_timer, BPS_TO_TIMER(1));
	
	// Set graphics mode
	set_color_depth(32);
	if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 512, 512, 0, 0))
	{
		allegro_message("Failed to set GFX mode.");
		return -1;
	}

	BITMAP* image = load_bitmap("bg.bmp", NULL);
	
	if(!image) {
		allegro_message("Failed to create bitmap.");
		return -1;
	}

	// Create distortion buffers and a screen back buffer
	BITMAP* dist1 = create_bitmap(image->w, image->h);
	BITMAP* dist2 = create_bitmap(image->w, image->h);
	BITMAP* dist3 = create_bitmap(image->w, image->h);
	BITMAP* back_buffer = create_bitmap(SCREEN_W, SCREEN_H);
	
	// The current frame of distortion
	int distort_time = 0;

	// Game loop
	while(!key[KEY_ESC])
	{
		while(game_time > 0)
		{
			// Compute three different distortions from the original image
			distort_frame(image, dist1, distort_time, 0);
			distort_frame(image, dist2, distort_time, 1);
			distort_frame(image, dist3, distort_time, 2);
			distort_time++;
			game_time--;
		}

		// Draw all three distortions, plus the original image
		blit(image, back_buffer, 0, 0, 0,   0,   256, 256);
		blit(dist1, back_buffer, 0, 0, 256, 0,   256, 256);
		blit(dist2, back_buffer, 0, 0, 0,   256, 256, 256);
		blit(dist3, back_buffer, 0, 0, 256, 256, 256, 256);

		// Draw some text with nifty shadows :D
		textprintf_ex(back_buffer, font, 1, 1, 0, -1, "Running at %d FPS", fps);
		textprintf_ex(back_buffer, font, 0, 0, 0xFFFFFF, -1, "Running at %d FPS", fps);
		textout_ex(back_buffer, font, "Original image", 6, 246, 0, -1);
		textout_ex(back_buffer, font, "Original image", 5, 245, 0xFFFFFF, -1);
		textout_ex(back_buffer, font, "Horizontal", 262, 246, 0, -1);
		textout_ex(back_buffer, font, "Horizontal", 261, 245, 0xFFFFFF, -1);
		textout_ex(back_buffer, font, "Horizontal interlaced", 6, 502, 0, -1);
		textout_ex(back_buffer, font, "Horizontal interlaced", 5, 501, 0xFFFFFF, -1);
		textout_ex(back_buffer, font, "Vertical compression", 257, 502, 0, -1);
		textout_ex(back_buffer, font, "Vertical compression", 256, 501, 0xFFFFFF, -1);
		
		// Copy the back buffer to the screen
		blit(back_buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

		// Increment number of frames drawn this second
		frame_count++;
	}

	return 0;
}
END_OF_MAIN()
