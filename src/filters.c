#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <stdlib.h>
#include "filters.h"
#include "Display.h"
#include "pixel_operations.h"


//#########################MATRICE FONCTIONS##########################

int sumcoef(double m1[],size_t rows, size_t cols, int res)
{
    
    for(size_t i = 0; i < cols; i++)
    {
	    for(size_t j = 0; j < rows;j++)
	    {
                res += m1[i * cols + j];
	    }
    }
    return res;
    
}

int addi(double m2[], double S[], double R[], size_t rows, size_t cols,int res, int xlevrai,int y,SDL_Surface* image_surface)
{
    Uint8 r, g, b;
    int initx = xlevrai;
    for(size_t i = 0; i < cols; i++)
    {       
            xlevrai = initx;
	    for(size_t j = 0; j < rows;j++)
	    {
                Uint32 pixel = get_pixel(image_surface, xlevrai-1, y-1);
		SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            	R[i * cols + j] = r;
                xlevrai += 1;

	    }
            y += 1;
      
    }
    for(size_t i = 0; i < cols; i++)
    {
	    for(size_t j = 0; j < rows;j++)
	    {
		S[i*cols+j] = R[i*cols+j] * m2[i*cols+j];
		

	    }
    }

    
    
    for(size_t i = 0; i < cols; i++)
    {
	    for(size_t j = 0; j < rows;j++)
	    {
            	res += S[i * cols + j];
	    }
    }
    return res;
    
}



//############CONVOLUTION MATRIX############




#define ROWS 3
#define COLS 3
double Contrast[] =
{
    0, -1, 0,
    -1, 5, -1,
    0, -1, 0,
};


#define ROWS 3
#define COLS 3
double Floue[] =
{
    1, 1, 1,
    1, 5, 1,
    1, 1, 1,
};


double Sum[ROWS*COLS];
double pix[ROWS*COLS];

//###################GRAYSCALE########################

SDL_Surface* grayscale(SDL_Surface* image)
{
    Uint8 r, g, b;
    //SDL_Surface* res;
    //init_sdl();
    //res = display_image(origin);
    //wait_for_keypressed();
    int width = image->w;
    int height = image->h;
    for(int x = 0;x < width;x++)
    {
        for(int y = 0;y < height;y++)
        {
            Uint32 pixel = get_pixel(image, x, y);
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            int average = 0.3*r + 0.59*g + 0.11*b;
            r = average;
            g = average;
            b = average;
            pixel = SDL_MapRGB(image->format, r, g, b);
            put_pixel(image, x, y, pixel);

        }
    }
    //update_surface(screen_surface, image_surface);
    //SDL_FreeSurface(image_surface);
    //SDL_FreeSurface(screen_surface);
    return image;

}


//###############################BLACK AND WHITE IMAGE#####################


SDL_Surface* black_and_white(SDL_Surface* image)
{
    Uint8 r, g, b;
   
    int width = image->w;
    int height = image->h;
    int limite = 100;
    for(int x = 0;x < width;x++)
    {
        for(int y = 0;y < height;y++)
        {
            Uint32 pixel = get_pixel(image, x, y);
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            int gray = r;
	    if(gray < limite)
            {
	    	gray = 0;
            }
	    else
		gray = 255;
            pixel = SDL_MapRGB(image->format, gray, gray, gray);
            put_pixel(image, x, y, pixel);

        }
    }
    return image;

}	




//################################INCREASING CONTRAST IMAGE#############################
	
SDL_Surface* contrast(SDL_Surface* image)
{
    int res = 0;
    int coeff = 0;
    int contrast = 0;
    SDL_Surface* surface;
    int width = image->w;
    int height = image->h;
    surface = SDL_CreateRGBSurface(0,width,height,32,0,0,0,0);
    for(int y = 1;y < height-1;y++)
    {
	 for(int x = 1;x < width-1;x++)
	 {
		res = 0;
		contrast = 0;
                coeff = 0;
                res = addi(Contrast,pix,Sum,ROWS,COLS,res,x,y,image);
		coeff = sumcoef(Contrast,ROWS,COLS,coeff);
		if( res < 0)
                {
			res = 0;
		}
		if( res > 255)
                {
			res = 255;
		}
                
		contrast = res/coeff;
		Uint32 pixel = SDL_MapRGB(image->format, contrast, contrast, contrast);
		put_pixel(surface, x, y, pixel);
		
	 }
    }
    return surface;
}

//################################INCREASING OUT OF FOCUS IMAGE#############################

SDL_Surface* floue(SDL_Surface* image)
{
    int res = 0;
    int coeff = 0;
    int floue = 0;
    //SDL_Surface* screen_surface;
    SDL_Surface* surface;
    //screen_surface = display_image(image_surface);
    int width = image->w;
    int height = image->h;
    surface = SDL_CreateRGBSurface(0,width,height,32,0,0,0,0);
    wait_for_keypressed();
    for(int y = 1;y < height-1;y++)
    {
	 for(int x = 1;x < width-1;x++)
	 {
		res = 0;
		floue = 0;
                coeff = 0;
                res = addi(Floue,pix,Sum,ROWS,COLS,res,x,y,image);
		coeff = sumcoef(Floue,ROWS,COLS,coeff);
		floue = res/coeff;
		Uint32 pixel = SDL_MapRGB(image->format, floue, floue, floue);
		put_pixel(surface, x, y, pixel);
	 }
    }
    return surface;
}

