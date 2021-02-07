#include<bits/stdc++.h>
using namespace std;
#define f(i,a,b) for(int i=a;i<b;i++)
#define vii(type) vector<vector<type>>
#define vi(type) vector<type>

int main(int argc,const char* argv[]){
    map<string,pair<int,int>> mp;
    ifstream fin1(argv[1]),fin2(argv[2]);
    string temp;
    for(int i=0;fin1>>temp;i++){
        mp[temp]=make_pair(i,-1);
        fin1>>temp;
        fin1>>temp;
    }

    int ind;
    for(ind=0;fin2>>temp;ind++){
        if(mp.count(temp))
            mp[temp].second=ind;
        else
            mp[temp]=make_pair(-1,ind);

        fin2>>temp;
        fin2>>temp;
    }
    //cout<<"ind:"<<ind<<endl;
    ofstream fout;
    fout.open(argv[3]);
    fout<<mp.size()<<endl;
    for(auto i:mp)
        fout<<i.first<<" "<<i.second.first<<" "<<i.second.second<<endl;
    fout.close();
    fin1.close();
    fin2.close();
}
