/*
 * to compile: mpicc me.c -o me
 * to run: mpirun -n 4 ./me
 */

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"


int rank, size;

int main (argc, argv)
    int argc;
    char *argv[];
{
    MPI_File fh;
    MPI_Status status;
    char rank_char;
    char junk[3][3] = {'0','1','2','3','4','5','6','7','8'};
    int i,j;
    MPI_Datatype sb_arr;

    MPI_Init (&argc, &argv);	/* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */

    /* This is a collective call, which means every process does the same thing, opens the same file */
    MPI_File_open( MPI_COMM_WORLD, "hellovirginfile2", MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh );
    
    
    /* print out the array and write to file */
    if ( rank == 0 ) {
        for ( i = 0; i < 3 ; i++ ) {
            for ( j = 0; j < 3; j++ ) {
                printf("%c ", junk[i][j]);
            }
            printf("\n");
        }
        
        MPI_File_write( fh, junk, 9, MPI_CHAR, &status);
    }

    MPI_Barrier(MPI_COMM_WORLD);


    int sizes[2] = {3,3};
    int subsizes[2] = {1,1};
    int starts[2] = {rank,rank};
    MPI_Type_create_subarray( 2, sizes, subsizes, starts, MPI_ORDER_C, MPI_CHAR, &sb_arr );
    MPI_Type_commit(&sb_arr);

    
    MPI_File_set_view(fh, 0, MPI_CHAR, sb_arr, "native", MPI_INFO_NULL);
    
    MPI_File_read( fh, &rank_char, 1, MPI_CHAR, &status);

    printf( "I am rank %d. I read %c from the file.\n", rank, rank_char );

    MPI_File_close(&fh);

    MPI_Finalize();
    return 0;
}

