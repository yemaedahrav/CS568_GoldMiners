#include<bits/stdc++.h>
#include <mlpack/methods/kmeans/kmeans.hpp>
#include <sys/types.h>
#include <unistd.h>
using namespace std;
using namespace arma;
using namespace mlpack::kmeans;

class Vertex
{
public:
	int num;
	bool is_doc;
	int cluster; 
	int weight;
	Vertex() {}
	Vertex(int x, bool y)
	{
		num = x;
		is_doc = y;
		cluster = -1;
	}

	bool operator<(const Vertex &v) const
	{
		if (num != v.num)
			return num < v.num;
		return is_doc < v.is_doc;
	}
	bool operator==(const Vertex& lhs)
	{
		if((*this).num == lhs.num && (*this).is_doc == lhs.is_doc)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

class Edge
{
public:
	Vertex v;
	double weight;
	Edge(Vertex x, double y)
	{
		v = x;
		weight = y;
	}
};

int main(){
    int w,d,k;
    ifstream fin("input.in");
    fin>>w>>d>>k;
    map<Vertex,vector<Edge>> adj_list;
    int word_inserted=100,num_switches=30,n_batches=3;
    int r,c;
    double weight;
    while(fin>>r){
        fin>>c>>weight;
        Vertex wi=Vertex(r,false),di=Vertex(c,true);
        adj_list[wi].push_back(Edge(di,weight));
        adj_list[di].push_back(Edge(wi,weight));
    }
    fin.close();
    
    fin = ifstream("labels.in");
    map<Vertex,int> labels;
    int cur_label,i=0;
    while(fin>>cur_label){
        labels[Vertex(i,true)]=cur_label;
        i++;
    }
    fin.close();

    vector<set<Vertex>> to_switch(n_batches);
    for(auto it:labels){
        Vertex u=it.first;
        for(int j=0;j<n_batches;j++){
            if(it.second==0 &&  adj_list[u].size()<word_inserted && to_switch[j].size()<num_switches){
                to_switch[j].insert(u);
                break;
            }
        }
    }

    double th=0.700;
    vector<pair<pair<int,int>,double>> edges;
    for(auto it:adj_list){
        Vertex u=it.first;
        if(!u.is_doc){
            for(auto e:it.second)
                edges.push_back(make_pair(make_pair(u.num,e.v.num),e.weight));
        }
    }

    
    for(int j=0;j<n_batches;j++){
        for(int it=w+j*word_inserted;it<w+(j+1)*word_inserted;it++){
            for(auto x:labels)
                if(to_switch[j].count(x.first) || (x.second==1 && rand()%5==1) )
                    edges.push_back(make_pair(make_pair(it,x.first.num),th));
        }
    }
    ofstream fout;
    fout.open("input2.in");
    fout<<w+word_inserted*n_batches<<" "<<d<<" "<<k<<" "<<edges.size()<<endl;
    for(auto it:edges)
        fout<<it.first.first<<" "<<it.first.second<<" "<<it.second<<endl;

    for(int j=0;j<n_batches;j++){
        fout<<0<<" "<<word_inserted<<" "<<0<<endl;
        for(int it=w+j*word_inserted;it<w+(j+1)*word_inserted;it++){
            fout<<it<<" "<<0<<endl;
        }
    }
    fout.close();
    
    map<Vertex,int> final_labels;
    fout.open("labels0.in");
    for(auto it:labels){
        bool found=false;
        for(int j=0;j<n_batches;j++){
            if(to_switch[j].count(it.first)){
                fout<<1<<" ";
                final_labels[it.first]=1;
                found=true;
                break;
            }
        }
        if(!found){
            fout<<it.second<<" ";
            final_labels[it.first]=it.second;
        }
    }
    fout.close();

    for(int j=0;j<n_batches;j++){
        for(auto it=final_labels.begin();it!=final_labels.end();it++)
            if(to_switch[j].count(it->first))
                it->second=0;
        
        fout.open("labels"+to_string(j+1)+".in");
        for(auto it:final_labels)
            fout<<it.second<<" "<<endl;
        fout.close();
    }


    return 0;
}