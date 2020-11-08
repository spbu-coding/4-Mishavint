#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "negative.c"

#define MY_BMP_FILE_AND_VERSION_3_HEADERS_SIZE 54

typedef struct BITMAPFILEHEADER
{
    unsigned  short          type;      // We need this to identity our file
    unsigned int             size;      // Size of file
    unsigned short  ReservedFirst;      // This must be 0
    unsigned short ReservedSecond;      // This must be 0
    unsigned int        bfOffBits;      // We need this to know The position of the pixel data relative to the start of this structure

} BITMAPFILEHEADER ;

typedef struct BITMAPINFO
{
    unsigned int     bmp_verison;       // Must be 40 ( We use 3rd version )
    unsigned int     pixel_width;       // Pixels in width
    long int        pixel_height;       // Pixel in height
    unsigned short        Planes;       // Must be 1
    unsigned short bit_per_pixel;       // How many bits we use for each pixel
    unsigned int     compression;       // How we save pixels
    unsigned int      image_size;       // Size of pixels
    unsigned int   HorResolution;       // Pixels per meter horizontally
    unsigned int   VerResolution;       // Pixels per meter vertically
    unsigned int      color_used;       // The size of the color table in cells
    unsigned int  count_of_cells;       // Count of cells from beginning clr table till end
}BITMAPINFO;

typedef struct BMPImage
{
    BITMAPFILEHEADER header;
    BITMAPINFO       info;
    int *color_table;
    int *pixel_array;
} BMPImage;

unsigned short MineReadUSHORT( char *restrict buffer , unsigned int *restrict index )
{
    char temp_for_USHORT[ 2 ];
    temp_for_USHORT[ 0 ] = buffer[ *index ];
    temp_for_USHORT[ 1 ] = buffer[ *index + 1 ];
    *index += 2;

    return ( temp_for_USHORT[ 1 ] << 8u | temp_for_USHORT[ 0 ] );
}

unsigned int MineReadUINT( char *restrict buffer , unsigned int *restrict index)
{
    unsigned char temp_for_UINT[ 4 ];
    temp_for_UINT[ 0 ] = buffer[ *index ];
    temp_for_UINT[ 1 ] = buffer[ *index + 1 ];
    temp_for_UINT[ 2 ] = buffer[ *index + 2 ];
    temp_for_UINT[ 3 ] = buffer[ *index + 3 ];
    *index+=4;

    return ( temp_for_UINT[3]  << 24 | temp_for_UINT[2] << 16 | temp_for_UINT[1] << 8 | temp_for_UINT[0]);
}

void MineWriteUSHORT(unsigned short *restrict thing_to_write, char *restrict buffer ,  int  *index )
{

    buffer[ ( *index )++ ] = ( *thing_to_write & 0x00ffu ) >> 0u;
    buffer[ ( *index )++ ] = ( *thing_to_write & 0xff00u ) >> 8u;

}

void MineWriteUINT(unsigned int *restrict thing_to_write, char *restrict buffer , int  *index )
{

    buffer[ ( *index )++ ] = ( *thing_to_write & 0x000000ffu ) >> 0u ;
    buffer[ ( *index )++ ] = ( *thing_to_write & 0x0000ff00u ) >> 8u ;
    buffer[ ( *index )++ ] = ( *thing_to_write & 0x00ff0000u ) >> 16u ;
    buffer[ ( *index )++ ] = ( *thing_to_write & 0xff000000u ) >> 24u ;

}


void reverse_pixel_array( int* array , int bit_per_pixel , int width , int height )
{
    int bytes_per_pixel = bit_per_pixel / 8;
    int aligning = ( 4 - ( bytes_per_pixel * width ) % 4 ) % 4;
    int length_to_swap = sizeof( int ) * width + aligning;
    printf("\nlentght = %d" , length_to_swap);
    printf("\nlentght = %d" , length_to_swap);
    int *temp = ( int* )malloc( length_to_swap );
    for( int i = 0 ; i < height / 2 ; i++ )
    {
        memcpy( temp , &array[ width * i ] , length_to_swap );
        memcpy( &array[ width * i ], &array[ width * ( height - i - 1 ) ], length_to_swap);
        memcpy( &array[ width * ( height - i - 1 )] , temp, length_to_swap);
    }
}


