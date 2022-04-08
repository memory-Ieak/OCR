#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "pixel_operations.h"
#include "detection_character.h"
#include "SDL_rotozoom.h"
#include "Display.h"

//check if the horizontal line is white
int is_white_horizontal(SDL_Surface* image_surface, int x_min, int x_max, int y){
	int res = 1;
	int i = x_min;
	while(i < x_max && res == 1){
		Uint32 pixel = get_pixel(image_surface, i, y);
		Uint8 r, g, b;
		SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
		if(r==0 && g==0 && b==0){
			res = 0;
		}
		i++;
	}
	return res;
}

//check if the vertical line is white
int is_white_vertical(SDL_Surface* image_surface, int x, int y_min, int y_max){
	int res = 1;
	int i = y_min;
	while(i < y_max && res == 1){
		Uint32 pixel = get_pixel(image_surface, x, i);
		Uint8 r, g, b;
		SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
		if(r==0 && g==0 && b==0){
			res = 0;
		}
		i++;
	}
	return res;
}

//draw a green line horizontally for text blocks
void cut_horizontal(SDL_Surface* image_surface, int x_min, int x_max, int y){
	for(int i = x_min; i <= x_max; i++){
		Uint32 pixel = get_pixel(image_surface, i, y);
		Uint8 r, g, b;
		SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
		if((r == 35 && g == 255 && b == 0) || (r == 255 && g == 0 && b == 0)){
			r = 255;
			g = 0;
			b = 0;
		}
		else{
			r = 35;
			g = 255;
			b = 0;
		}
		pixel = SDL_MapRGB(image_surface->format, r, g, b);
		put_pixel(image_surface, i, y, pixel);
	}
}

//draw a green line vertically for text blocks
void cut_vertical(SDL_Surface* image_surface, int x, int y_min, int y_max){
	for(int i = y_min; i <= y_max; i++){
		Uint32 pixel = get_pixel(image_surface, x, i);
		Uint8 r, g, b;
		SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
		if((r == 35 && g == 255 && b == 0) || (r == 255 && g == 0 && b == 0)){
			r = 255;
			g = 0;
			b = 0;
		}
		else{
			r = 35;
			g = 255;
			b = 0;
		}
		pixel = SDL_MapRGB(image_surface->format, r, g, b);
		put_pixel(image_surface, x, i, pixel);
	}
}

//draw a green lines for text blocks
int cut_bloc(SDL_Surface* image_surface, int x_min, int x_max, int y_min, int y_max, int space){
    
    int horizontal = 0;
    int vertical = 0;
	
	int previous_vertical = 1;
	int val = space;
	
	for(int x = x_min; x < x_max; x++){
		int bool = is_white_vertical(image_surface, x, y_min, y_max);
		if(bool == 1){
			if(space > 0){
				space -= 1;
			}
			else{
				if(previous_vertical == 0){
					vertical = 1;
					previous_vertical = 1;
					space = 10;
					cut_vertical(image_surface, x, y_min, y_max);
				}
			}
		}
		else{
			space = val;
			previous_vertical = 0;
		}
	}
	
	int nothing = 0;
	if(horizontal == 0 && vertical == 0){
		nothing = 1;
	}
	
	int previous_horizontal = 1;
    space = val;
    
	for(int y = y_min; y < y_max; y++){
		int bool = is_white_horizontal(image_surface, x_min, x_max, y);
		if(bool == 1){
			if(space > 0){
				space -= 1;
			}
			else{	
				if(previous_horizontal == 0){
					horizontal = 1;
					previous_horizontal = 1;
					space = 10;
					cut_horizontal(image_surface, x_min, x_max, y);
				}
			}
		}
		else{
			space = val;
			previous_horizontal = 0;
		}
	}
	
	return nothing;
}

//check if the horizontal line is green
int is_line_h(SDL_Surface* image_surface, int x, int y){
	int res = 0;
	Uint32 pixel = get_pixel(image_surface, x+1, y);
	Uint8 r, g, b;
	SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
	if(r==35 && g == 255 && b == 0){
		res = 1;
	}
	return res;
}

//check if the vertical line is green
int is_line_v(SDL_Surface* image_surface, int x, int y){
	int res = 0;
	Uint32 pixel = get_pixel(image_surface, x, y+1);
	Uint8 r, g, b;
	SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
	if(r==35 && g == 255 && b == 0){
		res = 1;
	}
	return res;
}

