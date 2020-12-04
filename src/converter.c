#include <stdio.h>
#include <string.h>
#include "MineRealization.h"



int check_for_params(int *restrict number_of_params , char *restrict params[] , unsigned  char *mine_or_theirs)
{
    if (*number_of_params < 4) {
        fprintf(stderr, "You have given less than 4 params, please enter valid params");
        return -1;
    }
    if (*number_of_params > 4) {
        fprintf(stderr, "You have more than 4 params, please enter valid params");
        return -1;
    }

    if ((strncmp(params[ 1 ], "--mine", 6) == 0) && (strlen(params[ 1 ]) == 6))
    {
        *mine_or_theirs = 1;
    }
    else if ((strncmp(params[ 1 ], "--theirs", 8) == 0) && (strlen(params[ 1 ]) == 8) )
    {
        *mine_or_theirs = 2;
    }


    if (*mine_or_theirs == 0)
    {
        fprintf(stderr, "PLease, enter --mine or --theirs in correct form");
        return -1;
    }

    int temp_for_strlen_of_param = (int)strlen( params[ 2 ] );
    if( ( temp_for_strlen_of_param <= 4 )  || ( strncmp( ( params[ 2 ] + ( strlen( params[ 2 ] ) - 4 ) ) , ".bmp" , 4) != 0 ) )
    {
        fprintf( stderr , "Please enter first parram correctly: <first_file>.bmp ");
        return -1;
    }

    temp_for_strlen_of_param = (int)strlen( params[ 3 ] );
    if( (  temp_for_strlen_of_param <= 4 ) || ( strncmp( (params[ 3 ] + (strlen( params[ 3 ] ) - 4)) , ".bmp" , 4) != 0 ) )
    {
        fprintf( stderr , "Please enter second parram correctly: <second_file>.bmp ");
        return -1;
    }


    return 0;
}

int main(int argc , char** argv)
{
    unsigned char mine_or_theirs = 0;         /* If users wants mine code then mine_or_theirs will be "1". If user wants their code then "2" */
    int check_for_errors_with_params = check_for_params(&argc , argv , &mine_or_theirs);  // Function works with mine_or_theirs. If user wants mine realisation, function will do it = 1, in other way = 2
    if(check_for_errors_with_params != 0)
    {
        return check_for_errors_with_params;
    }

    if( mine_or_theirs == 1 )
    {
        int check_for_errors = MyImplementation( argv );

        return check_for_errors;
    }
    else if ( mine_or_theirs == 2 )
    {
        int check_for_errors;

        check_for_errors = qdnmp(argc-1 , argv);       // -1 since we use --mine or --theirs

        if( check_for_errors != 0 )
        {
            return -3;
        }
        return 0;
    }

    fprintf( stderr , "Programm can not work in correct way" );
    return 0;
}
