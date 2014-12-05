#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
   
int p, id;
   
void read_matrix(char* filename, float** storage_mat, int* n,int offset)
{ 
	FILE* finptr;
	int m,i,j;


	if(id==p-1){		/*last process*/

		finptr = fopen(filename, "r");
		if(!finptr) {
			perror("ERROR: can't open matrix file\n");
			MPI_Finalize();	
			exit(1); 
		}
		if(fread(&m, sizeof(int), 1, finptr) != 1 || fread(n, sizeof(int), 1, finptr) != 1) {
			perror("Error reading matrix file");
			MPI_Finalize();	
			exit(1);
		}
		if(m!=(*n)){
			printf("Matrix is not square width:%d height:%d\n", *n,m); 
			MPI_Finalize();	
			exit(1);
		}

		*storage_mat = (float*)malloc(m*m/p*sizeof(float));
		
		for(i=0; i<p-1; i++)
			MPI_Send(&m, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			
		for(i=0; i<p; i++){   											  		  

			if(!offset){
				if(fread(*storage_mat, sizeof(float), m*m/p, finptr)==-1)
					printf("data read failed\n");
			}
			else if(!i){
				for(j=0; j<offset; j++)
					if(fread(*storage_mat, sizeof(float), m*m/p, finptr)==-1)
						printf("data read failed\n");
			}			
			MPI_Send(*storage_mat, m*m/p, MPI_FLOAT, i, 1, MPI_COMM_WORLD);														
		}

		
		MPI_Recv(*storage_mat,m*m/p,MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		/*if(fread(*storage_mat, sizeof(float), m, finptr)==-1)
			printf("data read failed\n");*/
			
		fclose(finptr);
	}
	else{

		MPI_Recv(n,1,MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		m=*n;
		*storage_mat = (float*)malloc(m*m/p*sizeof(float));
		
		MPI_Recv(*storage_mat,m*m/p,MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
	}
}

/* process 0 writes out the matrix n/p rows at a time on behalf of all
   other processes */
void write_matrix(char* filename, float* storage_mat,int n)
{
	int i;
  FILE *foutptr;
	
	/* open file */
	if(!id) {

		foutptr = fopen (filename, "w");
		if(!foutptr) {
			perror("ERROR: can't open output file\n");
			exit(3);
		}

		fwrite (&n, sizeof(int), 1, foutptr);
		fwrite (&n, sizeof(int), 1, foutptr);
		fwrite (storage_mat, sizeof(float), n*n/p , foutptr);

		for(i=1; i<p; i++) {

			/* mpi receive the array storage_mat from process rank i */
			MPI_Recv(storage_mat,n*n/p,MPI_FLOAT, i, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			fwrite (storage_mat, sizeof(float), n*n/p , foutptr);
		}
	}
	else
		MPI_Send(storage_mat, n*n/p, MPI_FLOAT, 0, 1, MPI_COMM_WORLD);
	
	/* close file */
}

int main(int argc, char* argv[])
{

	float* storage_matA,*storage_matB,*storage_matC;		/*local matrix chunks*/
	float **a,**b,**c;
	char* fileA, *fileB, *fileC;									/*names of matrix files*/
	int g,h,i,j,k;
	int zA,zB,n;														/*sizes*/

	MPI_Init(&argc,&argv);   
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	
	if(argc != 4) { 
		printf("USAGE: mpirun -np <p> %s <fileA> <fileB> <fileC>\n", argv[0]); 
		MPI_Finalize();
		return -1; 
	}

	fileA=argv[1];	
	fileB=argv[2];
	fileC=argv[3];
		
	/*allocate space for stripes A, B, and C*/
	
	read_matrix(fileA,&storage_matA,&zA,0);
	/*printf("(%d)matA:%f\t%f\t%f\t%f\n",id,*storage_matA,*(storage_matA+1),*(storage_matA+2),*(storage_matA+3));*/
	n=zA;
	
	a = (float**)malloc(n/p*sizeof(float*));										
  	for(i=0; i<n/p; i++) 
  		a[i] = &(storage_matA)[i*n];
	
	/* allocate space and intialize to zero for storage_matC as an array
	of floats of n x (n/p) in size */	
	
	storage_matC = (float*)malloc(n*n/p*sizeof(float));
	memset(storage_matC, 0, n*n/p*sizeof(float));
	
	c = (float**)malloc(n/p*sizeof(float*));										
  	for(i=0; i<n/p; i++) 
  		c[i] = &(storage_matC)[i*n];
  		
	for(h=0;h<p;h++){
		read_matrix(fileB,&storage_matB,&zB,h+1);
		b = (float**)malloc(n/p*sizeof(float*));										
	  	for(j=0; j<n/p; j++) 
  			b[j] = &(storage_matB)[j*n];
		
		
		if(zA!=zB){
			printf("Matrices must have same dimensions, M1:%d^2 M2:%d^2\n",zA,zB); 
			MPI_Finalize();
			return -1; 
		}
		
		for(g=0; g<n; g+=n/p)
			for(i=0; i<n/p; i++)
				for(j=0; j<n/p; j++)
					for(k=0; k<n/p; k++){
						c[i][j+g] += a[i][k+h*n/p]*b[k][j+g];

		}
			
		free(storage_matB);
		free(b);	
	}


	/*MPI_Barrier(MPI_COMM_WORLD);*/

	write_matrix(fileC, storage_matC,n);


	free(storage_matA);
	free(a);
	free(storage_matC);
	free(c);
	
	MPI_Finalize(); 
	return 0;
	/* reclaim matrices, finalize mpi */
}

