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

void print_2d_cross(int *neigh_arr, int rank)
{
    printf("   %02d   \n",neigh_arr[0]);
    printf("%02d %02d %02d\n",neigh_arr[2],rank,neigh_arr[3]);
    printf("   %02d   \n",neigh_arr[1]);
}

int main(int argc, char ** argv) 
{
    MPI_Init(&argc,&argv);
    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    int * matrix;
    int neigh[4] = {rank,rank,rank,rank};
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
    MPI_Barrier(cartesian);
    if(rank == 2)
    {   printf("###### RANK 2 Cross before allgather \n");
        print_2d_cross(neigh,rank);
    }
    MPI_Neighbor_allgather(&rank,1,MPI_INT,neigh,1,MPI_INT,cartesian);
    if(rank == 2)
    {
        printf("###### RANK 2 Cross after allgather \n");
        print_2d_cross(neigh,rank);
    }
    MPI_Finalize();
}