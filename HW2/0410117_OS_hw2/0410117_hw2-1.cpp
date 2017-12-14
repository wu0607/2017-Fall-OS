# include <iostream>
# include <fstream>
# include <vector>
using namespace std;

int main(){
	int num_process;
	double avg_waiting = 0, avg_turnaround = 0;
	vector<int> burst_time, waiting_time, turnaround_time;
	ifstream ReadFile;
	//ofstream OutputFile;
	//OutputFile.open("Output.txt");
	ReadFile.open("Q1.txt");
	
	if(ReadFile.is_open()){
		ReadFile >> num_process;
		//burst_time.resize(num_process);
		while(!ReadFile.eof()){
			double burst;
			ReadFile >> burst;
			burst_time.push_back(burst);
		}
	}
	waiting_time.resize(num_process);
	turnaround_time.resize(num_process);
	//waiting_time
	for(int i=0 ; i<num_process ; i++){
		if( i==0 ){
			waiting_time[i] = 0;
		}
		else{
			waiting_time[i] = waiting_time[i-1] + burst_time[i-1];
		}
	}
	//turnaround_time
	for(int i=0 ; i<num_process ; i++){
		if( i==0 ){
			turnaround_time[i] = burst_time[i];
		}
		else{
			turnaround_time[i] = waiting_time[i] + burst_time[i];
		}
	}
	//avg_waiting
	for(int i=0 ; i<num_process ; i++){
		avg_waiting += waiting_time[i];
	}
	avg_waiting = avg_waiting/num_process;
	//avg_turnaround
	for(int i=0 ; i<num_process ; i++){
		avg_turnaround += turnaround_time[i];
	}
	avg_turnaround = avg_turnaround/num_process;
	//output	
	/*
	OutputFile << "Questioin 2-1" << endl;
	OutputFile << "Process" <<"\t\t"<< "Waiting time" <<"	"<< "Turnaround Time" << endl;
	for(int i=0 ; i<num_process ; i++){
		OutputFile << "p[" << i+1 << "]\t\t" << waiting_time[i] <<"\t\t\t\t"<< turnaround_time[i] << endl;
	}
	OutputFile << "Average waiting time : " << avg_waiting << endl;
	OutputFile << "Average turnaround time : " << avg_turnaround << endl;
	
	OutputFile.close();*/

	cout << "Questioin 2-1" << endl;
	cout << "Process" <<"\t\t"<< "Waiting time" <<"	"<< "Turnaround Time" << endl;
	for(int i=0 ; i<num_process ; i++){
		cout << "p[" << i+1 << "]\t\t" << waiting_time[i] <<"\t\t\t\t"<< turnaround_time[i] << endl;
	}
	cout << "Average waiting time : " << avg_waiting << endl;
	cout << "Average turnaround time : " << avg_turnaround << endl;

	
	return 0;
}