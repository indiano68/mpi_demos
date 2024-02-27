#include <mpi.h>
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

/*
    Demostrates use of cartesian topology to make a torus topology
*/
void print_matrix(int * array, int * dims)
{
    for(size_t index = 0; index< dims[0]*dims[1]; index ++)
    {
        printf(" %02d ",array[index]);
        if(index!= 0 && (index+1)%dims[1] == 0 ) printf("\n");
    }
}

int main(int argc, char ** argv) 
{
    MPI_Init(&argc,&argv);
    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    int * matrix;
    int dims[2] ={0};
    MPI_Dims_create(size,2,dims);

    MPI_Comm cartesian;
    MPI_Cart_create(MPI_COMM_WORLD,2,dims,(int[2]){1,1},0,&cartesian);
    if(rank == 0 )
    {
        matrix = calloc(dims[0]*dims[1],sizeof(int));
        printf("{%d, %d}\n", dims[0],dims[1]);
    }
    MPI_Gather(&rank,1,MPI_INT,matrix,1,MPI_INT,0,cartesian);
    if(rank == 0)
    {
        print_matrix(matrix,dims);
    }
    int source,dest;
    int cords[2]={0};

    MPI_Barrier(cartesian);
    MPI_Cart_coords(cartesian,rank,2,cords);
    MPI_Cart_shift(cartesian, 1, 1, &source, &dest);

    // Print out the ranks of left and right neighbors
    printf("Rank %d(%d,%d) has left neighbor %d and right neighbor %d\n", rank,cords[0],cords[1], source, dest);

    // Find ranks of neighbors in the up and down directions
    MPI_Cart_shift(cartesian, 0, 1, &source, &dest);

    // Print out the ranks of up and down neighbors
    printf("Rank %d(%d,%d) has up neighbor %d and down neighbor %d\n", rank,cords[0],cords[1], source, dest);

    MPI_Finalize();
}