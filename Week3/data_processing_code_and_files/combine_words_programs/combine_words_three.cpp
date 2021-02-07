#include<bits/stdc++.h>
using namespace std;
#define f(i,a,b) for(int i=a;i<b;i++)
#define vii(type) vector<vector<type>>
#define vi(type) vector<type>

int main(int argc,const char* argv[]){
    map<string,vector<int>> mp;
    ifstream fin1(argv[1]),fin2(argv[2]),fin3(argv[3]);
    string temp;
    for(int i=0;fin1>>temp;i++){
        mp[temp]={i,-1,-1};
        fin1>>temp;
        fin1>>temp;
    }

    for(int i=0;fin2>>temp;i++){
        if(mp.count(temp)==0)
            mp[temp]={-1,i,-1};
        else
            mp[temp][1]=i;

        fin2>>temp;
        fin2>>temp;
    }

    for(int i=0;fin3>>temp;i++){
        if(!mp.count(temp))
            mp[temp]={-1,-1,i};
        else
            mp[temp][2]=i;
        fin3>>temp;
        fin3>>temp;
    }
    //cout<<"ind:"<<ind<<endl;
    ofstream fout;
    fout.open(argv[4]);
    fout<<mp.size()<<endl;
    for(auto i:mp)
        fout<<(i.second)[0]<<" "<<(i.second)[1]<<" "<<(i.second)[2]<<endl;
    fout.close();
    fin1.close();
    fin2.close();
    fin3.close();
}
