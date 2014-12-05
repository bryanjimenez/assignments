#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#define THRESHOLD 4



/* read a matrix from a file */
void read_matrix(char* fname, float*** a, float** sa, int* m, int* n)
{
  FILE* finptr;
  int i, sz;

  finptr = fopen(fname, "r");
  if(!finptr) {
    perror("ERROR: can't open matrix file\n");
    exit(1); 
 }

  if(fread(m, sizeof(int), 1, finptr) != 1 ||
     fread(n, sizeof(int), 1, finptr) != 1) {
    perror("Error reading matrix file");
    exit(1);
  }
  sz = (*m)*(*n);

  *sa = (float*)malloc(sz*sizeof(float));
  if(fread(*sa, sizeof(float), sz, finptr) != sz) {
    perror("Error reading matrix file");
    exit(1);
  }

  *a = (float**)malloc((*m)*sizeof(float*));										
  for(i=0; i<*m; i++) (*a)[i] = &(*sa)[i*(*n)];

  fclose(finptr);
}

/* write a matrix to a file */
void write_matrix(char* fname, float* sa, int m, int n)
{
  FILE* foutptr;
  int i;
  float* ptr;

  foutptr = fopen(fname, "w");
  if(!foutptr) {
    perror("ERROR: can't open matrix file\n");
    exit(1); 
 }

  if(fwrite(&m, sizeof(int), 1, foutptr) != 1 ||
     fwrite(&n, sizeof(int), 1, foutptr) != 1) {
    perror("Error reading matrix file");
    exit(1);
  }

  ptr = sa;
  for(i=0; i<m; i++) {
    if(fwrite(ptr, sizeof(float), n, foutptr) != n) {
      perror("Error reading matrix file");
      exit(1);
    }
    ptr += n;
  }

  fclose(foutptr);
}

/* dumb matrix multiplication; used for debugging purposes */
void dumb_matmul(float** a, float** b, float** c, int N) 
{
  int i, j, k;
  for(i=0; i<N; i++)
    for(j=0; j<N; j++)
      for(k=0; k<N; k++)
	c[i][j] += a[i][k]*b[k][j];
	
}
void my_dumb_matmul(float* a, float* b, float* c, int n, int N) 
{

  int i, j, k;
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			for(k=0; k<n; k++){
				c[i*N+j] += a[k+i*N]*b[k*N+j];
				/*printf("%.0f ",a[i*N+k]*b[k*N+j]);*/
			}
		}
	/*printf("\n");*/
	}
	/*printf("dumb-N:%d n:%d\n",N,n);*/
}

struct myargs {
	int crow;
	int ccol;	
	int arow;
	int acol;	
	int brow;
	int bcol;
	int n;
	int N;
	float* a;
	float* b;
	float* c;
	int p;
};

