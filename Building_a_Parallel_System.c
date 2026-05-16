#include <stdio.h>
#include <mpi.h>
#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define N 50

/* ================= INTEGER ================= */
long long factorial(int n)
{
    long long result = 1;

    #pragma omp parallel for reduction(*:result)
    for(int i=1;i<=n;i++)
        result *= i;

    return result;
}

/* ================= STRING ================= */
void reverse_string(char *str)
{
    int n = strlen(str);

    #pragma omp parallel for
    for(int i=0;i<n/2;i++)
    {
        char temp = str[i];
        str[i] = str[n-i-1];
        str[n-i-1] = temp;
    }
}

int count_vowels(char *str)
{
    int count = 0;
    int len = strlen(str);

    #pragma omp parallel for reduction(+:count)
    for(int i=0;i<len;i++)
    {
        char c = tolower(str[i]);
        if(c=='a'||c=='e'||c=='i'||c=='o'||c=='u')
            count++;
    }

    return count;
}

/* ================= FILE ================= */
void split_file(char *filename)
{
    FILE *f = fopen(filename,"r");
    FILE *even = fopen("even.txt","w");
    FILE *odd = fopen("odd.txt","w");

    if(!f)
    {
        printf("File not found!\n");
        return;
    }

    char lines[200][256];
    int count = 0;

    while(count < 200 && fgets(lines[count],256,f))
        count++;

    #pragma omp parallel for
    for(int i=0;i<count;i++)
    {
        #pragma omp critical
        {
            if(i%2==0)
                fputs(lines[i],even);
            else
                fputs(lines[i],odd);
        }
    }

    fclose(f);
    fclose(even);
    fclose(odd);
}

/* ================= MATRIX ================= */
void matrix_add(double A[N][N], double B[N][N],
                double C[N][N], int start, int rows)
{
    #pragma omp parallel for collapse(2)
    for(int i=start;i<start+rows;i++)
        for(int j=0;j<N;j++)
            C[i][j] = A[i][j] + B[i][j];
}

/* ================= MAIN ================= */
int main(int argc, char** argv)
{
    int rank, size;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    /* SAFE CHECK */
    if(size < 10)
    {
        if(rank == 0)
            printf("ERROR: Run with at least 10 processes\n");

        MPI_Finalize();
        return 0;
    }

    /* ================= MASTER ================= */
    if(rank == 0)
    {
        int number = 6;
        char str[] = "parallel";
        char file[] = "input.txt";

        double A[N][N], B[N][N];

        for(int i=0;i<N;i++)
            for(int j=0;j<N;j++)
            {
                A[i][j] = i + j;
                B[i][j] = i * j;
            }

        MPI_Send(&number,1,MPI_INT,1,0,MPI_COMM_WORLD);
        MPI_Send(str,strlen(str)+1,MPI_CHAR,2,0,MPI_COMM_WORLD);
        MPI_Send(file,strlen(file)+1,MPI_CHAR,3,0,MPI_COMM_WORLD);

        MPI_Send(A,N*N,MPI_DOUBLE,4,0,MPI_COMM_WORLD);
        MPI_Send(B,N*N,MPI_DOUBLE,4,0,MPI_COMM_WORLD);

        long long fact;
        MPI_Recv(&fact,1,MPI_LONG_LONG,1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        char resultStr[100];
        int vowels;
        MPI_Recv(resultStr,100,MPI_CHAR,2,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Recv(&vowels,1,MPI_INT,2,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        char msg[100];
        MPI_Recv(msg,100,MPI_CHAR,3,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        double C[N][N];
        MPI_Recv(C,N*N,MPI_DOUBLE,4,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        printf("\n===== FINAL RESULTS =====\n");
        printf("Factorial = %lld\n",fact);
        printf("Reversed String = %s\n",resultStr);
        printf("Vowels = %d\n",vowels);
        printf("%s\n",msg);
        printf("Matrix Addition Done\n");
    }

    else if(rank == 1)
    {
        int num;
        MPI_Recv(&num,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        long long result = factorial(num);

        MPI_Send(&result,1,MPI_LONG_LONG,0,0,MPI_COMM_WORLD);
    }

    else if(rank == 2)
    {
        char str[100];

        MPI_Recv(str,100,MPI_CHAR,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        reverse_string(str);
        int vowels = count_vowels(str);

        MPI_Send(str,100,MPI_CHAR,0,0,MPI_COMM_WORLD);
        MPI_Send(&vowels,1,MPI_INT,0,0,MPI_COMM_WORLD);
    }

    else if(rank == 3)
    {
        char filename[100];

        MPI_Recv(filename,100,MPI_CHAR,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        split_file(filename);

        char msg[] = "File split finished";

        MPI_Send(msg,strlen(msg)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);
    }

    else if(rank == 4)
    {
        double A[N][N], B[N][N], C[N][N];

        MPI_Recv(A,N*N,MPI_DOUBLE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Recv(B,N*N,MPI_DOUBLE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        int workers = size - 5;
        if(workers <= 0) workers = 1;

        int rows = N / workers;

        for(int p=5;p<size;p++)
        {
            MPI_Send(A,N*N,MPI_DOUBLE,p,0,MPI_COMM_WORLD);
            MPI_Send(B,N*N,MPI_DOUBLE,p,0,MPI_COMM_WORLD);
            MPI_Send(&rows,1,MPI_INT,p,0,MPI_COMM_WORLD);

            int start = (p-5)*rows;
            MPI_Send(&start,1,MPI_INT,p,0,MPI_COMM_WORLD);
        }

        for(int p=5;p<size;p++)
        {
            int start = (p-5)*rows;
            MPI_Recv(&C[start][0],rows*N,MPI_DOUBLE,
                     p,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        }

        MPI_Send(C,N*N,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
    }

    else if(rank >= 5)
    {
        double A[N][N], B[N][N], C[N][N];
        int rows, start;

        MPI_Recv(A,N*N,MPI_DOUBLE,4,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Recv(B,N*N,MPI_DOUBLE,4,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Recv(&rows,1,MPI_INT,4,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Recv(&start,1,MPI_INT,4,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        matrix_add(A,B,C,start,rows);

        MPI_Send(&C[start][0],rows*N,MPI_DOUBLE,4,0,MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}