//calculate the height of the block
int search_height(SDL_Surface* image_surface, int height_min, int height_max, int x){
	int res = -1;
	int find = 0;
	int i = height_min;
	while(i < height_max && find == 0){
		Uint32 pixel = get_pixel(image_surface, x, i);
		Uint8 r, g, b;
		SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
		if(r==255 && g == 0 && b == 0){
			int line = is_line_h(image_surface, x, i);
			if(line == 1){
				find = 1;
				res = i;
			}
		}
		i++;
	}
	return res;
}

//calculate the width of the block
int search_width(SDL_Surface* image_surface, int width_min, int width_max, int y){
	int res = -1;
	int find = 0;
	int i = width_min;
	while(i < width_max && find == 0){
		Uint32 pixel = get_pixel(image_surface, i, y);
		Uint8 r, g, b;
		SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
		if(r==255 && g == 0 && b == 0){
			int line = is_line_v(image_surface, i, y);
			if(line == 1){
				find = 1;
				res = i;
			}
		}
		i++;
	}
	return res;
}

//draw green lines as many times as possible
void cut_all(SDL_Surface* image_surface, int width, int height, int space){
	int all_nothing = 0;
	int new_height = 0;
	int new_width = 0;
	
	while(all_nothing != 1){
		all_nothing = 1;
		
		for(int pos_x = 0; pos_x < width; pos_x++){
			for(int pos_y = 0; pos_y < height; pos_y++){
				Uint32 pixel = get_pixel(image_surface, pos_x, pos_y);
				Uint8 r, g, b;
				SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
				if(r==255 && g == 0 && b == 0){
					new_height = search_height(image_surface, pos_y+1, height, pos_x);
					new_width = search_width(image_surface, pos_x+1, width, pos_y);
					if(new_height != -1 && new_width != -1){
						int nothing = cut_bloc(image_surface, pos_x, new_width, pos_y, new_height, space);
						if(nothing == 0){
							all_nothing = 0;
						}
					}
				}
			}
		}
	}
}

//draw a green line on the horizontal border
void init_border_h(SDL_Surface* image_surface, int y, int width){
	for(int x = 0; x < width; x++){
		Uint32 pixel = get_pixel(image_surface, x, y);
		Uint8 r, g, b;
		SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
		if((r == 35 && g == 255 && b == 0) || (r == 255 && g == 0 && b == 0)){
			r = 255;
			g = 0;
			b = 0;
		}
		else{
			r = 35;
			g = 255;
			b = 0;
		}
		pixel = SDL_MapRGB(image_surface->format, r, g, b);
		put_pixel(image_surface, x, y, pixel);
	}
}

//draw a green line on the vertical border
void init_border_v(SDL_Surface* image_surface, int x,  int height){
	for(int y = 0; y < height; y++){
		Uint32 pixel = get_pixel(image_surface, x, y);
		Uint8 r, g, b;
		SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
		if((r == 35 && g == 255 && b == 0) || (r == 255 && g == 0 && b == 0)){
			r = 255;
			g = 0;
			b = 0;
		}
		else{
			r = 35;
			g = 255;
			b = 0;
		}
		pixel = SDL_MapRGB(image_surface->format, r, g, b);
		put_pixel(image_surface, x, y, pixel);
	}
}

//check if the block is only white
int bloc_is_white(SDL_Surface* image_surface, int x_min, int x_max, int y_min, int y_max){
	int res = 1;
	for(int pos_y = y_min; pos_y < y_max; pos_y++){
		for(int pos_x = x_min; pos_x < x_max; pos_x++){
			Uint32 pixel = get_pixel(image_surface, pos_x, pos_y);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			if(r==0 && g==0 && b==0){
				res = 0;
			}
		}
	}
	return res;
}

//count the number of blocks
int count_bloc(SDL_Surface* image_surface, int width, int height){
	int res = 0;
	int new_height = 0;
	int new_width = 0;
	for(int pos_y = 0; pos_y < height; pos_y++){
		for(int pos_x = 0; pos_x < width; pos_x++){
			Uint32 pixel = get_pixel(image_surface, pos_x, pos_y);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			if(r==255 && g == 0 && b == 0){
				new_height = search_height(image_surface, pos_y+1, height, pos_x);
				new_width = search_width(image_surface, pos_x+1, width, pos_y);
				if(new_height != -1 && new_width != -1){
					int is_not_bloc = bloc_is_white(image_surface, pos_x, new_width, pos_y, new_height);
					if(is_not_bloc == 0){ 
						res += 1;
					}
				}
			}
		}
	}
	return res;
}

