#include<bits/stdc++.h>
using namespace std;

int main(){
	int w,d,k;
	ifstream fin("med_cisi_2d");
	fin>>w>>d>>k;
	ofstream fout;
	fout.open("input.in");
	vector<pair<pair <int,int>,float > > temp;
	vector<vector<pair<pair <int,int>,float > > > batch(3);
	for(int i=0;i<w;i++)
		for(int j=0;j<d;j++){
			float cur;
			fin>>cur;
			if(cur!=0){
				if(j<d-300)				
					temp.push_back(make_pair(make_pair(i,j),cur));
				else if(j<d-200)
					batch[0].push_back(make_pair(make_pair(i,j),cur));
				else if(j<d-100)
					batch[1].push_back(make_pair(make_pair(i,j),cur));
				else if(j<d)
					batch[2].push_back(make_pair(make_pair(i,j),cur));

			}
		}
	int num_edges=temp.size();	
	fout<<w<<" "<<d-300<<" "<<k<<" "<<num_edges<<endl;
	for(auto i:temp)
		fout<<i.first.first<<" "<<i.first.second<<" "<<i.second<<endl;
	
	for(int i=0;i<2;i++){
		int num_ins=100, num_del=0, n_edges=batch[i].size();
		fout<<num_ins<<" "<<num_del<<" "<<n_edges<<endl;
		for(int j=0;j<100;j++){
			int doc_num=d-300+100*i+j;
			fout<<doc_num<<" "<<1<<endl;
		}
		for(auto k:batch[i])
			fout<<k.first.first<<" "<<k.first.second<<" "<<k.second<<endl;
	}
	fout.close();
	fin.close();
			
}
