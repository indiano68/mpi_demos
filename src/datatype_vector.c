#include <mpi.h>
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

/*
    Demonstrate  how to fill up a matrix in parallel
*/
void print_square_matrix(int * array, size_t side_lenght)
{
    for(size_t index = 0; index< side_lenght*side_lenght; index ++)
    {
        printf(" %02d ",array[index]);
        if(index!= 0 && (index+1)%side_lenght == 0 ) printf("\n");
    }
}

void fill_square_matrix(int * array, size_t side_lenght,int fill)
{
    for(size_t index = 0; index< side_lenght*side_lenght; index ++)
    {
        array[index] = fill;
    }
}

void fill_column(int* array, size_t side_lenght, int rank)
{
    for( size_t index = 0; index<side_lenght; index++)
    {
        array[index] = side_lenght*rank + index;
    }
}

void ibuilt_prompt(int * array, size_t side_lengt, int rank)
{
    printf(" I am %d ",rank);
    printf("and i built ");
    for(size_t idx = 0; idx<side_lengt ; idx ++)
        printf(" %02d ", array[idx]);
    printf("\n");
}

int main(int argc, char ** argv) 
{
    MPI_Init(&argc,&argv);

    int rank = 0;
    int size = 0;

    MPI_Datatype mpi_column;
    MPI_Datatype mpi_column_rcv;
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);


    MPI_Type_vector(size,1,size,MPI_INT,&mpi_column);
    MPI_Type_commit(&mpi_column);

    MPI_Type_create_resized(mpi_column,0,sizeof(int),&mpi_column_rcv);
    MPI_Type_commit(&mpi_column_rcv);

    int * column = calloc(size,sizeof(int));
    int * matrix; 
    fill_column(column,size,rank);
    ibuilt_prompt(column,size,rank);
    if (rank == 0)
    {
        matrix = calloc(size*size,sizeof(int));
    }

    MPI_Gather(column,size,MPI_INT,matrix,1,mpi_column_rcv,0,MPI_COMM_WORLD);
    if(rank==0)
    {
        printf("On 0 i gathered:\n");
        print_square_matrix(matrix,size);
    }
    free(matrix);
    free(column);
    MPI_Type_free(&mpi_column);
    MPI_Type_free(&mpi_column_rcv);
    MPI_Finalize();
}


