#include <mpi.h>
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

/*

    Demostrates use simple ghost cell exchange in cartesian topology and writing everything to file!

*/


void print_matrix(int * array, int * dims)
{
    for(size_t index = 0; index< dims[0]*dims[1]; index ++)
    {
        printf(" %02d ",array[index]);
        if(index!= 0 && (index+1)%dims[1] == 0 ) printf("\n");
    }
}

void print_square_matrix(int * array, int side_lenght)
{
    print_matrix(array, (int[2]){side_lenght,side_lenght});
}


void print_2d_cross(int *neigh_arr, int rank)
{
    printf("   %02d   \n",neigh_arr[0]);
    printf("%02d %02d %02d\n",neigh_arr[2],rank,neigh_arr[3]);
    printf("   %02d   \n",neigh_arr[1]);
}

void fill_square_matrix(int * array, size_t side_lenght, int fill)
{
    for(size_t index = 0; index< side_lenght*side_lenght; index ++)
    {
        array[index] = fill;
    }
}

int * intialize_cell(size_t core_side_lenght, int rank)
{
    int * new_cell = calloc((core_side_lenght+2)*(core_side_lenght+2),sizeof(int));
    fill_square_matrix(new_cell, core_side_lenght + 2, rank);
    new_cell[0] = 0;
    new_cell[core_side_lenght+1] = 0;
    new_cell[(core_side_lenght+2)*4] = 0;
    new_cell[(core_side_lenght+2)*4+ core_side_lenght+1] = 0;
    return new_cell;
}


int main(int argc, char ** argv) 
{
    MPI_Init(&argc,&argv);
    int rank,size;
    int core_side = 3;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    int * local_cell = intialize_cell(core_side,rank);
    int rank_indicator = 0;
    if(argc!=1)
    {
        rank_indicator = atoi(argv[1]);
    }
    int dims[2] ={0};
    MPI_Dims_create(size,2,dims);

    MPI_Comm cartesian; 
    MPI_Cart_create(MPI_COMM_WORLD, 2,dims,(int[2]){1,1},0,&cartesian);
    int * matrix ; 

    if(rank == 0 )
    {
        matrix = calloc(dims[0]*dims[1],sizeof(int));
        printf("{%d, %d}\n", dims[0],dims[1]);
    }   

    MPI_Gather(&rank,1,MPI_INT,matrix,1,MPI_INT,0,cartesian);

    if(rank == 0)
    {
        printf("##### Setup #### \n\n");
        print_matrix(matrix,dims);
        printf("\n################ \n");
    }

    MPI_Barrier(cartesian);

    MPI_Datatype contiguos_boundary;
    MPI_Datatype vector_boundary;

    MPI_Type_contiguous(core_side, MPI_INT, &contiguos_boundary);
    MPI_Type_vector(core_side,1,core_side+2,MPI_INT,&vector_boundary);

    MPI_Type_commit(&contiguos_boundary);
    MPI_Type_commit(&vector_boundary);
    int count[] = {1,1,1,1};
    MPI_Aint send_displacement[] = {
                                (core_side+3)*sizeof(int),
                                ((core_side+2)*3+1)*sizeof(int),
                                (core_side+3)*sizeof(int),
                                (2*core_side+2)*sizeof(int)
                            };

    MPI_Aint recieve_displacement[] = {
                                sizeof(int),
                                ((core_side+2)*4+1)*sizeof(int),
                                (core_side+2)*sizeof(int),
                                (2*core_side+3)*sizeof(int)
                                };

    MPI_Datatype types[4] = {contiguos_boundary,contiguos_boundary,vector_boundary,vector_boundary};
    MPI_Neighbor_alltoallw(local_cell,count,send_displacement,types,local_cell,count,recieve_displacement,types,cartesian);
    if (rank == rank_indicator )
     print_square_matrix(local_cell, core_side +2);

    MPI_Type_free(&contiguos_boundary);
    MPI_Type_free(&vector_boundary);

    MPI_Datatype row_etype;
    int etype_size  = 0;
    MPI_Type_contiguous(16,MPI_CHAR,&row_etype);
    MPI_Type_commit(&row_etype);
    MPI_Type_size(row_etype,&etype_size);

    MPI_Datatype filetype;
    int filetype_size  = 0;
    // MPI_Type_contiguous(dims[1],row_etype,&filetype);
    MPI_Type_create_resized(row_etype, 0, 3*etype_size, &filetype);
    MPI_Type_commit(&filetype);
    MPI_Type_size(filetype,&filetype_size);
    // printf("%d %d\n",rank,(int)(rank/dims[1]));
    MPI_File file_handle; 
    MPI_File_open(MPI_COMM_WORLD,"matrix_total.msg",MPI_MODE_WRONLY|MPI_MODE_CREATE,MPI_INFO_NULL,&file_handle);
    MPI_File_set_view(file_handle,etype_size*(rank%dims[1])+(15*etype_size)*((int)(rank/dims[1])),row_etype,filetype,"external32",MPI_INFO_NULL);
    // MPI_File_set_view(file_handle,etype_size*(rank%dims[1]),row_etype,filetype,"external32",MPI_INFO_NULL);
    char string[16];
    int coord[2];
    MPI_Cart_coords(cartesian,rank,2,coord);

    for(int i = 0 ; i <5;i++)
    {
        if(coord[1] == dims[1]-1 && i!=4)
            sprintf(string,"%2d %2d %2d %2d %2d \n",local_cell[5*i],local_cell[5*i+1],local_cell[5*i+2],local_cell[5*i+3], local_cell[5*i+4]);
        else if(coord[1] == dims[1]-1 && i==4)
            sprintf(string,"%2d %2d %2d %2d %2d\n\n",local_cell[5*i],local_cell[5*i+1],local_cell[5*i+2],local_cell[5*i+3], local_cell[5*i+4]);
        else
            sprintf(string,"%2d %2d %2d %2d %2d  ",local_cell[5*i],local_cell[5*i+1],local_cell[5*i+2],local_cell[5*i+3], local_cell[5*i+4]);
        // if(rank<3)
            MPI_File_write(file_handle,string,1,row_etype,MPI_STATUS_IGNORE);
    }
    MPI_File_close(&file_handle);
    MPI_Type_free(&row_etype);
    MPI_Type_free(&filetype);
    MPI_Finalize();
}