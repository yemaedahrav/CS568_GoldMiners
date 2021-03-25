#include<bits/stdc++.h>
using namespace std;

int main(){
	int w,d,k;
	int n_batches=4;
	int b_size=70;
	ifstream fin("MED_CISI_SHUFFLED/input.in");
	set<pair<pair <int,int>,float > > temp;
	vector<set<pair<pair <int,int>,float > > > batch(n_batches);
	fin>>w>>d>>k;
	//cout<<w<<" "<<d<<" "<<endl;
	int r,c;
	float val;
	while(fin>>r){
		fin>>c>>val;
		if(c<d-n_batches*b_size)				
			temp.insert(make_pair(make_pair(r,c),val));
		else{
			for(int i=n_batches-1;i>=0;i--){
				if(c<d-i*b_size){
					batch[n_batches-1-i].insert(make_pair(make_pair(r,c),val));
					break;	
				}
			}
		}	
	}
	fin.close();
	//exit(0);
	ofstream fout;
	fout.open("input.in");
	int num_edges=temp.size();	
	fout<<w<<" "<<d-n_batches*b_size<<" "<<k<<" "<<num_edges<<endl;
	for(auto i:temp)
		fout<<i.first.first<<" "<<i.first.second<<" "<<i.second<<endl;
	
	for(int i=0;i<n_batches;i++){
		int num_ins=b_size, num_del=0, n_edges=batch[i].size();
		fout<<num_ins<<" "<<num_del<<" "<<n_edges<<endl;
		for(int j=0;j<b_size;j++){
			int doc_num=d-(n_batches-i)*b_size+j;
			fout<<doc_num<<" "<<1<<endl;
		}
		for(auto k:batch[i])
			fout<<k.first.first<<" "<<k.first.second<<" "<<k.second<<endl;
	}
	fout.close();			
}