//create a new image
SDL_Surface* create_image(SDL_Surface* image_surface, int x_min, int x_max, int y_min, int y_max){
	SDL_Surface* image_white = load_image("white.jpg");
	SDL_Surface* new_image = image_white;
    int width = new_image->w;
    int height = new_image->h;
	
	for(int pos_x = x_min; pos_x < x_max; pos_x++){
		for(int pos_y = y_min; pos_y < y_max; pos_y++){
			Uint32 pixel = get_pixel(image_surface, pos_x, pos_y);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			if((r==255 && g == 0 && b == 0) || (r==35 && g == 255 && b == 0)){
				r = 255;
				g = 255;
				b = 255;
			}
			pixel = SDL_MapRGB(image_surface->format, r, g, b);
			if(pos_x-x_min < width && pos_y-y_min < height){
				put_pixel(new_image, pos_x-x_min, pos_y-y_min, pixel);
			}
		}
	}
	return new_image;
}

//create a list containing the images of the blocks
void bloc_list(SDL_Surface* image_surface, int width, int height, SDL_Surface** list){
	int pos = 0;
	int new_height = 0;
	int new_width = 0;
	for(int pos_x = 0; pos_x < width; pos_x++){
		for(int pos_y = 0; pos_y < height; pos_y++){
			Uint32 pixel = get_pixel(image_surface, pos_x, pos_y);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			if(r==255 && g == 0 && b == 0){
				new_height = search_height(image_surface, pos_y+1, height, pos_x);
				new_width = search_width(image_surface, pos_x+1, width, pos_y);
				if(new_height != -1 && new_width != -1){
					int is_not_bloc = bloc_is_white(image_surface, pos_x, new_width, pos_y, new_height);
					if(is_not_bloc == 0){ 
						SDL_Surface* new_bloc = create_image(image_surface, pos_x, new_width, pos_y, new_height);
						list[pos] = new_bloc;
						pos += 1;
					}
				}
			}
		}
	}
}

//lines

//draw a green line horizontally for text lines
void cut_line_horizontal(SDL_Surface* image_surface, int x_min, int x_max, int y){
	for(int i = x_min; i <= x_max; i++){
		Uint32 pixel = get_pixel(image_surface, i, y);
		Uint8 r, g, b;
		SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
		r = 35;
		g = 255;
		b = 0;
		pixel = SDL_MapRGB(image_surface->format, r, g, b);
		put_pixel(image_surface, i, y, pixel);
	}
}

//check if the horizontal line is white
int is_white_line_horizontal(SDL_Surface* image_surface, int x_min, int x_max, int y){
	int res = 1;
	int i = x_min;
	while(i < x_max && res == 1){
		Uint32 pixel = get_pixel(image_surface, i, y);
		Uint8 r, g, b;
		SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
		if(r==0 && g==0 && b==0){
			res = 0;
		}
		i++;
	}
	return res;
}

//draw green lines for text lines
int cut_line(SDL_Surface* image_line, int width, int height){
	int res = 0;
	int previous = 1;
    
	for(int y = 0; y < height; y++){
		int bool = is_white_line_horizontal(image_line, 0, width, y);
		if(bool == 1){
			if(previous == 0){
				cut_line_horizontal(image_line, 0, width, y);
				res += 1;
				previous = 1;
			}
		}
		else{
			previous = 0;
		}
	}
	return res;
}

//create a list containing the images of lines
void line_list(SDL_Surface* image_line, int width, int height, SDL_Surface** list, int size){
	int pos = 0;
	int previous_height = 0;
	for(int pos_y = 0; pos_y < height; pos_y++){
		Uint32 pixel = get_pixel(image_line, 1, pos_y);
		Uint8 r, g, b;
		SDL_GetRGB(pixel, image_line->format, &r, &g, &b);
		if(r==35 && g == 255 && b == 0){
			if(pos < size){ 
				SDL_Surface* new_bloc = create_image(image_line, 0, width, previous_height, pos_y);
				previous_height = pos_y;
				list[pos] = new_bloc;
				pos += 1;
			}
		}
	}
}