/* matrix multiplication using recursive block decomposition */
void recursive_matmul(int crow, int ccol, /* corner of C block */
		      int arow, int acol, /* corner of A block */
		      int brow, int bcol, /* corner of B block */
		      int n, /* block size */
		      int N, /* matrices are N*N */
		      float** a, float** b, float** c, int p) /*p=blocksize*/
{
		/*printf("mult-N:%d n:%d crow:%d ccol:%d\n",N,n,crow,ccol);*/
	
  if(n > THRESHOLD) {

    /* break up the matrices and call recursive_matmul on sub-matrices */


		recursive_matmul(crow+0, ccol+0, arow+0, acol+0, brow+0, bcol+0, n/p, N, a, b, c, p);
      recursive_matmul(crow+0, ccol+0, arow+0, acol+n/p, brow+n/p, bcol+0, n/p, N, a, b, c, p);					


		recursive_matmul(crow+0, ccol+n/p, arow+0, acol+0, brow+0, bcol+n/p, n/p, N, a, b, c, p);	
		recursive_matmul(crow+0, ccol+n/p, arow+0, acol+n/p, brow+n/p, bcol+n/p, n/p, N, a, b, c, p);	


		recursive_matmul(crow+n/p, ccol+0, arow+n/p, acol+0, brow+0, bcol+0, n/p, N, a, b, c, p);		
		recursive_matmul(crow+n/p, ccol+0, arow+n/p, acol+n/p, brow+n/p, bcol+0, n/p, N, a, b, c, p);	


		recursive_matmul(crow+n/p, ccol+n/p, arow+n/p, acol+0, brow+0, bcol+n/p, n/p, N, a, b, c, p);			
		recursive_matmul(crow+n/p, ccol+n/p, arow+n/p, acol+n/p, brow+n/p, bcol+n/p, n/p, N, a, b, c, p);

		
  } else {

    /* multiply the two matrices right away */
	my_dumb_matmul(&(a[arow][acol]), &(b[brow][bcol]), &(c[crow][ccol]), n, N);

  }
}
void *thread_matmul(void* arg){
	struct myargs* s = (struct myargs*)arg;
	/*printf("Thread\n");*/
/*	recursive_matmul(s->crow, s->ccol, s->arow, s->acol, s->brow, s->bcol, s->n, s->N, s->a, s->b, s->c, s->p);*/
	int z;
	int N=s->N;
	int n=s->n;
	/*iterate through colums and rows multiplying ab and summing each move*/
	for(z=0;z<N;z+=n)
		my_dumb_matmul(s->a+z, s->b+z*N, s->c, n, N);
	return 0;
}
/* call this function to do matrix multiplication */
void matmul(float** a, float** b, float** c, int N, int p) 
{ 
		int i,j,n;
		/*struct myargs s;*/
	  	int count=0;	
		struct myargs *s = (struct myargs*)malloc(p*sizeof(struct myargs));
		pthread_t *t = (pthread_t*)malloc(p*sizeof(pthread_t));
		n=N/sqrt(p);	

			for(i=0; i<sqrt(p); i++){	/*ith block*/
				for(j=0; j<sqrt(p); j++){/*jth block*/

					s[count].crow=i;
					s[count].ccol=j;
					s[count].arow=i;
					s[count].acol=j*0;
					s[count].brow=i*0;
					s[count].bcol=j;
					s[count].n=N/sqrt(p);
					s[count].N=N;
					s[count].a=&(a[i*n][j*n*0]);
					s[count].b=&(b[i*n*0][j*n]);
					s[count].c=&(c[i*n][j*n]);
					s[count].p=sqrt(p);				


					pthread_create(t++, NULL, thread_matmul, &s[count]);
				  	/*pthread_join(*t++, NULL);*/

				  	count++;
				}

			}
			for(i=0;i<p;i++)
			/*t--;*/
			pthread_join(*t--, NULL);
		

		
		
	free(s);
	free(t);

  /*recursive_matmul(0, 0, 0, 0, 0, 0, N, N, a, b, c);*/
  /*recursive_matmul(0, 0, 0, 0, 0, 0, N, N, a, b, c, (int)sqrt(p));*/
  /*dumb_matmul(a, b, c, N);*/

}

int main (int argc, char * argv[]) 
{

  int n; /* dimension of the matrix */
  float *sa, *sb, *sc; /* storage for matrix A, B, and C */
  float **a, **b, **c; /* 2-d array to access matrix A, B, and C */
  int i, j;
  int p;
  
  if(argc != 5) {
    printf("Usage: %s fileA fileB fileC threadsP\n", argv[0]);
    return 1;
  }

  /* read matrix A */
  printf("read matrix A from %s\n", argv[1]);
  read_matrix(argv[1], &a, &sa, &i, &j);
  if(i != j) { printf("ERROR: matrix A not square\n"); return 2; }
  n = i;

  /* read matrix B */
  printf("read matrix B from %s\n", argv[2]);
  read_matrix(argv[2], &b, &sb, &i, &j);
  if(i != j) { printf("ERROR: matrix B not square\n"); return 2; }
  if(n != i) { printf("ERROR: matrix A and B incompatible\n"); return 2; }

	p=atoi(argv[4]);
	/*printf("P:%d\n",p);*/
	if(sqrt(p)-(int)(sqrt(p))){ printf("The number of threads(%d) is not a square\n",p); return 2;}
	if(n%(int)sqrt(p)){ printf("Matrix size(%d) is not divisible by the threads(%d)\n",n,p); return 2;}

  /* initialize matrix C */
  sc = (float*)malloc(n*n*sizeof(float));
  memset(sc, 0, n*n*sizeof(float));
  c = (float**)malloc(n*sizeof(float*));
  for(i=0; i<n; i++) c[i] = &sc[i*n];

  /* do the multiplication */
  matmul(a, b, c, n, (int)p);
  
  /* write matrix C */
  write_matrix(argv[3], sc, n, n);
  
  /*debug*/
  /*memset(sc, 0, n*n*sizeof(float));
  dumb_matmul(a, b, c, n);
  write_matrix("m0", sc, n, n);
  */

  free(a);
  free(b);
  free(c);
  free(sa);
  free(sb);
  free(sc);
      
  return 0;
}
