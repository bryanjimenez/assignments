#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define KEYSZ 10
#define WORDSZ 8
#define BUFFSZ	64

int l[WORDSZ/2],r[WORDSZ/2],keys[2][WORDSZ];

void haltwith(char *e){
	perror(e);
	exit(1);
}

void usage(char *u){
	printf("\tusage:\t%s\n",u);
	printf("\t  \t(default operation is encrypt)\n");
	printf("\t-d\t(decrypt)\n");
	
	exit(1);
}

void dec2bin(int d, int *b)
{
	int decimal[]={128,64,32,16,8,4,2,1},i;

	for(i=0;i<8;i++)
		if(d-decimal[i]>=0){
			b[i]=1;
			d-=decimal[i];
		}
		else
			b[i]=0;	
}

void bin2dec(int *b, unsigned char *d)
{
	int decimal[]={128,64,32,16,8,4,2,1},i;
	*d=0;
	for(i=0;i<8;i++)
		if(b[i]==1)
			*d+=decimal[i];	
}

void P10(int *keyi, int *keyo)		/*P10 permutation*/
{
	keyo[0] = keyi[2];
	keyo[1] = keyi[4];
	keyo[2] = keyi[1];
	keyo[3] = keyi[6];
	keyo[4] = keyi[3];
	keyo[5] = keyi[9];
	keyo[6] = keyi[0];
	keyo[7] = keyi[8];
	keyo[8] = keyi[7];
	keyo[9] = keyi[5];

}

void P8(int *keyi, int *keyo)			/*P8 permutation*/
{
	keyo[0] = keyi[5];
	keyo[1] = keyi[2];
	keyo[2] = keyi[6];
	keyo[3] = keyi[3];
	keyo[4] = keyi[7];
	keyo[5] = keyi[4];
	keyo[6] = keyi[9];
	keyo[7] = keyi[8];
}
void P4(int *keyi, int *keyo)			/*P4 permutation*/
{
	keyo[0] = keyi[1];
	keyo[1] = keyi[3];
	keyo[2] = keyi[2];
	keyo[3] = keyi[0];
}


void key2ip(int *key, int *ip)		/*IP permutation*/
{
	ip[0] = key[1];
	ip[1] = key[5];
	ip[2] = key[2];
	ip[3] = key[0];
	ip[4] = key[3];
	ip[5] = key[7];
	ip[6] = key[4];
	ip[7] = key[6];
}

void ip2key(int *ip, int *key)		/*IP^-1 permutation*/
{
	key[0] = ip[3];
	key[1] = ip[0];
	key[2] = ip[2];
	key[3] = ip[4];
	key[4] = ip[6];
	key[5] = ip[1];
	key[6] = ip[7];
	key[7] = ip[5];
}

void EP(int *keyi, int *keyo)			/*expansion/permutation */
{
	keyo[0] = keyi[3];
	keyo[1] = keyi[0];
	keyo[2] = keyi[1];
	keyo[3] = keyi[2];
	keyo[4] = keyi[1];
	keyo[5] = keyi[2];
	keyo[6] = keyi[3];
	keyo[7] = keyi[0];
}

int sboxes(int box,int x,int y){		/*s-box matrices*/
	int boxes[2][4][4];
	
	boxes[0][0][0]=1; boxes[0][0][1]=0; boxes[0][0][2]=3; boxes[0][0][3]=2;
	boxes[0][1][0]=3; boxes[0][1][1]=2; boxes[0][1][2]=1; boxes[0][1][3]=0;
	boxes[0][2][0]=0; boxes[0][2][1]=2; boxes[0][2][2]=1; boxes[0][2][3]=3;
	boxes[0][3][0]=3; boxes[0][3][1]=1; boxes[0][3][2]=3; boxes[0][3][3]=2;
	
	boxes[1][0][0]=0; boxes[1][0][1]=1; boxes[1][0][2]=2; boxes[1][0][3]=3;
	boxes[1][1][0]=2; boxes[1][1][1]=0; boxes[1][1][2]=1; boxes[1][1][3]=3;
	boxes[1][2][0]=3; boxes[1][2][1]=0; boxes[1][2][2]=1; boxes[1][2][3]=0;
	boxes[1][3][0]=2; boxes[1][3][1]=1; boxes[1][3][2]=0; boxes[1][3][3]=3;
	
	return boxes[box][x][y];
}


