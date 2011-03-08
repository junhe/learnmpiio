/*
 * to compile: mpicc me.c -o me
 * to run: mpirun -n 4 ./me
 */

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define EL_SIZE 1024
#define X_SIZE  100
#define Y_SIZE  100


int rank, size;

int main (argc, argv)
    int argc;
    char *argv[];
{
    MPI_File fh;
    MPI_Status status;
    char rank_char;
    char *junk;
    int i,j;
    MPI_Datatype sb_arr;

    MPI_Init (&argc, &argv);	/* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */

    /* This is a collective call, which means every process does the same thing, opens the same file */
    MPI_File_open( MPI_COMM_WORLD, "hellovirginfile2", MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh );
    

    /* create big element */
    MPI_Datatype el_type;
    MPI_Type_contiguous(EL_SIZE, MPI_CHAR, &el_type);
    MPI_Type_commit(&el_type);

    /* create filetype */
    int sizes[2] = {X_SIZE,Y_SIZE};
    int subsizes[2] = {1,1};
    int starts[2] = {0,0};
    MPI_Type_create_subarray( 2, sizes, subsizes, starts, MPI_ORDER_C, el_type, &sb_arr );
    MPI_Type_commit(&sb_arr);

    
    /* write to file */
    if ( rank == 0 && argc > 1 && (strcmp(argv[1], "createfile") == 0) ) {
        printf( "Create file...\n" );
        junk = malloc ( EL_SIZE );
        for ( i = 0 ; i < EL_SIZE ; i++ ) {
            junk[i] = (char) i;
        }
        int elcnt =  X_SIZE * Y_SIZE;
        for ( i = 0 ; i < elcnt ; i++ ) {
            MPI_File_write( fh, junk, EL_SIZE, MPI_CHAR, &status);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
   

    MPI_File_set_view(fh, 0, MPI_CHAR, sb_arr, "native", MPI_INFO_NULL);
    
    MPI_File_read( fh, &rank_char, 1, MPI_CHAR, &status);

    printf( "I am rank %d. I read %c from the file.\n", rank, rank_char );

    MPI_File_close(&fh);

    MPI_Finalize();
    return 0;
}

