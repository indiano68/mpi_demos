#include <mpi.h>
#include <stdio.h> 
#include <string.h>

/*
    Demonstrate how to send lower triangle of a matrix
*/

void print_square_matrix(int * array, size_t side_lenght)
{
    for(size_t index = 0; index< side_lenght*side_lenght; index ++)
    {
        printf(" %02d ",array[index]);
        if(index!= 0 && (index+1)%side_lenght == 0 ) printf("\n");
    }
}

void fill_square_matrix(int * array, size_t side_lenght)
{
    for(size_t index = 0; index< side_lenght*side_lenght; index ++)
    {
        array[index] = index;
    }
}


int main(int argc, char ** argv) 
{
    MPI_Init(&argc,&argv);

    int rank,size;
    MPI_Datatype lower_triangle;
    int block_lenghts[10] = {0};
    int displacements[10] = {0};
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    for(int i = 0; i<10;i++)
    {
        block_lenghts[i] = i+1;
        displacements[i] = i*10;
    }

    MPI_Type_indexed(10,block_lenghts,displacements,MPI_INT,&lower_triangle);
    MPI_Type_commit(&lower_triangle);
    int matrix_a[100] = {0};
    if(rank == 0)
    {
        fill_square_matrix(matrix_a,10);
        printf("I am 0 and i build:\n");
        print_square_matrix(matrix_a,10);
        MPI_Send(matrix_a,1,lower_triangle,1,0,MPI_COMM_WORLD);
    }
    if(rank==1)
    {
        MPI_Recv(matrix_a,1,lower_triangle,0,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUSES_IGNORE);
        printf("I am 1 and I got:\n");
        print_square_matrix(matrix_a,10);
    }
    MPI_Type_free(&lower_triangle);
    MPI_Finalize();
}