int DecodeColorTable(struct  BMPImage *decoded_struct , FILE *input_file , int *restrict check_for_errors)
{
    if( decoded_struct->info.color_used == 0 )
    {
        return 0;
    }
    int *color_table = (int*)malloc( sizeof( int ) * decoded_struct->info.color_used );

    if(color_table == NULL)
    {
        fprintf(stderr , "Can not get memory");
        return -1;
    }


    if( fread(color_table , sizeof( int ) , decoded_struct->info.color_used , input_file) != decoded_struct->info.color_used)
    {
        fprintf(stderr , "Can not read color_table from file");
        free ( color_table );
        return -1;
    }

    decoded_struct->color_table = color_table;

    return 0;
}

int DecodePixelArray(struct  BMPImage *decoded_struct , FILE *input_file)
{
    int pixels_count = decoded_struct->info.pixel_height * decoded_struct->info.pixel_width;
    int *pixel_array = (int*)malloc( sizeof( int ) * pixels_count );
    if(pixel_array == NULL)
    {
        fprintf(stderr , "Can not get memory");
        return -1;
    }


    int bytes_per_pixel = decoded_struct->info.bit_per_pixel / 8;
    int aligning = ( 4 - ( bytes_per_pixel * (int)decoded_struct->info.pixel_width ) % 4 ) % 4;

    for( unsigned int i = 0 ; i < decoded_struct->info.pixel_height * decoded_struct->info.pixel_width  ; i++ )
    {
        pixel_array[i] = 0;
        if (fread(&pixel_array[i], bytes_per_pixel, 1, input_file) != 1)
        {
            fprintf(stderr, "\n\ncan not read pixel_array from BMP file\ni = %d" , i);
            free(pixel_array);
            return -1;
        }
        if ((i + 1) % decoded_struct->info.pixel_width == 0)
        {
            fseek(input_file, aligning, SEEK_CUR);
        }

    }

    if( decoded_struct->info.pixel_height < 0 )
    {
        reverse_pixel_array( pixel_array , decoded_struct->info.bit_per_pixel
                             , decoded_struct->info.pixel_width , decoded_struct->info.pixel_height );
    }

    decoded_struct->pixel_array = pixel_array;

    return 0;
}

int MakeNegative(struct BMPImage *decoded_struct  )
{

    if( decoded_struct->color_table == NULL )
    {
        for( int i = 0 ; i < decoded_struct->info.pixel_width ; i++ )
        {
            for (int j = 0; j < decoded_struct->info.pixel_height; j++)
            {
                decoded_struct->pixel_array[ j * decoded_struct->info.pixel_width + i ]
                        = ~(decoded_struct->pixel_array[ j * decoded_struct->info.pixel_width + i ]);
            }
        }
    }
    else
    {
        for ( unsigned int i = 0; i < decoded_struct->info.color_used; i++ )
        {
            decoded_struct->color_table[ i ] = ( ~decoded_struct->color_table[ i ] ) & 0x00ffffff ;
        }
    }

    return 0;
}

