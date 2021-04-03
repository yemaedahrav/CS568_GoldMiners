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
	int cluster; // -1 for initialization, 0 and 1 for clusters
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

// note that graph is undirected , but edges are directed, i.e graph will have both u -> v, and v->u
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
    int word_inserted=200,num_switches=100;
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

    set<Vertex> to_switch;
    for(auto it:labels){
        Vertex u=it.first;
        if(it.second==0 &&  adj_list[u].size()<word_inserted && to_switch.size()<num_switches){
           to_switch.insert(u);
        }
    }

    ofstream fout;
    fout.open("input2.in");
    fout<<w+word_inserted<<" "<<d<<" "<<k<<endl;
    for(auto it:adj_list){
        Vertex u=it.first;
        if(!u.is_doc){
            for(auto e:it.second)
                fout<<u.num<<" "<<e.v.num<<" "<<e.weight<<endl;
        }
    }

    for(int it=w;it<w+word_inserted;it++){
        for(auto x:labels)
            if(x.second==1 || to_switch.count(x.first))
                fout<<it<<" "<<x.first.num<<" "<<0.700<<endl;
    }

    fout<<0<<" "<<word_inserted<<" "<<0<<endl;
    for(int it=w;it<w+word_inserted;it++){
        fout<<it<<" "<<0<<endl;
    }
    for(auto it:labels)
        fout<<it.second<<" ";
    fout<<endl;
    fout.close();
    
    fout.open("labels2.in");
    for(auto it:labels)
        if(to_switch.count(it.first))
            fout<<1<<" ";
        else
            fout<<it.second<<" ";
    fout.close();
    return 0;
}