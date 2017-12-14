//multi-level feedback queue(RR+RR+SJF)
# include <iostream>
# include <fstream>
# include <vector>
# include <deque>

using namespace std;
int RR(int time, int time_quantum);
void SJF(int time);

int num_process, quantum_1=0, quantum_2;
double sum_waiting=0, sum_turnaround=0;
vector<int> order, arrival_time, burst_time, remaining_time, waiting_time, turnaround_time;


int find_index(int pid, int num_process)
{
    int index = -1;
    for(int i=0;i<num_process;++i)
    {
        if(order[i] == pid)
            index = i;
    }
    return index;
}


int main(){
	int current_time=0;
	ifstream ReadFile;
	//ofstream OutputFile;
	//OutputFile.open("Output.txt");
	ReadFile.open("Q4.txt");
	
	if(ReadFile.is_open()){
		ReadFile >> num_process;
		int num_1 = 1, num_2 = 1;
		while(!ReadFile.eof()){
			if( num_1 <= num_process){
				double arrival;
				ReadFile >> arrival;
				arrival_time.push_back(arrival);
				++num_1;
			}
			else if(num_2 <= num_process){
				double burst;
				ReadFile >> burst;
				burst_time.push_back(burst);
				remaining_time.push_back(burst);
				++num_2;
			}
			else if(quantum_1==0){
				ReadFile >> quantum_1;
			}
			else{
				ReadFile >> quantum_2;
			}
		}
	}
	waiting_time.resize(num_process);
	turnaround_time.resize(num_process);
	order.resize(num_process);
	for(int i=0 ; i<num_process ; ++i){
		order[i] = i;
	}	
	for(int i=0 ; i<num_process ; ++i){
		for(int j=i+1 ; j<num_process ; ++j){
			if(arrival_time[i]>arrival_time[j]){
				swap(order[i], order[j]);
				swap(arrival_time[i], arrival_time[j]);
				swap(burst_time[i], burst_time[j]);
				swap(remaining_time[i], remaining_time[j]);
			}
		}
	}
	for(int i=0 ; i<num_process ; ++i){
		for(int j=i+1 ; j<num_process ; ++j){
			if(arrival_time[i]==arrival_time[j] && burst_time[i]==burst_time[j]&&order[i]>order[j]){
				swap(order[i], order[j]);
				swap(arrival_time[i], arrival_time[j]);
				swap(burst_time[i], burst_time[j]);
				swap(remaining_time[i], remaining_time[j]);
			}
		}
	}

	//for (int i = 0; i < num_process ; ++i)
	//	cout << arrival_time[i] << endl;



	deque<int> Q1, Q2, Q3;
	Q1.push_back(order[0]);

	int last_time = -100;
	int current = 0;
	int finished = 0;
	for(int current_time = arrival_time[0]; finished != num_process ; ){
		//cout << "time: " << current_time << endl;
		for(int i=current+1 ; i<num_process ; ++i){
			//check if there is new process wants to enter
			//cout<<current_time<<"---"<<arrival_time[i]<<endl;
			if(arrival_time[i]<=current_time && arrival_time[i] > last_time){
				Q1.push_back(order[i]);
				//cout << "insert order into Q1: " <<order[i]<< endl;
			}
		}

		last_time = current_time;

		if(Q1.empty()==0){
			int pid = Q1.front();
			Q1.pop_front();
			int index = find_index(pid, num_process);
			current = index;

			//cout << "Q1 processing pid:" << pid << endl;

			if(remaining_time[index] <= quantum_1 && remaining_time[index]>0){
				current_time += remaining_time[index];
				remaining_time[index] = 0;
			}
			else if(remaining_time[index]>0){
				remaining_time[index] -= quantum_1;
				current_time += quantum_1;
				Q2.push_back(order[index]);
				//cout << "insert order into Q2: " <<order[index]<< endl;
			}

			if(remaining_time[index] == 0){
				++finished;
				turnaround_time[index] = current_time - arrival_time[index];
				waiting_time[index] = turnaround_time[index] - burst_time[index];
				sum_waiting += waiting_time[index];
				sum_turnaround += turnaround_time[index];
			}

		}
		else if(Q1.empty()==1 && Q2.empty()==0){
			int pid = Q2.front();
			Q2.pop_front();
			int index = find_index(pid, num_process);
			//cout << "Q2 processing pid:" << pid << endl;

			if(remaining_time[index] <= quantum_2 && remaining_time[index]>0){
				current_time += remaining_time[index];
				remaining_time[index] = 0;
			}
			else if(remaining_time[index]>0){
				remaining_time[index] -= quantum_2;
				current_time += quantum_2;
				Q3.push_back(order[index]);
				//cout << "insert order into Q3: " <<order[index]<< endl;
			}

			if(remaining_time[index] == 0){
				++finished;
				turnaround_time[index] = current_time - arrival_time[index];
				waiting_time[index] = turnaround_time[index] - burst_time[index];
				sum_waiting += waiting_time[index];
				sum_turnaround += turnaround_time[index];
			}

		}
		else if(Q1.empty()==1 && Q2.empty()==1 && Q3.empty()==0){
			int target = 0;
			int min = 9999;
			int position = 0;
			for(int i=0 ; i<Q3.size() ; ++i){
				int index = find_index(Q3[i], num_process);
				if(burst_time[index] < min){
					min = burst_time[index];
					target = index;
					position = i;
				}
			}
			//cout << "Q3 processing pid:" << order[target] << endl;
			Q3.erase(Q3.begin()+position);
			finished++;

			current_time += remaining_time[target];
			remaining_time[target] = 0;

			turnaround_time[target] = current_time - arrival_time[target];
			waiting_time[target] = turnaround_time[target] - burst_time[target];

			sum_waiting += waiting_time[target];
			sum_turnaround += turnaround_time[target];


		}

	}



	/*
	current_time = arrival_time[0];
	current_time = RR(current_time, quantum_1);
	//cout<<"1_time"<<current_time<<endl;
	current_time = RR(current_time, quantum_2);
	//cout<<"2_time"<<current_time<<endl;
	SJF(current_time);*/
	
	
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
	//Output
	cout << "Questioin 2-4" << endl;
	cout << "Process" <<"\t\t"<< "Waiting time" <<"	"<< "Turnaround Time" << endl;
	for(int i=0 ; i<num_process ; i++){
		cout << "p[" << i+1 << "]\t\t" << waiting_time[i] <<"\t\t\t\t"<< turnaround_time[i] << endl;
	}
	cout << "Average waiting time : " << sum_waiting/num_process << endl;
	cout << "Average turnaround time : " << sum_turnaround/num_process << endl;
	
	
	return 0;
}