//characters

//create a new image for character
SDL_Surface* create_char(SDL_Surface* image_surface, int x_min, int x_max, int y_min, int y_max){ //HERE
	SDL_Surface* image_white = load_image("white.jpg");
	SDL_Surface* new_image = image_white;
	new_image->w = 60;
    new_image->h = 60;
	
	for(int pos_y = y_min; pos_y < y_max; pos_y++){ 
		for(int pos_x = x_min; pos_x <= x_max; pos_x++){
			Uint32 pixel = get_pixel(image_surface, pos_x, pos_y);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			pixel = SDL_MapRGB(image_surface->format, r, g, b);
			put_pixel(new_image, pos_x-x_min, pos_y-y_min, pixel);
		}
	}
	return new_image;
}

//draw a green line vertically for characters
void cut_char_vertical(SDL_Surface* image_surface, int y_min, int y_max, int x){
	for(int i = y_min; i < y_max; i++){
		Uint32 pixel = get_pixel(image_surface, x, i);
		Uint8 r, g, b;
		SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
		r = 35;
		g = 255;
		b = 0;
		pixel = SDL_MapRGB(image_surface->format, r, g, b);
		put_pixel(image_surface, x, i, pixel);
	}
}

//check if the vertical line is white for characters
int is_white_char_vertical(SDL_Surface* image_surface, int y_min, int y_max, int x){
	int res = 1;
	int i = y_min;
	while(i < y_max && res == 1){
		Uint32 pixel = get_pixel(image_surface, x, i);
		Uint8 r, g, b;
		SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
		if(r==0 && g==0 && b==0){
			res = 0;
		}
		i++;
	}
	return res;
}

//check if the horizontal line is white for characters
int is_white_char_horizontal(SDL_Surface* image_surface, int x_min, int x_max, int y){
	int res = 1;
	int i = x_min;
	while(i < x_max && res == 1){
		Uint32 pixel = get_pixel(image_surface, i, y);
		Uint8 r, g, b;
		SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
		if(r==0 && g==0 && b==0){
			res = 0;
		}
		i++;
	}
	return res;
}

//remove vertical margins
SDL_Surface* marge_less(SDL_Surface* image_surface, int width, int height){
	int first = -1;
	int second = -1;
	int x1 = 0;
	int x2 = width;
	while(x1 < width && first == -1){
		int is_black = is_white_char_vertical(image_surface, 0, height, x1);
		if(is_black == 0){
			first = x1;	
		}
		x1++;
	}
	while(x2 > 0 && second == -1){
		int is_black = is_white_char_vertical(image_surface, 0, height, x2-1);
		if(is_black == 0){
			second = x2;	
		}
		x2--;
	}	
	
	SDL_Surface* image_white = load_image("white.jpg");
	SDL_Surface* new_image = image_white;
	new_image->w = second-first +2;
    new_image->h = height;
	for(int pos_y = 0; pos_y < height; pos_y++){ 
		for(int pos_x = first; pos_x < second; pos_x++){ 
			Uint32 pixel = get_pixel(image_surface, pos_x, pos_y);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			pixel = SDL_MapRGB(image_surface->format, r, g, b);
			put_pixel(new_image, pos_x-first+1, pos_y, pixel);
		}
	}
	return new_image;
}

//remove horizontal margins
SDL_Surface* marge_less2(SDL_Surface* image_surface, int width, int height){
	int first = -1;
	int second = -1;
	int y1 = 0;
	int y2 = height;
	while(y1 < height && first == -1){
		int is_black = is_white_char_horizontal(image_surface, 0, width, y1);
		if(is_black == 0){
			first = y1;	
		}
		y1++;
	}
	while(y2 > 0 && second == -1){
		int is_black = is_white_char_horizontal(image_surface, 0, width, y2);
		if(is_black == 0){
			second = y2;	
		}
		y2--;
	}	
	
	SDL_Surface* image_white = load_image("white.jpg");
	SDL_Surface* new_image = image_white;
	new_image->w = width;
    new_image->h = second-first +2;
	for(int pos_y = first; pos_y < second; pos_y++){ 
		for(int pos_x = 0; pos_x < width; pos_x++){ 
			Uint32 pixel = get_pixel(image_surface, pos_x, pos_y);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			pixel = SDL_MapRGB(image_surface->format, r, g, b);
			put_pixel(new_image, pos_x, pos_y-first+1, pixel);
		}
	}
	return new_image;
}

