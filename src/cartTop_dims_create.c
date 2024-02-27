#include <mpi.h>
#include <stdio.h> 
#include <string.h>

/*
    Demostrates the workings of MPI_create_dims()
*/

int main(int argc, char ** argv) 
{
    MPI_Init(&argc,&argv);
    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    int dims[3] ={0};
    MPI_Dims_create(size,3,dims);
    if(rank == 0 )
    {
        printf("{%d, %d, %d}\n", dims[0],dims[1],dims[2]);
    }
    MPI_Finalize();
}