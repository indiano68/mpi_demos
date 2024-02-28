#include <mpi.h>
#include <stdio.h> 
#include <string.h>

/*
    Simple hello world for MPI_IO
*/

int main(int argc, char ** argv) 
{
    MPI_Init(&argc,&argv);
    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    char string[256];
    MPI_Datatype mpi_string;
    sprintf(string,"I am rank %2d, Hello Word!\n\0",rank);
    MPI_Type_contiguous(strlen(string),MPI_CHAR,&mpi_string);
    MPI_Type_commit(&mpi_string);
    int type_size = 0;
    MPI_File file_handle;

    MPI_File_open(MPI_COMM_WORLD,"hello_world.msg", MPI_MODE_WRONLY | MPI_MODE_CREATE ,MPI_INFO_NULL, &file_handle );
    MPI_Type_size(mpi_string, &type_size);
    MPI_File_set_view(file_handle,rank*type_size,mpi_string,mpi_string,"external32",MPI_INFO_NULL);
    MPI_File_write(file_handle,string,1,mpi_string,MPI_STATUSES_IGNORE);
    MPI_File_close(&file_handle);
    MPI_Type_free(&mpi_string);
    MPI_Finalize();
}