#include<bits/stdc++.h>
#include<fstream>
using namespace std;
#define ll long long
#define N1 1001
#define f(i,a,b) for(ll i=a;i<b;i++)
#define all(a) a.begin(),a.end()
typedef vector<ll> vi;
typedef vector<vi> vii;
typedef vector<pair<ll,ll>> vpa;
const ll N=1000000007;

int main(int argc, char const *argv[])
{

    int w,d,non_zero;
    int per_line=10,per_line2=8;
    string input_file=argv[1];
    string output_file=input_file.substr(0,input_file.length()-7)+".txt";
    cout<<input_file<<"\n";
    ifstream fin(input_file,ifstream::in);
    string cur_line;


    getline(fin,cur_line);
    getline(fin,cur_line);
    getline(fin,cur_line);
    char* temp=&cur_line[0];
    char* token=strtok(temp," ");
    token=strtok(NULL," ");
    d=stoi(string(token));
    token=strtok(NULL," ");
    w=stoi(string(token));
    token=strtok(NULL," ");
    non_zero=stoi(string(token));
    getline(fin,cur_line);

    //cout<<d<<" "<<w<<" "<<non_zero<<"\n";


    int arr_col[w+2];
    int row_num[non_zero+1];
    float val[non_zero+1];
    int number_of_lines=ceil(((w+1)*1.0)/per_line);
    f(i,0,number_of_lines)
    {
        getline(fin,cur_line);
        char* temp=&cur_line[0];
        char* token=strtok(temp," ");
        f(j,1,per_line+1){
            arr_col[i*per_line+j]=stoi(string(token));
            token=strtok(NULL," ");
            if(token==NULL)
                break;
        }
    }

    number_of_lines=ceil((non_zero*1.0)/per_line);
    f(i,0,number_of_lines)
    {
        getline(fin,cur_line);
        char* temp=&cur_line[0];
        char* token=strtok(temp," ");
        f(j,1,per_line+1){
            row_num[i*per_line+j]=stoi(string(token));
            token=strtok(NULL," ");
            if(token==NULL)
                break;
        }
    }
    //cout<<"done"<<"\n";

    number_of_lines=ceil((non_zero*1.0)/per_line2);
    f(i,0,number_of_lines)
    {
        getline(fin,cur_line);
        char* temp=&cur_line[0];
        char* token=strtok(temp," ");
        f(j,1,per_line2+1){
            val[i*per_line2+j]=stof(string(token));
            token=strtok(NULL," ");
            if(token==NULL)
                break;
        }
    }


    vector<vector<float>> X(w+1,vector<float>(d+1,0));
    int ctr=1;
    for(int i=1;i<=w;i++)
    {
        int num=arr_col[i+1]-arr_col[i];
        for(int j=0;j<num;j++)
            X[i][row_num[ctr+j]]=val[ctr+j];
        ctr+=num;
    }
    fin.close();

    ofstream fout;
    fout.open(output_file);
    fout<<w<<" "<<d<<"\n";
    for(int i=1;i<=w;i++){
        for(int j=1;j<=d;j++)
            fout<<X[i][j]<<" ";
        fout<<"\n";
    }
    fout.close();
}
