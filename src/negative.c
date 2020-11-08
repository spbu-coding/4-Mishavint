/**************************************************************

	The program reads an BMP image file and creates a new
	image that is the negative of the input file.

**************************************************************/

#include "qdbmp.h"
#include <stdio.h>

/* Creates a negative image of the input bitmap file */
int qdnmp( int argc, char* argv[] )
{
	UCHAR	r, g, b;
	UINT	width, height;
	UINT	x, y;
	BMP*	bmp;

	/* Check arguments */
	if ( argc != 3 )
	{
		fprintf( stderr, "Usage: %s <input file> <output file>\n", argv[ 0 ] );
		return -3;
	}

	/* Read an image file */
	bmp = BMP_ReadFile( argv[ 2 ] );
	BMP_CHECK_ERROR( stdout, -3 );

	/* Get image's dimensions */
	width = BMP_GetWidth( bmp );
	height = BMP_GetHeight( bmp );

	/* Iterate through all the image's pixels */
	for ( x = 0 ; x < width ; ++x )
	{
		for ( y = 0 ; y < height ; ++y )
		{
			/* Get pixel's RGB values */
			BMP_GetPixelRGB( bmp, x, y, &r, &g, &b );

			/* Invert RGB values */
			BMP_SetPixelRGB( bmp, x, y, 255 - r, 255 - g, 255 - b );
		}
	}

	// added line, if bbp == 8 ( mine code )
	if( bmp->Header.BitsPerPixel == 8 )
    {
	    for( long long int i = 0 ; i < BMP_PALETTE_SIZE_8bpp ; i++ )
        {
	        if( ( i + 1 ) %4 != 0 )
            {
	            bmp->Palette[ i ] = ~bmp->Palette[ i ];
            }
        }
    }

	/* Save result */
	BMP_WriteFile( bmp, argv[ 3 ] );
	BMP_CHECK_ERROR( stdout, -3 );


	/* Free all memory allocated for the image */
	BMP_Free( bmp );

	return 0;
}

