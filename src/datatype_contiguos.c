#include <mpi.h>
#include <stdio.h> 
#include <string.h>


int main(int argc, char ** argv) 
{
    MPI_Init(&argc,&argv);

    int rank = 0;
    char somestring[256]={0};
    MPI_Datatype mpi_string;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);


    MPI_Type_contiguous(256,MPI_CHAR,&mpi_string);
    MPI_Type_commit(&mpi_string);

    if (rank == 0)
    {
        strcpy(somestring,"Hi man, whatsupp?");
    }

    MPI_Bcast(somestring,1,mpi_string,0,MPI_COMM_WORLD);

    printf("%s I am process %d \n",somestring, rank);

    MPI_Type_free(&mpi_string);
    MPI_Finalize();
}