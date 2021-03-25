#include<bits/stdc++.h>
using namespace std;

int main(){
	int w,d,k;
	ifstream fin("med_cisi_cc.txt");
	vector<pair<pair <int,int>,float > > temp;
	vector<vector<pair<pair <int,int>,float > > > batch(2);
	fin>>w>>d>>k;
	//cout<<w<<" "<<d<<" "<<endl;
	int r,c;
	float val;
	while(fin>>r){
		fin>>c>>val;
		if(c<900 || c>=1200)				
			temp.push_back(make_pair(make_pair(r,c),val));
		else if(c<1050)
			batch[0].push_back(make_pair(make_pair(r,c),val));
		else if(c<1200)
			batch[1].push_back(make_pair(make_pair(r,c),val));
		// else if(c<1110)
		// 	batch[2].push_back(make_pair(make_pair(r,c),val));
		// else if(c<1180)
		// 	batch[3].push_back(make_pair(make_pair(r,c),val));
		// else if(c<1250)
		// 	batch[4].push_back(make_pair(make_pair(r,c),val));
		// else 
		//     batch[0].push_back(make_pair(make_pair(r,c),val));
	}
	fin.close();
	//exit(0);
	ofstream fout;
	fout.open("input.in");
	int num_edges=temp.size();	
	fout<<w<<" "<<d-300<<" "<<k<<" "<<num_edges<<endl;
	for(auto i:temp)
		fout<<i.first.first<<" "<<i.first.second<<" "<<i.second<<endl;
	
	for(int i=0;i<2;i++){
		int num_ins=150, num_del=0, n_edges=batch[i].size();
		fout<<num_ins<<" "<<num_del<<" "<<n_edges<<endl;
		for(int j=0;j<150;j++){
			int doc_num=900+150*i+j;
			fout<<doc_num<<" "<<1<<endl;
		}
		for(auto k:batch[i])
			fout<<k.first.first<<" "<<k.first.second<<" "<<k.second<<endl;
	}
	fout.close();
		
}
