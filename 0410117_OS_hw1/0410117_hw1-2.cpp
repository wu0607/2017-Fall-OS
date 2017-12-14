# include <iostream>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/wait.h>
# include <sys/shm.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/time.h>

# include <stdint.h>

using namespace std;

unsigned int* attachShareMem(int shmID);
//void* attachShareMem(int shmID);
void initMem(int dim);
void detachShareMem();
void detachAndReleaseShareMem();
unsigned int process_1(int dim);
unsigned int process_4(int dim);

unsigned int *A, *B, *C;
//global variables for share memory
int shmIDA, shmIDB, shmIDC;

int main(){
	int dim;
	unsigned int sum=0;
	struct timeval start, end;
	int sec, usec;
	
	key_t key = 0;
	int shmflg = IPC_CREAT|0666;
	
	cout << "Dimension: ";
	cin >> dim;
	
	//creat the block of shareMem, shmget returms its ID
	if( (shmIDA = shmget(key, sizeof(unsigned int)*dim*dim, shmflg)) == -1 ){
		cout << "shmgetA failed" << endl;
	}
	if( (shmIDB = shmget(key, sizeof(unsigned int)*dim*dim, shmflg)) == -1 ){
		cout << "shmgetB failed" << endl;
	}
	if( (shmIDC = shmget(key, sizeof(unsigned int)*dim*dim, shmflg)) == -1 ){
		cout << "shmgetC failed" << endl;
	}
	A = attachShareMem(shmIDA);
	B = attachShareMem(shmIDB);
	C = attachShareMem(shmIDC);
	
	initMem(dim);
	
	//1 process
	gettimeofday(&start, 0);
	sum = process_1(dim);
	cout << "1-process, checksum = " << sum << endl;
	gettimeofday(&end, NULL);
	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	cout << "elapsed " << (sec + usec/1000000.0) << "  s" << endl;
	
	
	//4 process
	gettimeofday(&start, 0);
	sum = process_4(dim);
	cout << "4-process, checksum = " << sum << endl;
	gettimeofday(&end, NULL);
	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	cout << "elapsed " << (sec + usec/1000000.0) << "  s" << endl;
	
	
	return 0;
}
//attach share memory to current process's address space
unsigned int* attachShareMem(int shmID){
	unsigned int *X;	//A.B.C
	if( (X = (unsigned int*)shmat(shmID, NULL, 0)) == (unsigned int*)(-1) ){
		cout << "shmat failed";
	}
	return X;	
}
//set A.B's values
void initMem(int dim){
	int i;
	for( i=0 ; i<(dim*dim) ; ++i){
		A[i] = i;
		B[i] = i;
		C[i] = 0;//initialize C[]
	}
}
//detach memory after child process is over
void detachShareMem(){
	shmdt(A);
	shmdt(B);
	shmdt(C);
}
//detach memory after parent process is over
void detachAndReleaseShareMem(){
	detachShareMem();
	shmctl(shmIDA, IPC_RMID, 0);//IPC_RMID: marking sharemem to be deleated
	shmctl(shmIDB, IPC_RMID, 0);
	shmctl(shmIDC, IPC_RMID, 0);
}
//1-process
unsigned int process_1(int dim){
	unsigned int sum=0;
	for( int i=0 ; i<dim ; ++i ){
		for( int j=0 ; j<dim ; ++j ){
			for( int k=0 ; k<dim ; ++k ){
				sum += A[i*dim+k] * B[k*dim+j];
			}
		}
	}
	return sum;
}
//4-process
unsigned int process_4(int dim){
	unsigned int sum=0;
	pid_t pid1;
	//fork1
	//child
	if( (pid1=fork()) == 0){
		pid_t pid2;
		//fork2
		if( (pid2=fork()) ==0){
			for( int i=0 ; i<dim/2 ; ++i ){
				for( int j=0 ; j<dim/2 ; ++j ){
					for( int k=0 ; k<dim ; ++k){
						C[i*dim+j] += A[i*dim+k] * B[k*dim+j];
					}
				}
			}
			detachShareMem();
			exit(0);
		}
		else{
			for( int i=dim/2 ; i<dim ; ++i ){
				for( int j=0 ; j<dim/2 ; ++j ){
					for( int k=0 ; k<dim ; ++k){
						C[i*dim+j] += A[i*dim+k] * B[k*dim+j];
					}
				}
			}
			wait(NULL);
		}
		detachShareMem();
		exit(0);
	}
	//parent
	else{
		pid_t pid3;
		//fork3
		if( (pid3=fork()) ==0){
			for( int i=0 ; i<dim/2 ; ++i ){
				for( int j=dim/2 ; j<dim ; ++j ){
					for( int k=0 ; k<dim ; ++k){
						C[i*dim+j] += A[i*dim+k] * B[k*dim+j];
					}
				}
			}
			detachShareMem();
			exit(0);
		}
		else{
			for( int i=dim/2 ; i<dim ; ++i ){
				for( int j=dim/2 ; j<dim ; ++j ){
					for( int k=0 ; k<dim ; ++k){
						C[i*dim+j] += A[i*dim+k] * B[k*dim+j];
					}
				}
			}
			wait(NULL);
			
		}
		wait(NULL);
		for( int i=0 ; i<dim*dim ; ++i){
			sum += C[i];
		}		
		detachAndReleaseShareMem();
	}
	return sum;
}