//draw green lines for characters
int cut_char(SDL_Surface* image_line, int width, int height, int space){
	int res = 0;
	int val = space;
	int previous = 1;
    
	for(int x = 0; x < width; x++){ 
		int bool = is_white_char_vertical(image_line, 0, height, x);
		if(bool == 1){
			if(previous == 0){
				cut_char_vertical(image_line, 0, height, x);
				res += 1;
				space = val;
				previous = 1;
			}
			else{
				if(space > 0){
					space -= 1;
				}
				else{
					previous = 0;
					space = val;
				}
			}
		}
		else{
			previous = 0;
		}
	}
	return res;
}

//create a list containing the images of the characters
void char_list(SDL_Surface* image_line, int width, int height, SDL_Surface** list, int size){
	int pos = 0;
	int previous_width = 0;
	for(int pos_x = 0; pos_x <= width; pos_x++){ 
		Uint32 pixel = get_pixel(image_line, pos_x, 1);
		Uint8 r, g, b;
		SDL_GetRGB(pixel, image_line->format, &r, &g, &b);
		if(r==35 && g == 255 && b == 0){
			if(pos < size){ 
				SDL_Surface* new_bloc = create_char(image_line, previous_width, pos_x+1, 0, height); 
				previous_width = pos_x;
				list[pos] = new_bloc;
				pos += 1;
			}
		}
	}
}

//turns all dark pixels to black
void black_change(SDL_Surface* image_surface, int width, int height){
	for(int y = 0; y < height; y++){ 
		for(int x = 0; x < width; x++){ 
			Uint32 pixel = get_pixel(image_surface, x, y);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			if(r < 30 && g < 30 && b < 30){
				r = 0;
				g = 0;
				b = 0;
			}
			pixel = SDL_MapRGB(image_surface->format, r, g, b);
			put_pixel(image_surface, x, y, pixel);
		}
	}
}

//compares two strings
int my_strcmp(char* str1, char* str2){
	int res = 1;
	int i = 0;
	
	while(res == 1 && (str1[i] != '\0' || str2[i] != '\0')){
		if(str1[i] == '\0' || str2[i] == '\0'){
			res = 0;
		}
		if(str1[i] != str2[i]){
			res = 0;
		}
		i++;
	}
	
	return res;
}

//gives the coordinates of the letter
int coord(SDL_Surface* image_surface, int width, int height, char* name){
	int x_min = width;
	int x_max = 0;
	int y_min = height;
	int y_max = 0;
	int pos_x = 0;
	int start_letter = 0;
	int y_max_find = 0;
	int res = 0;
	
	for(int pos_y = 0; pos_y < height; pos_y++){ 
		int find_x = 0;
		pos_x = 0;
		while(pos_x < width){
			Uint32 pixel = get_pixel(image_surface, pos_x, pos_y);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			if(r == 0 && g == 0 && b == 0){
				start_letter = 1;
				find_x = 1;
				if(pos_x < x_min){
					x_min = pos_x;
				}
			}
			pos_x++;
		}
		if(find_x == 0){
			if(start_letter == 0){
				y_min = pos_y;
			}
			else if(y_max_find == 0){
				y_max = pos_y;
				y_max_find = 1;
			}
		}
	}
	
	for(int pos_y = 0; pos_y < height; pos_y++){ 
		int find_x = 0;
		pos_x = width;
		while(pos_x >= 0 && find_x == 0){
			Uint32 pixel = get_pixel(image_surface, pos_x, pos_y);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			if(r == 0 && g == 0 && b == 0){
				find_x = 1;
				if(pos_x > x_max){
					x_max = pos_x;
				}
			}
			pos_x--;
		}
	}
	
	if(my_strcmp(name, "x_min") == 1){
		res = x_min;
	}
	else if(my_strcmp(name, "x_max") == 1){
		res = x_max;
	}
	else if(my_strcmp(name, "y_min") == 1){
		res = y_min;
	}
	else if(my_strcmp(name, "y_max") == 1){
		res = y_max;
	}
	
	return res;
}

