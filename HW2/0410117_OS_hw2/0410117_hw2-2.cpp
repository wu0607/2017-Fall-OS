//shortest job first, non-preemptive
# include <iostream>
# include <fstream>
# include <vector>

using namespace std;

int main(){
	int num_process;
	double sum_waiting=0, sum_turnaround=0, start_time = 0;
	vector<int> order, arrival_time, burst_time, waiting_time, turnaround_time;
	//vector<bool> done;
	ifstream ReadFile;
	//ofstream OutputFile;
	//OutputFile.open("Output.txt");
	ReadFile.open("Q2.txt");
	
	if(ReadFile.is_open()){
		ReadFile >> num_process;
		int num = 1;
		while(!ReadFile.eof()){
			if( num <= num_process){
				double arrival;
				ReadFile >> arrival;
				arrival_time.push_back(arrival);
				++num;
			}
			else{
				double burst;
				ReadFile >> burst;
				burst_time.push_back(burst);
			}
		}
	}
	start_time = arrival_time[0];
	waiting_time.resize(num_process);
	turnaround_time.resize(num_process);
	order.resize(num_process);
	for(int i=0 ; i<num_process ; ++i){
		order[i] = i+1;
	}
	//sort by arrival_time; if arrival at same time, put lower burst_time in front
	for(int i=0 ; i<num_process ; ++i){
		for(int j=i+1 ; j<num_process ; ++j){
			if(arrival_time[i]>arrival_time[j]){
				swap(order[i], order[j]);
				swap(arrival_time[i], arrival_time[j]);
				swap(burst_time[i], burst_time[j]);
			}
			if((arrival_time[i]==arrival_time[j]) && (burst_time[i]>burst_time[j])){
				swap(order[i], order[j]);
				swap(arrival_time[i], arrival_time[j]);
				swap(burst_time[i], burst_time[j]);
			}
		}
	}
	/*
	for(int i=0 ; i<num_process ; ++i){
		cout<<order[i]<<" ";
	}
	cout<<endl;
	for(int i=0 ; i<num_process ; ++i){
		cout<<arrival_time[i]<<" ";
	}
	cout<<endl;
	for(int i=0 ; i<num_process ; ++i){
		cout<<burst_time[i]<<" ";
	}
	cout<<endl;
	cout << "####" << "\n";
	*/
	//sort to the output sequence order
	for(int i=0, total_burst=0 ; i<num_process ; ++i){
		total_burst += burst_time[i];
		for(int j=i+1 ; j<num_process ; ++j){
			if((arrival_time[j]<= total_burst) && ((burst_time[j]<burst_time[i+1]) || arrival_time[j]<arrival_time[i+1] || 
				( order[j] < order[i+1] &&burst_time[j]==burst_time[i+1] && arrival_time[j]==arrival_time[i+1]))){
				//cout << arrival_time[j] << " , " << total_burst << " , " << burst_time[j] << " , " << burst_time[i+1] << endl;
				//cout << "swap " << i+1 << "," << j << endl;
				swap(order[i+1], order[j]);
				swap(arrival_time[i+1], arrival_time[j]);
				swap(burst_time[i+1], burst_time[j]);
			}

		}
	}
	/*
	for(int i=0 ; i<num_process ; ++i){
		cout<<order[i]<<" ";
	}
	cout<<endl;
	for(int i=0 ; i<num_process ; ++i){
		cout<<arrival_time[i]<<" ";
	}
	cout<<endl;
	for(int i=0 ; i<num_process ; ++i){
		cout<<burst_time[i]<<" ";
	}
	cout<<endl;
	*/
	//waiting_time
	for(int i=0, total_waiting=0 ; i<num_process ; ++i){
		if( i==0 ){
			waiting_time[i] = 0; 
		}
		else{
			total_waiting = total_waiting + burst_time[i-1];
			waiting_time[i] = total_waiting - arrival_time[i] + start_time;
			sum_waiting += waiting_time[i];
		}
	}
	//turnaround_time
	for(int i=0 ; i<num_process ; ++i){
		turnaround_time[i] = waiting_time[i] + burst_time[i];
		sum_turnaround += turnaround_time[i];
	}
	//return to the original order
	for(int i=0 ; i<num_process ; ++i){
		for(int j=i+1 ; j<num_process ; ++j){
			if(order[i] > order[j]){
				swap(order[i], order[j]);
				swap(arrival_time[i], arrival_time[j]);
				swap(burst_time[i], burst_time[j]);
				swap(waiting_time[i], waiting_time[j]);
				swap(turnaround_time[i], turnaround_time[j]);
			}
		}
		
	}
	cout << "Questioin 2-2" << endl;
	cout << "Process" <<"\t\t"<< "Waiting time" <<"	"<< "Turnaround Time" << endl;
	for(int i=0 ; i<num_process ; i++){
		cout << "p[" << i+1 << "]\t\t" << waiting_time[i] <<"\t\t\t\t"<< turnaround_time[i] << endl;
	}
	cout << "Average waiting time : " << sum_waiting/num_process << endl;
	cout << "Average turnaround time : " << sum_turnaround/num_process << endl;
	
	
	return 0;
}