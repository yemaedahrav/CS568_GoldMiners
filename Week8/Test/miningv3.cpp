#include <bits/stdc++.h>
#include <mlpack/methods/kmeans/kmeans.hpp>
#include <sys/types.h>
#include <unistd.h>
#include "cluster_components.h"
using namespace std;
using namespace arma;
using namespace mlpack::kmeans;
//#define NDEBUG

#ifndef NDEBUG
#define ASSERT_EX(condition, statement) \
    do { \
        if (!(condition)) { statement; assert(condition); } \
    } while (false)
#else
#define ASSERT_EX(condition, statement) ((void)0)
#endif

int k;
vector <int> labels(3891);
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

char get_char(bool y)
{
	if (y)
		return 'd';
	return 'w';
}

class Graph
{
private:
	map<Vertex, vector<Edge>> adj_list;
	set<Vertex> vlist;
	vector<Vertex> order;

public:
	Graph(map<Vertex, vector<Edge>> x, set<Vertex> y)
	{
		adj_list = x;
		vlist = y;
	}

	Graph() {}
	map<Vertex, vector<Edge>> get_adj_list()
	{
		return adj_list;
	};

	set<Vertex> get_vlist()
	{
		return vlist;
	}
	vector<Vertex> get_order()
	{
		return order;
	}
	int num_vertex()
	{
		return vlist.size();
	}

	void print()
	{
		cout << "The vertices in the graph are\n";
		for (auto u : vlist)
			cout << get_char(u.is_doc) << u.num << " ";
		cout << endl;
		cout << "The adjacency list is\n";
		for (auto i : adj_list)
		{
			Vertex u = i.first;
			cout << "Vertex " << get_char(u.is_doc) << u.num << endl;
			for (auto e : i.second)
				cout << get_char(e.v.is_doc) << e.v.num << " " << e.weight << "    ";
			cout << endl;
		}
		cout << endl;
	}

	void print_words_and_docs()
	{
		int wordcount = 0;
		int doccount = 0;
		for(auto vertex: vlist)
		{
			wordcount += !(vertex.is_doc);
			doccount += (vertex.is_doc);
		}
		cout << "# Words: " << wordcount << endl;
		cout << "# Docs : " << doccount  << endl;
	}
	Graph get_subgraph(set<Vertex> v_set)
	{
		map<Vertex, vector<Edge>> sub_adj_list;
		for (auto u : v_set)
			for (auto e : adj_list[u])
				if (v_set.count(e.v))
					sub_adj_list[u].push_back(e);

		return Graph(sub_adj_list, v_set);
	};

	void add_Edge(Vertex u, Vertex v, double weight)
	{
		adj_list[u].push_back(Edge(v, weight));
		adj_list[v].push_back(Edge(u, weight));
		vlist.insert(u);
		vlist.insert(v);
	}

	void add_vertex(Vertex u)
	{
		vlist.insert(u);
	}

	sp_mat get_word_by_document_matrix()
	{
		int w = 0, d = 0;
		map<Vertex, int> new_index;
		vector<Vertex> words, documents;
		for (auto v : vlist)
		{
			if (v.is_doc)
			{
				new_index.insert({v, d});
				d++;
				documents.push_back(v);
			}
			else
			{
				new_index.insert({v, w});
				w++;
				words.push_back(v);
			}
		}
		sp_mat word_by_doc(w, d);
		for (auto pr : adj_list)
		{

			Vertex vert = pr.first;
			vector<Edge> vert_list = pr.second;
			if (vert.is_doc)
				continue;
			for (auto e : vert_list)
			{
				word_by_doc(new_index[vert], new_index[e.v]) = e.weight;
			}
		}

		order.clear();
		for (auto word : words)
		{
			order.push_back(word);
		}
		for (auto document : documents)
		{
			order.push_back(document);
		}

		return word_by_doc;
	}
};

