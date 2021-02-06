#include<bits/stdc++.h>
using namespace std;
#define f(i,a,b) for(int i=a;i<b;i++)
#define vii(type) vector<vector<type>>
#define vi(type) vector<type>

int main(int argc,const char* argv[]){
    ifstream fin1(argv[1]),fin2(argv[2]),fin3(argv[3]),fin4(argv[4]);
    int w1,w2,d1,d2,w3,d3;
    string cur_word;

    // w1 assignment
    fin1>>cur_word;
    w1=stoi(cur_word);
    fin1>>cur_word;
    d1=stoi(cur_word);

     // w2 assignment
    fin2>>cur_word;
    w2=stoi(cur_word);
    fin2>>cur_word;
    d2=stoi(cur_word);

    fin3>>cur_word;
    w3=stoi(cur_word);
    fin3>>cur_word;
    d3=stoi(cur_word);



    //cout<<w1<<" "<<d1<<" "<<w2<<" "<<d2<<" "<<w3<<" "<<d3<<"\n";
    vii(float) A(w1,vi(float)(d1)),B(w2,vi(float)(d2)),D(w3,vi(float)(d3));
    for(int i=0;i<w1;i++){
        for(int j=0;j<d1;j++){
            fin1>>cur_word;
            A[i][j]=stof(cur_word);
            //if(A[i][j]==1460)
                //cout<<"A:"<<i<<" "<<j<<endl;
        }
    }
    //cout<<A[0][0]<<endl;
    for(int i=0;i<w2;i++){
        for(int j=0;j<d2;j++){
            fin2>>cur_word;
            B[i][j]=stof(cur_word);
        }
    }

     for(int i=0;i<w3;i++){
        for(int j=0;j<d3;j++){
            fin3>>cur_word;
            D[i][j]=stof(cur_word);
        }
    }
    // MED is first , CRAN is second,CISI is thired
    //cout<<B[0][0]<<endl;
    fin4>>cur_word;
    int x4=stoi(cur_word);
    vector<vector<int>> winf(x4,vector<int>(3));
    for(int i=0;i<x4;i++)
        fin4>>winf[i][0]>>winf[i][1]>>winf[i][2];

    //cout<<winf[0].first<<endl;

    vii(float) C(x4,vi(float)(d1+d2+d3,0));
    f(i,0,x4)
        f(j,0,d1)
            if(winf[i][0]!=-1)
                C[i][j]=A[winf[i][0]][j];

    //cout<<"C[0][0]:"<<C[0][0]<<endl;
    f(i,0,x4)
        f(j,d1,d1+d2)
            if(winf[i][1]!=-1)
                C[i][j]=B[winf[i][1]][j-d1];

    f(i,0,x4)
        f(j,d1+d2,d1+d2+d3)
            if(winf[i][2]!=-1)
                C[i][j]=D[winf[i][2]][j-d1-d2];

    //cout<<"row dimesnion"<<x3<<endl;
    fin1.close();
    fin2.close();
    fin3.close();
    fin4.close();

    // writing input file for algo
    ofstream fout;
    fout.open("input.in");
    fout<<x4<<" "<<d1+d2+d3<<" "<<3<<endl;
    int ctr=0;
    f(i,0,x4){
        f(j,0,d1+d2+d3){
            fout<<C[i][j]<<" ";
            if(C[i][j]!=0)
                ctr++;
        }
        fout<<endl;
    }
    cout<<"Number of non zero elements:"<<ctr<<" value should be"<<52003+68240+83302<<endl;

    ofstream fout2;
    fout2.open("labels.in");
    f(i,0,d1)
        fout2<<0<<" ";
    f(i,d1,d1+d2)
        fout2<<1<<" ";
    f(i,d1+d2,d1+d2+d3)
        fout2<<2<<" ";
    fout2<<endl;

    fout.close();
    fout2.close();


}