int  decode_input_file(FILE *input_file , int *restrict check_for_errors , BMPImage *decoded_struct) {

    char buffer[MY_BMP_FILE_AND_VERSION_3_HEADERS_SIZE];
    unsigned int index = 0;
    if (fread(buffer, sizeof(buffer), 1, input_file) != 1)
    {
        fprintf(stderr, "Can not copy from file");
        return -1;
    }
    printf("\n");

    unsigned int calculated_size;
    fseek( input_file, 0 , SEEK_END );
    calculated_size = ftell( input_file );
    fseek( input_file , MY_BMP_FILE_AND_VERSION_3_HEADERS_SIZE , SEEK_SET );

    decoded_struct->header.type = MineReadUSHORT( &buffer , &index );
    decoded_struct->header.size = MineReadUINT  ( &buffer , &index );
    if( decoded_struct->header.type != 0x4d42 )
    {
        fprintf(stderr , "This is not BMP file");
        return -2;
    }
    if( calculated_size != decoded_struct->header.size )
    {
        fprintf( stderr , "File was damaged" );
        return -2;
    }
    decoded_struct->header.ReservedFirst = MineReadUSHORT( &buffer , &index );
    if( decoded_struct->header.ReservedFirst != 0x0 )
    {
        fprintf(stderr , "BMP file was damaged" );
        return -2;
    }
    decoded_struct->header.ReservedSecond = MineReadUSHORT( &buffer , &index );
    if( decoded_struct->header.ReservedSecond != 0x0 )
    {
        fprintf(stderr , "BMP file was damaged" );
        return -2;
    }
    decoded_struct->header.bfOffBits = MineReadUINT( &buffer , &index );
    decoded_struct->info.bmp_verison = MineReadUINT( &buffer , &index );
    if(decoded_struct->info.bmp_verison != 40 )
    {
        fprintf(stderr , "Wrong BMP version, program can work only with 3rd verison" );
        return -1;
    }

    decoded_struct->info.pixel_width = MineReadUINT( &buffer , &index );
    if( decoded_struct->info.pixel_width == 0 )
    {
        fprintf(stderr , "File was damaged");
        return -2;
    }
    decoded_struct->info.pixel_height = MineReadUINT( &buffer , &index );
    if( decoded_struct->info.pixel_height == 0 ) {
        fprintf(stderr, "File was damaged");
        return -2;
    }

    decoded_struct->info.Planes = MineReadUSHORT( &buffer , &index );
    if(decoded_struct->info.Planes != 1)
    {
        fprintf(stderr , "File was damaged");
        return -2;
    }
    decoded_struct->info.bit_per_pixel = MineReadUSHORT( &buffer , &index );
    if ( decoded_struct->info.bit_per_pixel != 8 && decoded_struct->info.bit_per_pixel !=24 )
    {
        fprintf(stderr , "App can work only witch 8 or 24 bits per pixel. Also it is possible, that your file was damaged");
        return -2;
    }
    decoded_struct->info.compression   = MineReadUINT( &buffer , &index );
    if ( decoded_struct->info.compression != 0 )
    {
        fprintf( stderr , "App can work only with uncomressed files" );
        return -1;
    }
    decoded_struct->info.image_size = MineReadUINT( &buffer , &index );
    if( ( ( decoded_struct->info.pixel_height ) * ( decoded_struct->info.pixel_width ) * ( decoded_struct->info.bit_per_pixel / 8 ) )
        != decoded_struct->info.image_size )
    {
        fprintf(stderr , "File was damaged");
        return -2;
    }
    decoded_struct->info.HorResolution  =    MineReadUINT( &buffer , &index );
    decoded_struct->info.VerResolution  =    MineReadUINT( &buffer , &index );
    decoded_struct->info.color_used     =    MineReadUINT( &buffer , &index );
    decoded_struct->info.count_of_cells =    MineReadUINT( &buffer , &index );
    if ( *check_for_errors != 0 )
    {
        return *check_for_errors;
    }

    if (( *check_for_errors = DecodeColorTable(decoded_struct, input_file, check_for_errors)) != 0)
    {
        return *check_for_errors;
    }

    if (( *check_for_errors = DecodePixelArray( decoded_struct , input_file  )) != 0)
    {
        return *check_for_errors;
    }


    return 0;
}

int get_negative(int *restrict check_for_errors , struct BMPImage *decoded_struct)
{
    if (( *check_for_errors = MakeNegative( decoded_struct) ) != 0)
    {
        return *check_for_errors;
    }

    return 0;
}

