#include <mpi.h>
#include <stdio.h> 
#include <string.h>

/*
    Demonstrate how to send lower triangle of a matrix
*/

int main(int argc, char ** argv) 
{
    MPI_Init(&argc,&argv);
    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);


    MPI_Finalize();
}