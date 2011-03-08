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

    MPI_Init (&argc, &argv);	/* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */
    
    /* This is a collective call, which means every process does the same thing, opens the same file */
    MPI_File_open( MPI_COMM_WORLD, "hellovirginfile", MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh );

    
    /* comment the follwing 2 lines and you'll see only the rank of the last file is in the file */
    MPI_Offset my_offset = rank;
    MPI_File_seek( fh, my_offset, MPI_SEEK_SET );
    
    /* Write using individual file pointer */
    MPI_File_read( fh, &rank_char, 1, MPI_CHAR, &status);

    printf( "I am rank %d. I read %c from the file.\n", rank, rank_char );

    MPI_File_close(&fh);

    MPI_Finalize();
    return 0;
}

