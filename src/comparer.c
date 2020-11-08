//
// Created by misha on 08.11.2020.
//
#include <stdio.h>
#include <string.h>
#include "MineRealization.h"

int check_for_errors_with_params( int *restrict number_of_params , char* params[] )
{
    if( *number_of_params != 3 )
    {
        fprintf( stderr , "Please enter 2 params: <file1_name>.bmp <file2_name>.bmp>" );
        return -1;
    }

    int temp_for_strlen_of_param = strlen( params[ 1 ] );
    if( ( temp_for_strlen_of_param <= 4 )  || ( strncmp( ( params[ 1 ] + ( strlen( params[ 1 ] ) - 4 ) ) , ".bmp" , 4) != 0 ) )
    {
        fprintf( stderr , "Please enter first parram correctly: <first_file>.bmp ");
        return -1;
    }

    temp_for_strlen_of_param = strlen( params[ 2 ] );
    if( (  temp_for_strlen_of_param <= 4 ) || ( strncmp( (params[ 2 ] + (strlen( params[ 2 ] ) - 4)) , ".bmp" , 4) != 0 ) )
    {
        fprintf( stderr , "Please enter second parram correctly: <second_file>.bmp ");
        return -1;
    }

    return 0;
}

int main(int argc , char **argv)
{

    int check_for_errors =  check_for_errors_with_params( &argc , argv );
    if( check_for_errors != 0 )
    {
        return check_for_errors;
    }

    FILE *first_file , *second_file;
    if((first_file = fopen(argv[ 1 ] , "rb" ) ) == NULL )
    {
        fprintf( stderr , "Can not open %s file" , argv[1] );
        return -1;
    }
    if((second_file = fopen(argv[ 2 ] , "rb" ) ) == NULL )
    {
        fprintf( stderr , "Can not open %s file" , argv[2] );
        return -1;
    }

    BMPImage first_decoded_struct = { {} , {} , {NULL} , {NULL} } ;
    BMPImage second_decoded_struct = { {} , {} , {NULL} , {NULL} } ;

    check_for_errors = decode_input_file( first_file , &check_for_errors  , &first_decoded_struct );
    if( check_for_errors != 0 )
    {
        FreeBmp( &first_decoded_struct );
        return check_for_errors;
    }

    check_for_errors = decode_input_file( second_file , &check_for_errors , &second_decoded_struct );
    if( check_for_errors != 0 )
    {
        FreeBmp( &second_decoded_struct );
        return check_for_errors;
    }

    if( first_decoded_struct.info.bit_per_pixel != second_decoded_struct.info.bit_per_pixel )
    {
        fprintf( stderr , "can not make compare between 8 bits_per_pixel and 24 bites_per_pixel" );
        return -1;
    }
    if( first_decoded_struct.info.pixel_height != second_decoded_struct.info.pixel_height )
    {
        fprintf(stderr , "First file's height not as second file's one " );
        return -1;
    }
    if( first_decoded_struct.info.pixel_width != second_decoded_struct.info.pixel_width )
    {
        fprintf(stderr , "First file's width not as second file's one " );
        return -1;
    }

    int array_of_pixels[ 100 ]  , array_of_pixels2[ 100 ] , mask , count = 0 ;

    if( first_decoded_struct.info.bit_per_pixel == 8 )
    {
        mask = 0x000000ff;
    }
    else
    {
        mask = 0x00ffffff;
    }

//    for( int i = 0 ; i < 10 ; i++ )
//    {
//        printf("[%d] = %d ; %d\n" , i , first_decoded_struct.pixel_array[i] , second_decoded_struct.pixel_array[i] );
//    }


for (int i = 0; i < first_decoded_struct.info.pixel_width; i++) {
    for (int j = 0; j < first_decoded_struct.info.pixel_height; j++) {
        if ((first_decoded_struct.pixel_array[j * first_decoded_struct.info.pixel_width + i] & mask)
        != (second_decoded_struct.pixel_array[j * first_decoded_struct.info.pixel_width + i] & mask))
        {
            array_of_pixels[count] = i ;
            array_of_pixels2[count++] = j;
        }

        if (count == 100)
        {
            j = first_decoded_struct.info.pixel_height;                 // Did this to
            i = (int) first_decoded_struct.info.pixel_width;            // Exit from "for"
        }
    }
}



    for( int i = 0 ; i < count ; i++)
    {
        fprintf( stderr , "x:%d y:%d\n" , array_of_pixels[ i ] , array_of_pixels2[ i ] );
    }


    FreeBmp( &first_decoded_struct );
    FreeBmp( &second_decoded_struct );
    fclose(first_file );
    fclose(second_file );
    return 0;
}