int RR(int time,int time_quantum){
	int current_time = time;
	for(int i=0 ; i<num_process ; ++i){
		if(remaining_time[i] <= time_quantum && remaining_time[i]!=0){
			current_time += remaining_time[i];
			remaining_time[i] = 0;	
			turnaround_time[i] = current_time - arrival_time[i];
		}
		else if(remaining_time[i] > time_quantum){
			current_time += time_quantum;
			remaining_time[i] -= time_quantum;
		}
		
	}
	return current_time;
}

void SJF(int time){
	
	for(int i=0 ; i<num_process ; ++i){
		cout<<remaining_time[i]<<" ";
		}
		cout<<endl;
		for(int i=0 ; i<num_process ; ++i){
		cout<<turnaround_time[i]<<" ";
		}
		cout<<endl<<endl;//sort by arrival_time; if arrival at same time, put lower remaining_time in front
	//for(int i=0 ; i<num_process ; ++i){cout<<"order: "<<order[i]<<" ";}
	//cout<<"	remaining_time: ";
	//for(int i=0 ; i<num_process ; ++i){cout<<remaining_time[i]<<" ";}
	//cout<<"	turnaround_time: ";
	//for(int i=0 ; i<num_process ; ++i){cout<<turnaround_time[i]<<" ";}
	//cout<<"	waiting_time: ";
	//for(int i=0 ; i<num_process ; ++i){cout<<waiting_time[i]<<" ";}
	
	for(int i=0 ; i<num_process ; ++i){
		for(int j=i+1 ; j<num_process ; ++j){
			if(remaining_time[i] > remaining_time[j] && remaining_time[j]!=0){
				swap(order[i], order[j]);
				swap(arrival_time[i], arrival_time[j]);
				swap(remaining_time[i], remaining_time[j]);
				swap(burst_time[i], burst_time[j]);
			}
		}
	}
	
	//for(int i=0 ; i<num_process ; ++i){cout<<"order: "<<order[i]<<" ";}
	//cout<<"	remaining_time: ";
	//for(int i=0 ; i<num_process ; ++i){cout<<remaining_time[i]<<" ";}
	//cout<<"	turnaround_time: ";
	//for(int i=0 ; i<num_process ; ++i){cout<<turnaround_time[i]<<" ";}
	//cout<<"	waiting_time: ";
	//for(int i=0 ; i<num_process ; ++i){cout<<waiting_time[i]<<" ";}
	//sort to the output sequence order
	for(int i=0, total_burst=0 ; i<num_process ; ++i){
		total_burst += remaining_time[i];
		for(int j=i+1 ; j<num_process ; ++j){
			//if((arrival_time[j]<= total_burst) && (remaining_time[j]<remaining_time[i+1]) && remaining_time[j]!=0){
			if((arrival_time[j]<= total_burst) && 
				( ((remaining_time[j]<remaining_time[i+1]) && remaining_time[j]!=0 ) || 
				  ( order[j] < order[i+1] &&remaining_time[j]==remaining_time[i+1]) ) ){
				
				swap(order[i+1], order[j]);
				swap(arrival_time[i+1], arrival_time[j]);
				swap(remaining_time[i], remaining_time[j]);
				swap(burst_time[i+1], burst_time[j]);
			}
		}
	}
	
	//for(int i=0 ; i<num_process ; ++i){cout<<"order: "<<order[i]<<" ";}
	//cout<<"	remaining_time: ";
	//for(int i=0 ; i<num_process ; ++i){cout<<remaining_time[i]<<" ";}
	//cout<<"	turnaround_time: ";
	//for(int i=0 ; i<num_process ; ++i){cout<<turnaround_time[i]<<" ";}
	//cout<<"	waiting_time: ";
	//for(int i=0 ; i<num_process ; ++i){cout<<waiting_time[i]<<" ";}
	
	//turnaround_time
	for(int i=0, total_waiting=0 ; i<num_process ; ++i){
		if(remaining_time[i] != 0){	
			turnaround_time[i] = time + remaining_time[i] - arrival_time[i];
			time += remaining_time[i];
		}
	}
	//waiting_time
}