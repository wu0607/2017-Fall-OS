// Student ID:
// Name      :
// Date      : 2017.11.03

#include "bmpReader.h"
#include "bmpReader.cpp"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

#define MYRED	2
#define MYGREEN 1
#define MYBLUE	0

#define NUM_THREADS_1 1
#define NUM_THREADS_2 10

//mutex
pthread_mutex_t Mutex;

int imgWidth, imgHeight;
int FILTER_SIZE;
//int FILTER_SCALE;
int *filter_GX, *filter_GY;

const char *inputfile_name[5] = {
	"input1.bmp",
	"input2.bmp",
	"input3.bmp",
	"input4.bmp",
	"input5.bmp"
};

const char *outputSobel_name[5] = {
	"Sobel1.bmp",
	"Sobel2.bmp",
	"Sobel3.bmp",
	"Sobel4.bmp",
	"Sobel5.bmp"
};

unsigned char *pic_in, *pic_grey, *pic_gx, *pic_gy, *pic_final;

unsigned char RGB2grey(int w, int h)
{
	int tmp = (
		pic_in[3 * (h*imgWidth + w) + MYRED] +
		pic_in[3 * (h*imgWidth + w) + MYGREEN] +
		pic_in[3 * (h*imgWidth + w) + MYBLUE] )/3;

	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

unsigned char SobelFilter(int w, int h, int*filter_G)
{
	int tmp = 0;
	//int tmpx, tmpy = 0;
	int a, b;
	int ws = (int)sqrt((float)FILTER_SIZE);
	for (int j = 0; j<ws; j++)
	for (int i = 0; i<ws; i++)
	{
		a = w + i - (ws / 2);
		b = h + j - (ws / 2);

		// detect for borders of the image
		if (a<0 || b<0 || a>=imgWidth || b>=imgHeight) continue;

		tmp += filter_G[j*ws + i] * pic_grey[b*imgWidth + a];
	};
	//tmp /= FILTER_SCALE;
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}
void *thr_func_1(void *arg) {
	//int* ptr = &a;
	//int *index = (int *)arg;	
	int index = (long)arg;
	int start = imgHeight*(index) / NUM_THREADS_1 ;
	int end = imgHeight*(index+1) / NUM_THREADS_1 ;
	pthread_mutex_lock(&Mutex); 
	//convert RGB image to grey image
	for (int j = 0; j<imgHeight; j++) {
		for (int i = 0; i<imgWidth; i++){
			pic_grey[j*imgWidth + i] = RGB2grey(i, j);
		}
	}
	pthread_mutex_unlock(&Mutex); 
	//cout << "hello from thr_func, thread id: " << index << endl;//*arg
	//cout<<"done convert to Grey"<<endl;
	pthread_exit(NULL);
}
void *thr_func_2(void *arg) {
	
	int index = (long)arg;
	int start = imgHeight*(index) / NUM_THREADS_2 ;
	int end = imgHeight*(index+1) / NUM_THREADS_2 ;
	//sem_wait(&bin_sem);
	//cout<<"start convolve"<<endl;
	//apply the Gaussian filter to the image
	for (int j = start; j<end; j++) {
		for (int i = 0; i<imgWidth; i++){
			pic_gx[j*imgWidth + i] = SobelFilter(i, j, filter_GX);
			pic_gy[j*imgWidth + i] = SobelFilter(i, j, filter_GY);
		}
	}
	pthread_exit(NULL);
}
int main()
{
	//thread
	pthread_t thr_1[NUM_THREADS_1];
	pthread_t thr_2[NUM_THREADS_2];
	int thr_data_1[NUM_THREADS_1];
	int thr_data_2[NUM_THREADS_2];
	int rc;
	// read mask file
	FILE* mask;
	mask = fopen("mask_Sobel.txt", "r");
	fscanf(mask, "%d", &FILTER_SIZE);
	//fscanf(mask, "%d", &FILTER_SCALE);

	filter_GX = new int[FILTER_SIZE];
	filter_GY = new int[FILTER_SIZE];
	for (int i = 0; i<FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_GX[i]);
	//cout<<"HI";
	for (int i = 0; i<FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_GY[i]);
	fclose(mask);

	/*for (int i = 0 ; i < FILTER_SIZE ; ++i)
		cout << filter_GX[i] << ' ' ;
	for (int i = 0 ; i < FILTER_SIZE ; ++i)
		cout << filter_GY[i] << ' ' ;*/
	

	BmpReader* bmpReader = new BmpReader();
	for (int k = 0; k<5; k++){
		// read input BMP file
		pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);
		// allocate space for output image
		pic_grey = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_gx = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_gy = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_final = (unsigned char*)malloc(3 * imgWidth*imgHeight*sizeof(unsigned char));
		
		
		//thread_1
		for (int i = 0; i < NUM_THREADS_1; ++i) {
			thr_data_1[i] = i;
			//If successful, the pthread_create() function returns zero.
			//Otherwise, an error number is returned to indicate the error.
			if ( (rc = pthread_create( &thr_1[i], NULL, thr_func_1, (void*)thr_data_1[i] )) ) {//return 0 if success
				fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
				return EXIT_FAILURE;
			}
		}	
		for (int i = 0; i < NUM_THREADS_1; ++i) {
			pthread_join(thr_1[i], NULL);
			//cout<<"join: "<<i<<endl;
		}
		//cout<<"start thread2\n";
		//thread_2
		
		for (int i = 0; i < NUM_THREADS_2; ++i) {
			thr_data_2[i] = i;
			if ( (rc = pthread_create( &thr_2[i], NULL, thr_func_2, (void*)thr_data_2[i] )) ) {//return 0 if success
				fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
				return EXIT_FAILURE;
			}
		}		
		//pthread_mutex_lock(&Mutex); 
		for (int i = 0; i < NUM_THREADS_2; ++i) {
			pthread_join(thr_2[i], NULL);
		}
		//pthread_mutex_unlock(&Mutex); 
		//extend the size form WxHx1 to WxHx3
		for (int j = 0; j<imgHeight; j++) {
			for (int i = 0; i<imgWidth; i++){
			unsigned temp = sqrt((unsigned)pic_gx[j*imgWidth + i]*(unsigned)pic_gx[j*imgWidth + i] 
													+ (unsigned)pic_gy[j*imgWidth + i]*(unsigned)pic_gy[j*imgWidth + i]);
			//temp = sqrt(temp);
			if (temp < 0) temp = 0;
			if (temp > 255) temp = 255;
			pic_final[3 * (j*imgWidth + i) + MYRED] = (unsigned char)temp;
			pic_final[3 * (j*imgWidth + i) + MYGREEN] = (unsigned char)temp;
			pic_final[3 * (j*imgWidth + i) + MYBLUE] = (unsigned char)temp;
		}
	}
		// write output BMP file
		bmpReader->WriteBMP(outputSobel_name[k], imgWidth, imgHeight, pic_final);

		//free memory space
		free(pic_in);
		free(pic_grey);
		free(pic_gx);
		free(pic_gy);
		free(pic_final);
	}

	return 0;
}