int SaveImage(char *OutputName  , struct BMPImage *decoded_struct )
{
    unsigned char buffer[ MY_BMP_FILE_AND_VERSION_3_HEADERS_SIZE ];
    int index = 0;

    FILE *output_file = fopen( OutputName , "wb" );
    if( output_file == NULL )
    {
        fprintf(stderr , "Can not create file %s" , OutputName);
        return -1;
    }

    MineWriteUSHORT( &decoded_struct->header.type             , &buffer , &index );
    MineWriteUINT  ( &decoded_struct->header.size             , &buffer , &index );
    MineWriteUSHORT( &decoded_struct->header.ReservedFirst    , &buffer , &index );
    MineWriteUSHORT( &decoded_struct->header.ReservedSecond   , &buffer , &index );
    MineWriteUINT  ( &decoded_struct->header.bfOffBits        , &buffer , &index );

    MineWriteUINT  ( &decoded_struct->info.bmp_verison        , &buffer , &index );
    MineWriteUINT  ( &decoded_struct->info.pixel_width        , &buffer , &index );
    MineWriteUINT  ( &decoded_struct->info.pixel_height       , &buffer , &index );
    MineWriteUSHORT( &decoded_struct->info.Planes             , &buffer , &index );
    MineWriteUSHORT( &decoded_struct->info.bit_per_pixel      , &buffer , &index );
    MineWriteUINT  ( &decoded_struct->info.compression        , &buffer , &index );
    MineWriteUINT  ( &decoded_struct->info.image_size         , &buffer , &index );
    MineWriteUINT  ( &decoded_struct->info.HorResolution      , &buffer , &index );
    MineWriteUINT  ( &decoded_struct->info.VerResolution      , &buffer , &index );
    MineWriteUINT  ( &decoded_struct->info.color_used         , &buffer , &index );
    MineWriteUINT  ( &decoded_struct->info.count_of_cells     , &buffer , &index );

    if ( fwrite( buffer , sizeof( buffer ) , 1 , output_file ) != 1 )
    {
        fprintf( stderr , "Can not write to new file ");
        return -1;
    }


    if ( fwrite( decoded_struct->color_table , 4 , decoded_struct->info.color_used , output_file )
    != decoded_struct->info.color_used )
    {
        fprintf( stderr , "Can not write to new file ");
        return -1;
    }

    int bites_per_pixel = decoded_struct->info.bit_per_pixel / 8;
    int aligning = ( 4 - ( bites_per_pixel * (int)decoded_struct->info.pixel_width ) % 4 ) % 4;
    int temp_for_zero = 0;

    if( decoded_struct->info.pixel_height < 0  )
    {
        reverse_pixel_array( decoded_struct->pixel_array , decoded_struct->info.bit_per_pixel ,
                             decoded_struct->info.pixel_width , decoded_struct->info.pixel_height);
    }


    for( unsigned int i = 0 ; i < decoded_struct->info.pixel_width * decoded_struct->info.pixel_height ; i++ )
    {
        if ( fwrite( &decoded_struct->pixel_array[i] , bites_per_pixel , 1 , output_file ) != 1 )
        {
            fclose( output_file );
            fprintf( stderr ,  "Can not write to new file");
            return -1;
        }
        if ( ( i + 1  ) % decoded_struct->info.pixel_width == 0 && aligning > 0 )
        {
            if ( fwrite( &temp_for_zero , aligning , 1 , output_file ) != 1 )
            {
             fclose( output_file );
             fprintf( stderr , "Can not write to new file" );
             return -1;
            }
        }
    }
    fclose( output_file );
    return 0;
}

void FreeBmp( struct BMPImage *decoded_struct)
{
    free( decoded_struct->color_table );
    free( decoded_struct->pixel_array );
}

int MyImplementation( char **argv)
{

    int check_for_errors = 0;
    FILE *input_file = fopen( argv[ 2 ] , "rb");
    if(input_file == NULL)
    {
        fprintf(stderr , "Can not open file %s" , argv[ 2 ] );
        return -1;
    }

    BMPImage decoded_struct = { {} , {} , {NULL} , {NULL} };
    check_for_errors = decode_input_file(input_file , &check_for_errors , &decoded_struct);
    if(check_for_errors != 0)
    {
        FreeBmp(&decoded_struct);
        fclose(input_file);
        return check_for_errors;
    }

    check_for_errors = get_negative( &check_for_errors, &decoded_struct );
    if(check_for_errors != 0)
    {
        fclose(input_file);
        FreeBmp(&decoded_struct);
        return check_for_errors;
    }

    check_for_errors = SaveImage(argv[3] , &decoded_struct) ;

    FreeBmp( &decoded_struct );
    fclose(input_file);

    return check_for_errors;
}