class Batch
{
	vector<Vertex> ins_vertices, del_vertices;
	vector<Edge> added_edges;

public:
	vector<Vertex> get_ins_vertices()
	{
		return ins_vertices;
	}

	void add_v(Vertex u)
	{
		ins_vertices.push_back(u);
	}
};

void dfs(Vertex &u, map<Vertex, vector<Edge>> &adj_list, int &c_num, map<Vertex, int> &component_num)
{
	component_num[u] = c_num;
	for (auto e : adj_list[u])
	{
		if (!component_num.count(e.v))
			dfs(e.v, adj_list, c_num, component_num);
	}
	return;
}

// get_neighbourhood basically uses a multi-source bfs;
Graph get_neighbourhood(Graph G, Batch B, int d, double threshold)
{
	map<Vertex, vector<Edge>> adj_list = G.get_adj_list();
	vector<Vertex> ins_vertices = B.get_ins_vertices();
	queue<Vertex> q;
	set<Vertex> v_set;
	for (auto u : ins_vertices)
	{
		q.push(u);
		v_set.insert(u);
	}

	for (int i = 0; i < d; i++)
	{
		int x = q.size();
		while (x--)
		{
			Vertex u = q.front();
			q.pop();
			for (auto e : adj_list[u])
			{
				if (!v_set.count(e.v) && e.weight >= threshold)
				{
					v_set.insert(e.v);
					q.push(e.v);
				}
			}
		}
	}

	return G.get_subgraph(v_set);
}

vector<Graph> get_connected_components(Graph G)
{
	map<Vertex, int> component_num;
	map<Vertex, vector<Edge>> adj_list = G.get_adj_list();
	set<Vertex> v_set = G.get_vlist();
	int c_num = 0;
	for (auto u : v_set)
		if (!component_num.count(u))
		{
			dfs(u, adj_list, c_num, component_num);
			c_num++;
		}
	vector<set<Vertex>> v_groups(c_num);
	for (auto u : v_set)
		v_groups[component_num[u]].insert(u);

	vector<Graph> G_list;
	for (int i = 0; i < c_num; i++)
		G_list.push_back(G.get_subgraph(v_groups[i]));

	return G_list;
}
class Clustering
{
public:
	vector<set<Vertex>> c= vector<set<Vertex>>(k);
	vector<double> cut=vector<double>(k,0.0),weight=vector<double>(k,0.0);
	double normalized_cut;
	Graph G;
	Clustering(){}	
	
