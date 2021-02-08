#include<bits/stdc++.h>
using namespace std;
#define f(i,a,b) for(int i=a;i<b;i++)
#define vii(type) vector<vector<type>>
#define vi(type) vector<type>

int main(int argc,const char* argv[]){
    ifstream fin1(argv[1]),fin2(argv[2]),fin3(argv[3]);
    int w1,w2,d1,d2;
    string cur_word;

    // w1, d1 assignment
    fin1>>cur_word;
    w1=stoi(cur_word);
    fin1>>cur_word;
    d1=stoi(cur_word);

     // w2, d2 assignment
    fin2>>cur_word;
    w2=stoi(cur_word);
    fin2>>cur_word;
    d2=stoi(cur_word);

    //cout<<w1<<" "<<d1<<" "<<w2<<" "<<d2<<"\n";
    vii(float) A(w1,vi(float)(d1)),B(w2,vi(float)(d2));
    for(int i=0;i<w1;i++){
        //cout<<cur_word<<"\n";
        for(int j=0;j<d1;j++){
            fin1>>cur_word;
            A[i][j]=stof(cur_word);
        }
    }
    //cout<<A[0][0]<<endl;
    for(int i=0;i<w2;i++){
       for(int j=0;j<d2;j++){
             fin2>>cur_word;
            B[i][j]=stof(cur_word);
        }
    }
    /*
    for(int i=0;i<w1;i++){
        for(int j=0;j<d1;j++)
            cout<<A[i][j];
        cout<<"\n";
    }*/
    // MED is first , CRAN is second
    //cout<<B[0][0]<<endl;
    fin3>>cur_word;
    int x3=stoi(cur_word);
    vector<pair<int,int>> winf(x3);
    for(int i=0;i<x3;i++)
        fin3>>winf[i].first>>winf[i].second;

    //cout<<winf[0].first<<endl;

    vii(float) C(x3,vi(float)(d1+d2,0));
    f(i,0,x3)
        f(j,0,d1)
            if(winf[i].first!=-1)
                C[i][j]=A[winf[i].first][j];

    //cout<<"C[0][0]:"<<C[0][0]<<endl;
    f(i,0,x3)
        f(j,d1,d1+d2)
            if(winf[i].second!=-1){
                if(winf[i].second>=w2)
                    cout<<w2<<"\n";
                C[i][j]=B[winf[i].second][j-d1];
            }

    //cout<<"C[0][d1]:"<<C[0][d1]<<endl;
    fin1.close();
    fin2.close();
    fin3.close();

    // writing input file for algo
    ofstream fout;
    fout.open("input.in");
    fout<<x3<<" "<<d1+d2<<" "<<2<<endl;
    int ctr=0;
    f(i,0,x3){
        f(j,0,d1+d2){
            fout<<C[i][j]<<" ";
            if(C[i][j]!=0)
                ctr++;
        }
        fout<<endl;
    }

    cout<<"Number of non zero elements:"<<ctr<<" value should be "<<52003+68240<<" or "<<52003+83302<<endl;
    ofstream fout2;
    fout2.open("labels.in");
    f(i,0,d1)
        fout2<<0<<" ";
    f(i,d1,d1+d2)
        fout2<<1<<" ";
    fout2<<endl;

    fout.close();
    fout2.close();


}
