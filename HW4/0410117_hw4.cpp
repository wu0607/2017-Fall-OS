# include <iostream>
# include <fstream>
#include <vector>
#include <stdio.h>
using namespace std;

int main(int argc, char **argv){
	//ifstream inFile_backing(argv[1]);
	FILE * inFile_backing = fopen(argv[1],"r");
	ifstream inFile_address(argv[2]);
	ofstream outFile;
	outFile.open("results.txt");
	int n;		
	vector<int> address; 
	vector<int> page_table(256,NULL);//256 ints with value 0 	
	char read[256];
	char physicalMemory[65536];
	
	//initialize
	int **TLB = new int*[16];
	int TLB_pos = 0;
	int frame = 0;
	int TLBhit = 0;
	int pagefault = 0;
	for(int i=0 ; i<16 ; ++i){
		TLB[i] = new int[3];//TLB[i][j]=0
	}
	
	//readfile
	inFile_address >> n;
	address.resize(n);
	int count = 0;
	while(!inFile_address.eof()){
		inFile_address >> address[count];
		++count;	
	}
	 // for(int i=0;i<count;++i){
		 // cout<<address[i]<<endl;		
	 // }
	//
	for(int i=0 ; i<count ; ++i){
		//cout<<"frame: "<<frame<<endl;
		int page_num = address[i]/256 ;//move to address[0] position
		int offset = address[i]%256 ;
		//Check if already exists.
		bool found = false;
		for(int j=0 ; j<16 ; ++j){//See TLB first
			if(page_num == TLB[j][0]){
				++TLBhit;
				int physicalAddress = TLB[j][1]*256 + offset;
				//cout<<"physicalAddress: "<<physicalAddress;
				//cout<<" "<<(int)physicalMemory[physicalAddress]<<endl;
				outFile<<physicalAddress<<" "<<(int)physicalMemory[physicalAddress]<<endl;
				found = true;
				TLB[j][2] = 0;
				for(int other=0 ; other<TLB_pos ; ++other){
					if(other != j){
						++TLB[other][2];		
					}					
				}
				break;
			}		
		}
		if(found==false){//Then See page table
			if(page_table[page_num]!=NULL){
				int physicalAddress = page_table[page_num]*256 + offset;
				//cout<<"physicalAddress: "<<physicalAddress;
				//cout<<" "<<(int)physicalMemory[physicalAddress]<<endl;
				outFile<<physicalAddress<<" "<<(int)physicalMemory[physicalAddress]<<endl;
				//TLB renew
				int highest = 0;
				for(int index=1 ; index<16 ; index++){
					if(TLB[index][2]>TLB[highest][2]){
						highest = index;						
					}
				}
				//cout<<"highest"<<highest;
				TLB[highest][0] = page_num;
				TLB[highest][1] = page_table[page_num];//frame
				for(int other=0 ; other<TLB_pos ; ++other){
					if(other == highest){
						TLB[highest][2] = 0;
					}else{
						++TLB[other][2];		
					}					
				}
				found = true;		
			}
		}
		if(found==false){//New value! Save it to TLB&page_table			
			++pagefault;
			fseek(inFile_backing, page_num*256 , SEEK_SET );	
			fread(read, sizeof(char), 256, inFile_backing);//get 256numbers of char to read
			for(int index = 0; index < 256; index++) {
				physicalMemory[frame*256 + index] = read[index];
				//cout<<"read"<<index<<read[index]<<endl;
			}
			
			if (TLB_pos<16){
				TLB[TLB_pos][0] = page_num;
				TLB[TLB_pos][1] = frame;
				TLB[TLB_pos][2] = 0;
				for(int other=0 ; other<TLB_pos ; ++other){
					++TLB[other][2];					
				}
				++TLB_pos;
				page_table[page_num] = frame;
				int physicalAddress = page_table[page_num]*256 + offset;
				//cout<<"physicalAddress: "<<physicalAddress;
				//cout<<" "<<(int)physicalMemory[physicalAddress]<<endl;
				outFile<<physicalAddress<<" "<<(int)physicalMemory[physicalAddress]<<endl;
			}else{//LRU, choose one to replace				
				int highest = 0;
				for(int index=1 ; index<16 ; index++){
					if(TLB[index][2]>TLB[highest][2]){
						highest = index;						
					}
				}
				TLB[highest][0] = page_num;
				TLB[highest][1] = frame;
				for(int other=0 ; other<16 ; ++other){
					if(other == highest){
						TLB[highest][2] = 0;
					}else{
						++TLB[other][2];		
					}					
				}				
				page_table[page_num] = frame;
				int physicalAddress = page_table[page_num]*256 + offset;
				//cout<<"physicalAddress: "<<physicalAddress;
				//cout<<" "<<(int)physicalMemory[physicalAddress]<<endl;
				outFile<<physicalAddress<<" "<<(int)physicalMemory[physicalAddress]<<endl;
			}
		++frame;
		}		
		//++frame;
	}
	outFile << "TLB hits: " << TLBhit << endl;
	outFile << "Page Faults: " << pagefault;
	for(int i=0 ; i<16 ; ++i){
		delete[] TLB[i];
	}
	delete[] TLB;
	return 0;
}
