/*
 * to compile: mpicc me.c -o me
 * to run: mpirun -n 4 ./me
 */

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define BLOCK_CNT   1024
#define BLOCK_LEN   1024
#define STRIDE_LEN  (4096)

int rank, size;

int main (argc, argv)
    int argc;
    char *argv[];
{
    MPI_File fh;
    MPI_Status status;
    char *junk;
    int i,j;
    int ret;
    MPI_Datatype vec_type;

    MPI_Init (&argc, &argv);	/* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */

    /* This is a collective call, which means every process does the same thing, opens the same file */
    ret = MPI_File_open( MPI_COMM_WORLD, "/tmp/hellovirginfile2", MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh );
    if ( ret != MPI_SUCCESS ) {
        printf("Failed to open file");
        goto final;
    }
    

    MPI_Type_vector( BLOCK_CNT, BLOCK_LEN, STRIDE_LEN, MPI_CHAR, &vec_type );
    MPI_Type_commit(&vec_type);

    
    /* write to file */
    if ( argc > 1 && (strcmp(argv[1], "createfile") == 0) ) {
        printf( "Create file...\n" );
        junk = malloc ( STRIDE_LEN  );
        for ( i = 0 ; i < STRIDE_LEN ; i++ ) {
            junk[i] = (char) i + '0';
        }
        for ( i = 0 ; i < BLOCK_CNT ; i++ ) {
            ret = MPI_File_write( fh, junk, STRIDE_LEN, MPI_CHAR, &status);
            if ( ret != MPI_SUCCESS ) {
                printf("failed to write file\n");
                goto final;
            }
        }
        free(junk);
    } else {
        printf("read file only\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);
   

    MPI_File_set_view(fh, 0, MPI_CHAR, vec_type, "native", MPI_INFO_NULL);
    
    
    long readsize = BLOCK_CNT*BLOCK_LEN;
    char *buffer = malloc (readsize);
    if (buffer == NULL) {
        printf("Can't allocate memory for buffer.\n");
        goto final;
    }

    
    double start = MPI_Wtime();
    MPI_File_read( fh, buffer, readsize, MPI_CHAR, &status);
    double end = MPI_Wtime();
    double total = end - start;

    if ( ret != MPI_SUCCESS ) {
        printf("File read failed\n");
        goto final;
    } else {
        printf( "Read success. Bandwidth:%lf\n", readsize/(total*1024*1024) );
    }


    MPI_File_close(&fh);
final:
    MPI_Finalize();
    return 0;
}

