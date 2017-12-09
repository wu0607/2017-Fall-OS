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

#define NUM_THREADS_1 5
#define NUM_THREADS_2 10
//semaphore
sem_t bin_sem;

int imgWidth, imgHeight;
int FILTER_SIZE;
int FILTER_SCALE;
int *filter_G;

const char *inputfile_name[5] = {
	"input1.bmp",
	"input2.bmp",
	"input3.bmp",
	"input4.bmp",
	"input5.bmp"
};
const char *outputBlur_name[5] = {
	"Blur1.bmp",
	"Blur2.bmp",
	"Blur3.bmp",
	"Blur4.bmp",
	"Blur5.bmp"
};
/*
const char *outputSobel_name[5] = {
	"Sobel1.bmp",
	"Sobel2.bmp",
	"Sobel3.bmp",
	"Sobel4.bmp",
	"Sobel5.bmp"
};*/

unsigned char *pic_in, *pic_grey, *pic_blur, *pic_final;

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

unsigned char GaussianFilter(int w, int h)
{
	int tmp = 0;
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
	tmp /= FILTER_SCALE;
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}
void *thr_func_1(void *arg) {
	//int* ptr = &a;
	//int *index = (int *)arg;
	int index = (long)arg;
	int start = imgHeight *(index) / NUM_THREADS_1 ;
	int end = imgHeight * (index+1) / NUM_THREADS_1 ;
	//cout<<"1"<<endl;
	//convert RGB image to grey image	
	for (int j = start; j<end; j++) {
			for (int i = 0; i<imgWidth; i++){
				pic_grey[j*imgWidth + i] = RGB2grey(i, j);
			}
	}
	//cout << "hello from thr_func, thread id: " << index << endl;//*arg
	//cout<<"done convert to Grey"<<endl;
	sem_post(&bin_sem);
	pthread_exit(NULL);
}
void *thr_func_2(void *arg) {
	int index = (long)arg;
	int start = imgHeight*(index) / NUM_THREADS_2 ;
	int end = imgHeight*(index+1) / NUM_THREADS_2 ;
	//cout<<"3"<<endl;
	//sem_wait(&bin_sem);
	//cout<<"start convolve"<<endl;
	//apply the Gaussian filter to the image
	for (int j = start; j<end; j++) {
		for (int i = 0; i<imgWidth; i++){
			pic_blur[j*imgWidth + i] = GaussianFilter(i, j);
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
	//semaphore
	int res;
	res = sem_init(&bin_sem, 0, 0);
	
	if (res != 0){
		perror("Semaphore initialization failed");
	}
	
	// read mask file
	FILE* mask;
	mask = fopen("mask_Gaussian.txt", "r");
	fscanf(mask, "%d", &FILTER_SIZE);
	fscanf(mask, "%d", &FILTER_SCALE);

	filter_G = new int[FILTER_SIZE];
	for (int i = 0; i<FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_G[i]);
	fclose(mask);


	BmpReader* bmpReader = new BmpReader();
	for (int k = 0; k<5; k++){
		// read input BMP file
		pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);
		// allocate space for output image
		pic_grey = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_blur = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
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
		}
		
		//thread_2
		sem_wait(&bin_sem);
		for (int i = 0; i < NUM_THREADS_2; ++i) {
			thr_data_2[i] = i;
			if ( (rc = pthread_create( &thr_2[i], NULL, thr_func_2, (void*)thr_data_2[i] )) ) {//return 0 if success
				fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
				return EXIT_FAILURE;
			}
		}		
		for (int i = 0; i < NUM_THREADS_2; ++i) {
			pthread_join(thr_2[i], NULL);
		}
		

		//extend the size form WxHx1 to WxHx3
		for (int j = 0; j<imgHeight; j++) {
			for (int i = 0; i<imgWidth; i++){
				pic_final[3 * (j*imgWidth + i) + MYRED] = pic_blur[j*imgWidth + i];
				pic_final[3 * (j*imgWidth + i) + MYGREEN] = pic_blur[j*imgWidth + i];
				pic_final[3 * (j*imgWidth + i) + MYBLUE] = pic_blur[j*imgWidth + i];
			}
		}

		// write output BMP file
		bmpReader->WriteBMP(outputBlur_name[k], imgWidth, imgHeight, pic_final);

		//free memory space
		free(pic_in);
		free(pic_grey);
		free(pic_blur);
		free(pic_final);
	}

	return 0;
}