/*
 * to compile: mpicc me.c -o me
 * to run: mpirun -n 4 ./me
 */

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define ROW_S 8
#define COL_S 2

int rank, size;

int main (argc, argv)
    int argc;
    char *argv[];
{
    MPI_File fh;
    MPI_Status status;
    char rank_char[128];
    char junk[ROW_S][COL_S] = {'a','b','c','d','e','f','g','h','i', 'j', 'k', 'l', 'm', 'n', 'o','p'};
    int i,j;
    MPI_Datatype sb_arr;

    MPI_Init (&argc, &argv);	/* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */

    /* This is a collective call, which means every process does the same thing, opens the same file */
    MPI_File_open( MPI_COMM_WORLD, "hellovirginfile2", MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh );
    
    
    /* print out the array and write to file */
    if ( rank == 0 ) {
        for ( i = 0; i < ROW_S ; i++ ) {
            for ( j = 0; j < COL_S; j++ ) {
                printf("%c ", junk[i][j]);
                MPI_File_write( fh, "0", 1, MPI_CHAR, &status);
            }
            printf("\n");
        }
        
        //MPI_File_write( fh, junk, 16, MPI_CHAR, &status);
    }

    MPI_Barrier(MPI_COMM_WORLD);


    int sizes[2] = {ROW_S,COL_S}; // big array has sizes[0] rows and size[1] column
    int subsizes[2] = {2,2};
    int starts[2] = {0,0};
    MPI_Type_create_subarray( 2, sizes, subsizes, starts, MPI_ORDER_C, MPI_CHAR, &sb_arr );
    MPI_Type_commit(&sb_arr);

    
    MPI_File_set_view(fh, 0, MPI_CHAR, sb_arr, "native", MPI_INFO_NULL);
    
    //MPI_File_read( fh, rank_char, 4, MPI_CHAR, &status);
    rank_char[4] = '\0';
    rank_char[0]='1';
    rank_char[1]='2';
    rank_char[2]='3';
    rank_char[3]='4';
    MPI_File_write( fh, rank_char, 4, MPI_CHAR, &status);

    //printf( "I am rank %d. I read %s from the file.\n", rank, rank_char );

    MPI_File_close(&fh);

    MPI_Finalize();
    return 0;
}

