//shortest remaining time first, preemptive
# include <iostream>
# include <fstream>
# include <vector>

using namespace std;

int main(){
	int num_process, total_time=0;
	double avg_waiting = 0, avg_turnaround = 0, sum_waiting=0, sum_turnaround=0;
	vector<int> order, arrival_time, burst_time, remaining_time,waiting_time, turnaround_time;
	//vector<bool> done;
	ifstream ReadFile;
	ofstream OutputFile;
	OutputFile.open("Output.txt");
	ReadFile.open("Q3.txt");
	
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
				remaining_time.push_back(burst);
				total_time += burst;
			}
		}
	}
	
	waiting_time.resize(num_process);
	turnaround_time.resize(num_process);
	order.resize(num_process);
	for(int i=0 ; i<num_process ; ++i){
		order[i] = i+1;
	}
	for(int i=0 ; i<num_process ; ++i){
		for(int j=i+1 ; j<num_process ; ++j){
			if(arrival_time[i]>arrival_time[j]){
				swap(order[i], order[j]);
				swap(arrival_time[i], arrival_time[j]);
				swap(burst_time[i], burst_time[j]);
				swap(remaining_time[i],remaining_time[j]);
			}
			if((arrival_time[i]==arrival_time[j]) && (burst_time[i]>burst_time[j])){
				swap(order[i], order[j]);
				swap(arrival_time[i], arrival_time[j]);
				swap(burst_time[i], burst_time[j]);
				swap(remaining_time[i],remaining_time[j]);
			}
		}
	}
	for(int i=0 ; i<num_process ; ++i){
		for(int j=i+1 ; j<num_process ; ++j){
			if(arrival_time[i]==arrival_time[j]&&burst_time[i]==burst_time[j] && order[i]>order[j]){
				swap(order[i], order[j]);
				swap(arrival_time[i], arrival_time[j]);
				swap(burst_time[i], burst_time[j]);
				swap(remaining_time[i],remaining_time[j]);
			}
		}
	}

	remaining_time.push_back(9999);
	//sort to the output sequence order
	int SR_process_index = 0;
	for(int current_time=0, finish_process=0; current_time<total_time ; ++current_time){	

		int smallest = num_process;
        for(int i=0;i<num_process;++i)
        {
            /*choose the smallest remaining time target*/
            if ((arrival_time[i] <= current_time)                              
                && (remaining_time[i] < remaining_time[smallest]) 
                && (remaining_time[i] > 0))
                smallest = i;
        }




		/*for(int i=0 ; i<num_process ; ++i){		
			while(remaining_time[SR_process_index]==0){
				if(enter==false){
					SR_process_index = 0;
				}
				else{
					enter=true;
					SR_process_index = (SR_process_index+1)%num_process;
				}
				
			}
			if(arrival_time[i]<=current_time && remaining_time[i]<remaining_time[SR_process_index] && remaining_time[i]>0 && remaining_time[SR_process_index]>0){
				SR_process_index = i;
			}
			//if((order[i]<order[j] &&arrival_time[i]==arrival_time[j])||)
		}*/
		--remaining_time[smallest];
		/*
		for(int i=0 ; i<num_process ; ++i){
		cout<<"order "<<order[i]<<"\t";
		}
		cout<<endl;
		for(int i=0 ; i<num_process ; ++i){
		cout<<"remai "<<remaining_time[i]<<" ";
		}
		//cout<<"current_time:"<<current_time<<"	SR_process_index: "<<SR_process_index<<endl;
		cout<<"current_time:"<<current_time<<"	order: "<<order[smallest]<<endl;
		*/
		if(remaining_time[smallest] == 0){		
			int end_time = current_time + 1;
			++finish_process;								
			//calaulate turnaround time first
			turnaround_time[smallest] = end_time - arrival_time[smallest];
			//then we can get waiting_time
			waiting_time[smallest] = turnaround_time[smallest] - burst_time[smallest];
			
			//turn to the next process, initialize the SR_index for the next round of"for loop"
			//SR_process_index = (SR_process_index+1)%num_process;
			
			//for(int i=0 ; i<num_process ; ++i){
			//	cout<<turnaround_time[i]<<" ";
			//}
			//cout<<endl;
		}	
		
	}
	for(int i=0 ; i<num_process ; ++i){
		for(int j=i+1 ; j<num_process ; ++j){
			if(order[i]>order[j]){
				swap(order[i], order[j]);
				swap(arrival_time[i], arrival_time[j]);
				swap(burst_time[i], burst_time[j]);
				swap(remaining_time[i],remaining_time[j]);
				swap(waiting_time[i],waiting_time[j]);
				swap(turnaround_time[i],turnaround_time[j]);
			}
		}
	}
	
	cout << "Questioin 2-3" << endl;
	cout << "Process" <<"\t\t"<< "Waiting time" <<"	"<< "Turnaround Time" << endl;
	for(int i=0 ; i<num_process ; i++){
		cout << "p[" << i+1 << "]\t\t" << waiting_time[i] <<"\t\t\t\t"<< turnaround_time[i] << endl;
	}
	
	for(int i=0 ; i<num_process ; i++){
		sum_waiting += waiting_time[i];
	}
	for(int i=0 ; i<num_process ; i++){
		sum_turnaround += turnaround_time[i];
	}
	//Output
	cout << "Average waiting time : " << sum_waiting/num_process << endl;
	cout << "Average turnaround time : " << sum_turnaround/num_process << endl;
	

	
	return 0;
}