#include<bits/stdc++.h>
using namespace std;

int main(){
	int w,d,k;
	ifstream fin("input.in");
	fin>>w>>d>>k;
	ofstream fout;
	fout.open("input_cc.in");
	fout<<w<<" "<<d<<" "<<k<<endl;
	for(int i=0;i<w;i++)
		for(int j=0;j<d;j++){
			float cur;
			fin>>cur;
			if(cur!=0)
				fout<<i<<" "<<j<<" "<<cur<<endl;
		}
	fout.close();
	fin.close();
			
}