	// Normalized cut value, weight of cut 0, weight of cut 1
	Clustering(Graph G, vector<Vertex> vertices, Row<size_t> assignments)
	{
		this->G = G;
		for (int i = 0; i < vertices.size(); i++)
		{
			if (assignments[i] < k)
				c[assignments[i]].insert(vertices[i]);
			else
				assert(0);
		}
	}
	Clustering(vector<Vertex> vertices, Row<size_t> assignments)
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			if (assignments[i] < k)
				c[assignments[i]].insert(vertices[i]);
			else
				assert(0);
		}
	}
	Clustering(Graph G, vector<set<Vertex>> c)
	{
		this->c = c;
		this->G = G;
	}
	void print_cluster_size()
	{
		int ctr = 0;
		for(auto i: c)
		{
			cout << "Cluster " << ++ctr << ": " << i.size() << endl;
		}
		cout << endl;
	}

	void print_confusion_matrix()
	{
		if(labels.size() == 0) return;

		int cf[k][k];
		for (int i = 0; i < k; i++)
			for (int j = 0; j < k; j++)
				cf[i][j] = 0;
		
		for(auto s: c)
		{
			for(auto doc: s)
			{
				if(!doc.is_doc) continue;
				int assigned_label = return_label(doc);
				int true_label = labels[doc.num];

				ASSERT_EX(doc.num <3891 && doc.num >=0, cerr << "Doc_num = " << doc.num << endl);
				
				cf[true_label][assigned_label]++;
			}
		}

		for(int i = 0; i < k; i++)
		{
			for(int j = 0; j < k; j++)
				cout << cf[i][j] << " " ;
			cout << endl;
		}

	}
	void remove(set<Vertex> vset)
	{
		for (auto u : vset)
			for(int i=0;i<k;i++)			
				c[i].erase(u);
	}

	int num_of_vertices()
	{
		int sum=0;
		for(auto ci:c)
			sum+=ci.size();
		return (int)(sum);
	}

	
	void print2()
	{
		for(int i=0;i<k;i++){
			cout << "The documents in cluster "<<i+1<<" are: \n ";
			for (auto x :c[i])
				if (x.is_doc)
					cout <<get_char(x.is_doc)<< x.num << " ";
			cout << endl << endl;
		}
	}
	int return_label(Vertex v)
	{
		for(int i=0;i<k;i++)
			if(c[i].count(v))
				return i;
		assert(0);
		return -1;
	}

	void compute_nCut()
	{
		auto sub_adj_list = G.get_adj_list();
		normalized_cut= 0;
		for(int i=0;i<k;i++){
			weight[i]=0;
			cut[i]=0;
		}
		for (auto it : sub_adj_list)
		{
			for (auto e : it.second)
			{
				auto u = it.first;
				for(int i=0;i<k;i++){
					if(c[i].count(u)){
						weight[i]+=e.weight;
						if(!c[i].count(e.v))
							cut[i]+=e.weight;
					}
				}
			}
		}
		//Checking if weights are zero
		for(int i=0;i<k;i++){
			if (abs(weight[i]) <= 0.0000001){
				weight[i]=0.0000001;
				cut[i]=100.0;

				//cout << "Weight of some set " << i << " is zero. " << endl;
			}
			normalized_cut += cut[i]/ weight[i];
		}
	}

	void print()
	{
		cout << "NC : " << normalized_cut << endl;
		for(int i=0;i<k;i++)
			cout<<"W"<<i<<" :"<<weight[i]<<endl;
		cout<<endl;
		for(int i=0;i<k;i++)
			cout<<"C"<<i<<" :"<<cut[i]<<endl;
	}
};

Clustering merge_Clustering_Cluster(Graph G, Clustering c, Clustering d, int j)
{
	set<Vertex> to_merge = d.c[j];
	double min_cut_value=LONG_MAX;
	Clustering c_prime;
	for(int i=0;i<k;i++)
	{
		vector<set<Vertex>> x=c.c;	
		set_union(x[i].begin(), x[i].end(), to_merge.begin(), to_merge.end(), inserter(x[i], x[i].begin()));
		set<Vertex> total=x[0];
		for(int r=1;r<k;r++)
				set_union(total.begin(), total.end(), x[r].begin(), x[r].end(), inserter(total, total.begin()));
		
		Clustering candidate(G.get_subgraph(total),x);
		candidate.compute_nCut();
		if (candidate.normalized_cut <= min_cut_value){
			c_prime=candidate;
			min_cut_value=candidate.normalized_cut;
		}
	}
	return c_prime;
}

Clustering merge_Clustering(Graph G, Clustering c, Clustering d)
{
	Clustering c_prime=c;
	//cout<<"c_prime initial\n";
	//c_prime.print2();
	//cout<<endl;
	for(int i=0;i<k;i++){	
		c_prime = merge_Clustering_Cluster(G, c_prime, d, i);
		//cout<<"c_prime'"<<i<<" \n";
		//c_prime.print2();
		//cout<<endl;
	}
	return c_prime;
}

