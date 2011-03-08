/*
 * to compile: mpicc me.c -o me
 * to run: mpirun -n 4 ./me
 */

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define BLOCK_CNT   2
#define BLOCK_LEN   6
#define STRIDE_LEN  10

int rank, size;

int main (argc, argv)
    int argc;
    char *argv[];
{
    MPI_File fh;
    MPI_Status status;
    char rank_char[128];
    char *junk;
    int i,j;
    MPI_Datatype vec_type;

    MPI_Init (&argc, &argv);	/* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */

    /* This is a collective call, which means every process does the same thing, opens the same file */
    MPI_File_open( MPI_COMM_WORLD, "hellovirginfile2", MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh );
    

    MPI_Type_vector( BLOCK_CNT, BLOCK_LEN, STRIDE_LEN, MPI_CHAR, &vec_type );
    MPI_Type_commit(&vec_type);

    
    /* write to file */
    if ( rank == 0 && argc > 1 && (strcmp(argv[1], "createfile") == 0) ) {
        printf( "Create file...\n" );
        junk = malloc ( STRIDE_LEN  );
        for ( i = 0 ; i < STRIDE_LEN ; i++ ) {
            junk[i] = (char) i + '0';
        }
        for ( i = 0 ; i < BLOCK_CNT ; i++ ) {
            MPI_File_write( fh, junk, STRIDE_LEN, MPI_CHAR, &status);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
   

    MPI_File_set_view(fh, 0, MPI_CHAR, vec_type, "native", MPI_INFO_NULL);
    
    MPI_File_read( fh, rank_char, 8, MPI_CHAR, &status);
    rank_char[8] = '\0';
    printf( "I am rank %d. I read %s from the file.\n", rank, rank_char );

    MPI_File_close(&fh);

    MPI_Finalize();
    return 0;
}