void s_box(int *sip,int *p,int sbno,int i)
{
	int x,y,sop;
	x = sip[3]+sip[0]*2;
	y = sip[2]+sip[1]*2;

	sop = sboxes(sbno,x,y);
	for(;sop!=0;sop/=2)
		p[i--]=sop%2;
}
void fun_k(int round)
{
	int i,epd[WORDSZ];
	int lxor[4],rxor[4];
	int p[4]={0},np[4];
	EP(r,epd);					

	for(i=0;i<4;i++)
	{	
		lxor[i] = epd[i]^keys[round][i]; 
		rxor[i] = epd[i+4]^keys[round][i+4];
	}		
	
	s_box(lxor,p,0,1);
	s_box(rxor,p,1,3);

	P4(p,np);
		
	for(i=0;i<4;i++)
		l[i] = l[i]^np[i];
}
void left_shift(int *keyip,int x)
{
	int t1,t2,i;
	while(x>0)
	{
		t1=keyip[0];
		t2=keyip[5];
		for(i=0;i<4;i++)
		{
			keyip[i] =keyip[i+1];
			keyip[i+5] = keyip[i+6];	
		}		
		keyip[4]=t1,keyip[9]=t2;
		x--;
	}
}

void keygen(int *key)			/*generates K_1 and K_2*/
{
	int i,keyip[KEYSZ];

	P10(key, keyip);				/*P10 permutation*/
	
	left_shift(keyip,1);	 		/*generating key1*/
	P8(keyip, &(keys[0][0]));	
	
	left_shift(keyip,2);			/*generating key2*/
	P8(keyip, &(keys[1][0]));	
}

/* 
	c=0 encrypt
	c=1 decrypt
*/
void enc_decode(int *in,int *out,int c)
{
	int temp[8],i;

	key2ip(in,temp);
	
	for(i=0;i<4;i++){
		l[i]=temp[i];
		r[i]=temp[i+4];
	}
		
	fun_k(c);
	for(i=0;i<4;i++){
		r[i]=l[i]+r[i];
		l[i]=r[i]-l[i];
		r[i]=r[i]-l[i];
	}
	
	fun_k(!c); 
	for(i=0;i<4;i++)
	{
		temp[i]=l[i];
		temp[i+4]=r[i];
	}
	ip2key(temp,out);	
}

int main(int argc, char **argv){
	FILE* ifp, *ofp;
	unsigned char buff[BUFFSZ],buff2[BUFFSZ];
	int key[KEYSZ],iv[WORDSZ],pt[WORDSZ],ct[WORDSZ],a,h,i,r,decrypt=0;

	if(argc==6 && !strcmp(argv[1],"-d")){
		decrypt=1;
		argv++;
		argc--;
	}

	if(argc!=5)
		usage("mycipher [-d] <key> <init_vector> <input_file> <output_file>");	


	for(i=KEYSZ-1;i>=0;i--)
		key[i]=atoi((char*)(&argv[1]+i));

	for(i=0;i<WORDSZ;i++)
		iv[i]=atoi((char*)(&argv[2]+i));

	
	if((ifp=fopen(argv[3],"rb"))&&(ofp = fopen(argv[4],"wb")))		/*make sure input and output files are good to go*/
	{
	
		r=fread (buff, sizeof(char), BUFFSZ, ifp);
		
		for(h=0;h<r;h++)
		{

			if(!decrypt){						/*encode*/
				dec2bin(buff[h], pt);

				for(i=0;i<WORDSZ;i++)
					pt[i]=pt[i] ^ iv[i];

				keygen(key); 					/* Generating Keys key1 & key2*/
				enc_decode(pt,ct,0);
			
				for(i=0;i<WORDSZ;i++)		/*last ct is now current initial vector (CBC)*/
					iv[i]=ct[i];

				bin2dec(ct,buff2);

				fwrite (buff2, sizeof(char), 1, ofp);	
			}
			else{									/*decode*/

				keygen(key); 					/* Generating Keys key1 & key2*/
				dec2bin(buff[h], ct);
					
				enc_decode(ct,pt,1);

				for(i=0;i<WORDSZ;i++)
				pt[i]=pt[i] ^ iv[i];	
							
				for(i=0;i<WORDSZ;i++)		/*last ct is now current initial vector (CBC)*/
				iv[i]=ct[i];
											
				bin2dec(pt, buff2);
				fwrite (buff2, sizeof(char), 1, ofp);		

			}		
				
		}/*end for*/
		fclose(ifp);
		fclose(ofp);
	}
	else
		perror("Error, check input/output file");
	return 0;
	
}