//resizes image for characters
SDL_Surface* resize_char(SDL_Surface* image_surface, int x_min, int x_max, int y_min, int y_max){
	SDL_Surface* image_white = load_image("white.jpg");
	SDL_Surface* new_image = image_white;
	new_image->w = 28;
    new_image->h = 28;
    int mid_x = (x_min + x_max)/2 - x_min;
    int co_x = 28/2 - mid_x;
    int mid_y = (y_min + y_max)/2 - y_min;
    int co_y = 28/2 - mid_y;
	
	for(int pos_y = y_min; pos_y < y_max; pos_y++){
		for(int pos_x = x_min; pos_x < x_max; pos_x++){
			Uint32 pixel = get_pixel(image_surface, pos_x, pos_y);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			if((r==255 && g == 0 && b == 0) || (r==35 && g == 255 && b == 0)){
				r = 255;
				g = 255;
				b = 255;
			}
			pixel = SDL_MapRGB(image_surface->format, r, g, b);
			if((co_x + (pos_x-x_min)) < 28 && (co_y + (pos_y-y_min)) < 28 && (co_x + (pos_x-x_min)) > 0 && (co_y + (pos_y-y_min)) > 0){
				put_pixel(new_image, co_x + (pos_x-x_min), co_y + (pos_y-y_min), pixel);
			}
		}
	}
	return new_image;
}

//TEST
SDL_Surface* resize_char_test(SDL_Surface* image_surface, int x_min, int x_max, int y_min, int y_max, int width, int height){
	SDL_Surface* image_white = load_image("white.jpg");
	SDL_Surface* new_image = image_white;
	new_image->w = width;
    new_image->h = height;
    int width2 = new_image->w;
    int height2 = new_image->h;
    int mid_x = x_min + x_max/2 - x_min;
    int co_x = width2/2 - mid_x;
    int mid_y = (y_min + y_max)/2 - y_min;
    int co_y = height2/2 - mid_y;
	
	for(int pos_y = y_min; pos_y < y_max; pos_y++){
		for(int pos_x = x_min; pos_x < x_max; pos_x++){
			Uint32 pixel = get_pixel(image_surface, pos_x, pos_y);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			if((r==255 && g == 0 && b == 0) || (r==35 && g == 255 && b == 0)){
				r = 255;
				g = 255;
				b = 255;
			}
			pixel = SDL_MapRGB(image_surface->format, r, g, b);
			if((co_x + (pos_x-x_min)) < width2 && (co_y + (pos_y-y_min)) < height2 && (co_x + (pos_x-x_min)) > 0 && (co_y + (pos_y-y_min)) > 0){
				put_pixel(new_image, co_x + (pos_x-x_min), co_y + (pos_y-y_min), pixel);
			}
		}
	}
	return new_image;
}

//ZOOM
SDL_Surface* zoom(SDL_Surface* image_surface){
	double width = image_surface->w;
	double height = image_surface->h;
	
	SDL_Surface* new_image;
	
	double zoom_x = 28 / width;
	double zoom_y = 28 / height;
	
	new_image = rotozoomSurfaceXY(image_surface, 0, zoom_x, zoom_y, 0);
	
	return new_image;
}

//change color in white
SDL_Surface* color_white(SDL_Surface* image_surface, int width, int height){
	int start = 1;
	int finish = 0;
	for(int pos_x = 0; pos_x < width; pos_x++){
		for(int pos_y = 0; pos_y < height; pos_y++){
			Uint32 pixel = get_pixel(image_surface, pos_x, pos_y);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			if(r==0 && g == 0 && b == 0 && start == 1){
				start = 0;
			}
			int bool = is_white_char_vertical(image_surface, 0, height, pos_x);
			if(bool == 1 && start == 0){
				finish = 1;
			}
			if(r==0 && g == 0 && b == 0 && finish == 1){
				r = 255;
				g = 255;
				b = 255;
			}
			pixel = SDL_MapRGB(image_surface->format, r, g, b);
			put_pixel(image_surface, pos_x, pos_y, pixel);
		}
	}
	return image_surface;
}

