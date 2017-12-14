# include <sys/wait.h>
# include <iostream>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>


using namespace std;

int main(){
	cout << "Main process ID : " << getpid() << endl << endl;
	pid_t pid1,pid2,pid3,pid4,pid5;
	pid_t p1,p2,p3,p4,p5;
	//start fork1
	//child
	if( (pid1=fork()) == 0){
		cout << "Fork 1. I'm the child " << getpid() << ", my parent is " << getppid() << "." <<endl;
		//start fork2
		//child
		if( (pid2=fork()) == 0){
			cout << "Fork 2. I'm the child " << getpid() << ", my parent is " << getppid() << "." <<endl;	
			//start fork3
			usleep(1);
			if( (pid3=fork()) == 0){
				cout << "Fork 3. I'm the child " << getpid() << ", my parent is " << getppid() << "." <<endl;
				exit(0);//every child need to do exit in the end
			}
			else{
				//every parent should wait for child
				pid3 = wait(NULL);
				
			}
			exit(0);
			
		}
		//parent
		else{			
			//pid2 = wait(NULL);
			if( (pid4 = fork()) == 0){
				cout << "Fork 2. I'm the child " << getpid() << ", my parent is " << getppid() << "." <<endl;			
				//start fork3
				usleep(100);
				if( (pid5=fork()) == 0){
					cout << "Fork 3. I'm the child " << getpid() << ", my parent is " << getppid() << "." <<endl;
					exit(0);
				}
				else{					
					pid5 = wait(NULL);
				}
				exit(0);
			}
			else{
				pid4 = wait(NULL);
			}
			pid2 = wait(NULL);
		}
	exit(0);
	}
	//parent
	else{
		pid1 = wait(NULL); 		
	}
	
	return 0;
}
