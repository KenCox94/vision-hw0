#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "image.h"


inline float* get( image* im, int x, int y, int c ) {
    return ( ( (im->data + x) + im->w * y) + im->w * im->h * c );

}

float get_pixel(image im, int x, int y, int c)
{
    if ( y < 0 || x < 0 || c < 0 ) { 
    	if( y < 0 ){
	    y = 0;
    	}
    	if (x < 0) { 
	    x = 0;
    	}
         if(c <  0){ 
            c = 0;
        }
    } else if ( x >= im.w || y >= im.h || c >= im.c ){
	if ( x >= im.w ){
	    x = im.w - 1;
	}
	if( c >= im.c ){
	   c = im.c - 1;
	}
	if ( y >= im.h ){
	    y = im.h - 1;
   	} 
    }
    return *get(&im, x, y, c);    
}

void set_pixel(image im, int x, int y, int c, float v)
{
    if( ( x < 0 || y < 0 || c < 0 ) || ( x >= im.w || y >= im.h || c >= im.c ) ) {
        return;
    }
    float* pixel = get(&im, x, y, c);
    
    *pixel = v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    copy.data = malloc(sizeof(float) * im.w * im.h * im.c);
    for ( int i = 0; i < im.h * im.w * im.c; i++ ){
        copy.data[i] = im.data[i];
    }
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    gray.data = malloc( sizeof(float) * im.w * im.h);

    for( int y = 0; y < gray.h; y++ ){
        for( int x = 0; x < gray.w; x++){
	    
	    float delta = 0; 
	    float luma_val;
	    for ( int channel = 0; channel < im.c; channel++ ){ 
		switch( channel ){
		    case 0:
			luma_val = 0.299;
			break;
		    case 1:
			luma_val = 0.587;
			break;
		    case 2:
			luma_val = 0.114;
			break;
		}
		delta += luma_val * *get(&im, x, y, channel);
	    }
	    float* pixel = ( (gray.data + x) + gray.w * y);
	    *pixel = delta; 	
	}	
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    for( int y = 0; y < im.h; y++ ){
        for( int x = 0; x < im.w; x++ ){
	    float* pixel = get(&im, x, y, c);
	    *pixel += v;
	}
    }
}

void clamp_image(image im)
{
    for( int idx = 0; idx  < im.c * im.h * im.w; idx++ ){
        float* pixel = im.data + idx;

	if( *pixel < 0 ) {
	    *pixel = 0;
	} else if( *pixel > 1) {
	    *pixel = 1;
	}
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    
    for( int y = 0; y < im.h; y++ ){
        for( int x = 0; x< im.w; x++ ){
	   float* r, *g, *b;
	   r = get(&im, x, y, 0);
	   g = get(&im, x, y, 1);
	   b = get(&im, x, y, 2);


	   float value = three_way_max( *r, *g, *b );
	   float m = three_way_min( *r, *g, *b );	
	   float saturation, chroma, hue;
	   saturation = hue = 0;
	   chroma = value - m;

	   if ( value != 0 ){
	      saturation = chroma/value; 
	   }

	   if ( value == m ){
	       hue = 0;
	   } else if ( value == *r ){
	       hue = ( *g - *b )/ chroma;
	   } else if ( value == *g ){
	       hue = ( *b - *r ) / chroma + 2;
	   } else if ( value == *b) {
	       hue = ( *r - *g ) / chroma + 4;
	   }

	   hue = hue / 6;
	   if ( hue < 0 ) hue += 1;

	   *r = hue;
	   *g = saturation;
	   *b = value;
	}
    }
}

void hsv_to_rgb(image im)
{
    for( int y = 0; y < im.h; y++ ){	
        for( int x = 0; x < im.w; x++ ){
	    
	    float* hue, *saturation, *value;
	    hue = get(&im, x, y, 0);
	    saturation = get(&im, x, y, 1);
	    value = get(&im, x, y, 2);

	   
	    float hue_prime = (*hue * 6);

	    float m = *value - *saturation * *value;
	    float* r, *g, *b;

	    r = hue;
	    g = saturation;
	    b = value;

	    float x = hue_prime - floor(hue_prime); 
	    float mid_pos, mid_neg;

	    mid_pos = *saturation * *value * x + m;
	    mid_neg = *saturation * *value * (1 - x) + m;


	    if ( hue_prime >= 0 && hue_prime < 1 ){
	        *r = *value, *g = mid_pos, *b = m;
	    } else if( hue_prime >= 1 && hue_prime < 2 ){
		*r = mid_neg, *g = *value, *b = m;
	    } else if( hue_prime >= 2 && hue_prime < 3) {
		*r = m, *g = *value, *b = mid_pos;
	    } else if ( hue_prime >= 3 && hue_prime < 4){
		*r = m, *g = mid_neg, *b = *value;
	    } else if( hue_prime >= 4 && hue_prime < 5 ){
		*r = mid_pos, *g = m, *b = *value;
	    } else{
		*r = *value, *g = m, *b = mid_neg;
	    }
	
	}
    }
}
