#include <mpi.h>
#include <stdio.h> 
#include <string.h>
#include <stddef.h>
/*
    Demonstrate how to send a struct over mpi
*/

typedef struct
{
    char Name[10];
    char Surname[10];
    unsigned short age;
    float height;
} Person;


void print_person(Person person)
{
    printf("Name   : %+10s\n",person.Name);
    printf("Surname: %+10s\n",person.Surname);
    printf("Age    : %10d\n",person.age);
    printf("Height : %10.2f\n",person.height);
}
int main(int argc, char ** argv) 
{
    MPI_Init(&argc,&argv);

    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    MPI_Datatype name_type;
    MPI_Type_contiguous(10,MPI_CHAR,&name_type);
    MPI_Type_commit(&name_type);

    MPI_Datatype person_type;
    MPI_Type_struct(3,
                    (int[4]){2,1,1},
                    (MPI_Aint[4]){offsetof(Person,Name),offsetof(Person,age),offsetof(Person,height)}
                    ,(MPI_Datatype[3]){name_type,MPI_UNSIGNED_SHORT,MPI_FLOAT},
                    &person_type);
    MPI_Type_commit(&person_type);

    Person local_person;
    if(rank == 0)
    {
        strcpy(local_person.Name,"Erik");
        strcpy(local_person.Surname,"Fabrizzi");
        local_person.age = 24;
        local_person.height = 1.80;
        printf("I am rank 0 and this is my person: \n");
        print_person(local_person);
        MPI_Send(&local_person,1,person_type,1,0,MPI_COMM_WORLD);
    }
    if(rank == 1)
    {
        MPI_Recv(&local_person,1,person_type,0,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        printf("I am rank 1 and this is my person: \n");
        print_person(local_person);
    }
    MPI_Type_free(&name_type);
    MPI_Type_free(&person_type);
    MPI_Finalize();
}