int main(int argc, char* argv[])
{
	ifstream label_stream("labels.in");
	for(int i = 0; i < 3891; i++)
	{
		label_stream >> labels[i];
	}
	label_stream.close();
	ifstream fin("input.in");
	int n_batches = 4;
	Graph G;
	int depth = 1;
	double t = 0.0;

	cout << "ARGC: " << argc << endl;
	if(argc >= 2)
	{
		n_batches = atoi(argv[1]);
		cout << "Number of batches: " << n_batches << endl;
	}

	if(argc >= 3)
	{
		depth = atoi(argv[2]);
		cout << "Depth : " << depth << endl;
	}

	int w, d, num_edges;
	int total_documents = 0;
	fin >> w >> d >> k >> num_edges;
	total_documents += d;
	for(int i = 0; i < w; i++)
	{
		G.add_vertex(Vertex(i, false));
	}
	for (int i = 0; i < num_edges; i++)
	{
		int u, v;
		double weight;
		fin >> u >> v >> weight;
		G.add_Edge(Vertex(u, false), Vertex(v, true), weight);
	}
	//cout<<"Initial Graph input done"<<endl;
	//cout<<"size of initial graph is "<<G.get_vlist().size() << endl << endl;

	//cout << words << endl;
	//exit(0);

	sp_mat a = G.get_word_by_document_matrix();
	vector<Vertex> order = G.get_order();
	Row<size_t> assignments = cluster_components(a,k);
	Clustering C(G, order, assignments);
	cout << "Clustering of initial graph done..." << endl << endl;
	C.print_confusion_matrix();
	cout << "---------------------------------------------------------------------------------" << endl;

	for (int b_num = 0; b_num < n_batches; b_num++)
	{
		int num_ins, num_del, n_edges;
		Batch B;
		fin >> num_ins >> num_del >> n_edges;
		total_documents += num_ins;
		for (int i = 0; i < num_ins; i++)
		{
			int u, is_doc;
			fin >> u >> is_doc;
			B.add_v(Vertex(u, is_doc));
		}

		for (int i = 0; i < n_edges; i++)
		{
			int u, v;
			double weight;
			fin >> u >> v >> weight;
			G.add_Edge(Vertex(u, false), Vertex(v, true), weight);
		}

		cout << "Graph size after batch " << b_num + 1 << " : " << G.get_vlist().size() << endl;
		G.print_words_and_docs();
		cout << endl;
		Graph G_sub = get_neighbourhood(G, B, depth, t);
		C.remove(G_sub.get_vlist()); // remove all vertexes found in neighbourhood , so that merging of new clusterings can be done later with this C clustering
		vector<Graph> G_list = get_connected_components(G_sub);
		//cout << " number of cc: " << G_list.size() << endl;

		for (int i = 0; i < G_list.size(); i++)
		{
			//cout << "Component " << i << endl;
			sp_mat b = G_list[i].get_word_by_document_matrix();
			vector<Vertex> order1 = G_list[i].get_order();
			Row<size_t> assignments1 = cluster_components(b,k);
			Clustering D(G_list[i], order1, assignments1);
			//cout<<"D1 size :"<<D.c[0].size()<<" D2 size: "<<D.c[1].size()<<endl;	
			//D.print2();		
			C = merge_Clustering(G, C, D);
			//C.print2();
			//exit(0);
		}
		C.print_cluster_size();
		C.print_confusion_matrix();

		cout << "---------------------------------------------------------------------------------" << endl;

		cout << endl;
		//cout << "merged size after " << b_num + 1 << " batch: " << C.num_of_vertices() << endl << endl;
		//exit(0);
		//C.compute_nCut();
		//C.print();
	}
	//C.print2();
	fin.close();
	

	int cf[k][k];
	for (int i = 0; i < k; i++)
		for (int j = 0; j < k; j++)
			cf[i][j] = 0;
	ifstream fin1;
	fin1.open("labels.in");
	for (int i = 0; i < total_documents; i++)
	{
		int cur_label;
		fin1 >> cur_label;
		cf[cur_label][C.return_label(Vertex(i, true))]++;
	}
	cout << "Confusion Matrix: " << endl << endl;
	for (int i = 0; i < k; i++){
		for (int j = 0; j < k; j++){
			cout<<cf[i][j]<<" ";
		}
		cout<<endl;
	}
	fin1.close();
}