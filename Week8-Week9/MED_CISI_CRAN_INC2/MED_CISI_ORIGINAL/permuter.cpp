#include<bits/stdc++.h>
using namespace std;

int main(){
    ifstream fin("med_cisi_cran_cc.txt");
    ofstream fout;
    fout.open("input.in");
    int w,d,k;
    fin>>w>>d>>k;
    fout<<w<<" "<<d<<" "<<k<<endl;
    vector<int> perm;
    for(int i=0;i<d;i++){
        perm.push_back(i);
    }
    random_shuffle(perm.begin(),perm.end());
    int r,c;
	float val;
	while(fin>>r){
		fin>>c>>val;
		fout<<r<<" "<<perm[c]<<" "<<val<<endl;
	}
    fin.close();
    fout.close();

    fin=ifstream("old_labels.in");
    fout.open("labels.in");
    int cur_label;
    vector<int> new_labels(d);
	for(int i=0;i<d;i++){
        fin>>cur_label;
        new_labels[perm[i]]=cur_label;
    }
    for(int i=0;i<d;i++){
        fout<<new_labels[i]<<" ";
    }
    fin.close();
    fout.close();

    return 0;
}