//detects characters
//space block corresponds to the number of pixels that there are between two blocks of text
//space is the number of pixels a space makes
SDL_Surface** detection(SDL_Surface* image_surface, int space_block, int space){
	SDL_Surface* line_break = load_image("line_break.jpg");
    
	int width = image_surface->w;
    int height = image_surface->h;
    
    black_change(image_surface, width, height);
    
    //blocks
    
    init_border_h(image_surface, 0, width);
    init_border_v(image_surface, 0, height);
    init_border_h(image_surface, height-1, width);
    init_border_v(image_surface, width-1, height);
    
	cut_all(image_surface, width, height, space_block);
    
    int nb_bloc = count_bloc(image_surface, width, height);
	
    SDL_Surface* list_bloc[nb_bloc];
    bloc_list(image_surface, width, height, list_bloc);
    
	//lines
	
	int nb_line = 0;
	
	for(int i = 0; i < nb_bloc; i++){
		SDL_Surface* image_line = list_bloc[i];
		int width_line = image_line->w;
		int height_line = image_line->h;
		int nb_one_line = cut_line(image_line, width_line, height_line);
		SDL_Surface* list_one_line[nb_one_line];
		line_list(image_line, width_line, height_line, list_one_line, nb_one_line);
		nb_line += nb_one_line;
	}
	
	SDL_Surface* list_line[nb_line];
	int pos = 0;
	
	for(int i = 0; i < nb_bloc; i++){
		SDL_Surface* image_line = list_bloc[i];
		int width_line = image_line->w;
		int height_line = image_line->h;
		int nb_one_line = cut_line(image_line, width_line, height_line);
		SDL_Surface* list_one_line[nb_one_line];
		line_list(image_line, width_line, height_line, list_one_line, nb_one_line);
		for(int i = 0; i < nb_one_line; i++){
			if(pos < nb_line){
				list_line[pos] = list_one_line[i];
				pos += 1;
			}
		}
	}
	//characters
	
	int nb_char = 0;
	for(int i = 0; i < nb_line; i++){
		SDL_Surface* image = list_line[i];
		int width_char = image->w;
		int height_char = image->h;	
		SDL_Surface* image_char = marge_less(image, width_char, height_char);
		int width_char2 = image_char->w;
		int height_char2 = image_char->h;
		width_char2 = image_char->w;
		height_char2 = image_char->h;
		int nb_one_char = cut_char(image_char, width_char2, height_char2, space);
		SDL_Surface* list_one_char[nb_one_char];
		char_list(image_char, width_char2, height_char2, list_one_char, nb_one_char);
		nb_char += nb_one_char+1;
	}
	SDL_Surface** list_char = malloc((nb_char+1) * sizeof(SDL_Surface*));
	int L[nb_line];
	int pos2_L = 0;
	int pos2 = 0;
	
	for(int i = 0; i < nb_line; i++){
		SDL_Surface* image_char1 = list_line[i];
		int width_char1 = image_char1->w;
		int height_char1 = image_char1->h;
		SDL_Surface* image_char = marge_less(image_char1, width_char1, height_char1);
		int width_char = image_char->w;
		int height_char = image_char->h;
		int nb_one_char = cut_char(image_char, width_char, height_char, space);
		SDL_Surface* list_one_char[nb_one_char];
		char_list(image_char, width_char, height_char, list_one_char, nb_one_char);
		for(int i = 0; i < nb_one_char; i++){
			if(pos2 < nb_char){
				list_char[pos2] = list_one_char[i];
				pos2 += 1;
			}
		}
		list_char[pos2] = line_break;
		L[pos2_L] = pos2;
		pos2_L += 1;
		pos2 += 1;
	}
	
	for(int i = 0; i < nb_char; i++){
		SDL_Surface* image_one_char = list_char[i];
		int width3 = image_one_char->w;
		int height3 = image_one_char->h;
		int coord_x_min = coord(image_one_char, width3, height3, "x_min");
		int coord_x_max = coord(image_one_char, width3, height3, "x_max");
		int coord_y_min = coord(image_one_char, width3, height3, "y_min");
		int coord_y_max = coord(image_one_char, width3, height3, "y_max");
		SDL_Surface* image_one_char2 = resize_char(image_one_char, coord_x_min, coord_x_max+2, coord_y_min, coord_y_max);
		
		//SDL_Surface* image_one_char3 = zoom(image_one_char2);
		
		/*int width4 = image_one_char3->w;
		int height4 = image_one_char3->h;
		image_one_char = color_white(image_one_char3, width4, height4);*/
		list_char[i] = image_one_char2;
	}
	
	for(int i = 0; i < nb_line; i++){
		int pos = L[i];
		list_char[pos] = line_break;
	}
	list_char[nb_char] = NULL;
	    
    return list_char;
}
