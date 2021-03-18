#include <bits/stdc++.h>
#include <mlpack/methods/kmeans/kmeans.hpp>
#include <sys/types.h>
#include <unistd.h>
#include "cluster_components.h"
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
	set<Vertex> c0;
	set<Vertex> c1;
	double cut_01 = 0, weight_0 = 0, weight_1 = 0;
	double normalized_cut;
	Graph G;

	// Normalized cut value, weight of cut 0, weight of cut 1
	Clustering(Graph G, vector<Vertex> vertices, Row<size_t> assignments)
	{
		this->G = G;
		for (int i = 0; i < vertices.size(); i++)
		{
			if (assignments[i] == 0)
				c0.insert(vertices[i]);
			else if (assignments[i] == 1)
				c1.insert(vertices[i]);
			else
				assert(0);
		}
	}
	Clustering(vector<Vertex> vertices, Row<size_t> assignments)
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			if (assignments[i] == 0)
				c0.insert(vertices[i]);
			else if (assignments[i] == 1)
				c1.insert(vertices[i]);
			else
				assert(0);
		}
	}
	Clustering(Graph G, set<Vertex> c0, set<Vertex> c1)
	{
		this->c0 = c0;
		this->c1 = c1;
		this->G = G;
	}

	void remove(set<Vertex> vset)
	{
		for (auto u : vset)
		{
			if (c0.count(u))
				c0.erase(u);
			else if (c1.count(u))
				c1.erase(u);
		}
	}

	int num_of_vertices()
	{
		return (int)(c0.size() + c1.size());
	}

	void print2()
	{
		cout << "The documents in cluster 1 are: \n ";
		for (auto x : c0)
			if (x.is_doc)
				cout << x.num << " ";
		cout << endl << endl;
		cout << "The documents in cluster 2 are: \n";
		for (auto x : c1)
			if (x.is_doc)
				cout << x.num << " ";
		cout << endl << endl;
	}
	int return_label(Vertex v)
	{
		if (c0.count(v))
			return 0;
		else if (c1.count(v))
			return 1;
		else
			assert(0);
		return -1;
	}
	// can make mistakes if true labels and assignments are exact opposite
	bool match_label(Vertex u, int label)
	{
		if (label == 1)
			if (c0.count(u))
				return true;
			else
				return false;
		else if (c1.count(u))
			return true;
		else
			return false;
	}

	void compute_nCut()
	{
		auto sub_adj_list = G.get_adj_list();
		normalized_cut = weight_0 = weight_1 = cut_01 = 0;
		for (auto u : sub_adj_list)
		{
			for (auto v : u.second)
			{
				auto n0 = u.first;
				auto n1 = v.v;

				if (!(c0.count(n0) || c1.count(n0)))
					assert(0);
				if (!(c0.count(n1) || c1.count(n1)))
					assert(0);

				if (c0.count(n0) && c0.count(n1))
				{
					weight_0 += v.weight;
				}
				else if (c1.count(n0) && c1.count(n1))
				{
					weight_1 += v.weight;
				}
				else
				{
					cut_01 += v.weight;
				}
			}
			//Checking if weights are zero.
		}
		if (abs(weight_0) <= 0.0000001 || abs(weight_1) <= 0.0000001)
		{
			//cout << "Normalized_cut weight = 0 " << endl;
			weight_0 = (abs(weight_0) <= 0.0000001) ? 0.0000001 : weight_0;
			weight_1 = (abs(weight_1) <= 0.0000001) ? 0.0000001 : weight_1;
			cut_01 = 100.0; // it is must as otherwise normalized cut will be zero instead of inf
		}
		normalized_cut = cut_01 * (1 / weight_0 + 1 / weight_1);
	}

	void print()
	{
		cout << "NC : " << normalized_cut << endl
			 << "W0 : " << weight_0 << endl
			 << "W1 : " << weight_1 << endl
			 << "C01: " << cut_01 << endl;
	}
};

Clustering merge_Clustering_Cluster(Graph G, Clustering c, Clustering d, int type)
{
	//create graph, vertex vector, assignments vector
	set<Vertex> x0, x1, y0, y1, total, to_merge;
	if (type == 0)
	{
		to_merge = d.c0;
	}
	else
	{
		to_merge = d.c1;
	}

	set_union(c.c0.begin(), c.c0.end(), to_merge.begin(), to_merge.end(), inserter(x0, x0.begin()));
	x1 = c.c1;

	y0 = c.c0;
	set_union(c.c1.begin(), c.c1.end(), to_merge.begin(), to_merge.end(), inserter(y1, y1.begin()));

	set_union(x0.begin(), x0.end(), x1.begin(), x1.end(), inserter(total, total.begin()));
	Clustering candidate1(G.get_subgraph(total), x0, x1);
	Clustering candidate2(G.get_subgraph(total), y0, y1);

	candidate1.compute_nCut();
	candidate2.compute_nCut();

	if (candidate1.normalized_cut <= candidate2.normalized_cut)
	{
		return candidate1;
	}
	else
	{
		return candidate2;
	}
}

Clustering merge_Clustering(Graph G, Clustering c, Clustering d)
{
	Clustering c_prime = merge_Clustering_Cluster(G, c, d, 0);
	Clustering final = merge_Clustering_Cluster(G, c_prime, d, 1);
	return final;
}

int main()
{
	ifstream fin("input.in");
	int n_batches = 3;
	Graph G;
	int depth = 2;
	double t = 0.0;
	int w, d, k, num_edges;
	int total_documents = 0;
	fin >> w >> d >> k >> num_edges;
	total_documents += d;
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
	Row<size_t> assignments = cluster_components(a);
	Clustering C(G, order, assignments);
	cout << "Clustering of initial graph done..." << endl << endl;

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

		Graph G_sub = get_neighbourhood(G, B, depth, t);
		C.remove(G_sub.get_vlist()); // remove all vertexes found in neighbourhood , so that merging of new clusterings can be done later with this C clustering
		vector<Graph> G_list = get_connected_components(G_sub);
		//cout << " number of cc: " << G_list.size() << endl;

		for (int i = 0; i < G_list.size(); i++)
		{
			cout << "Component " << i << endl;
			sp_mat b = G_list[i].get_word_by_document_matrix();
			vector<Vertex> order1 = G_list[i].get_order();
			Row<size_t> assignments1 = cluster_components(b);
			Clustering D(G_list[i], order1, assignments1);
			C = merge_Clustering(G, C, D);
		}
		cout << "merged size after " << b_num + 1 << " batch: " << C.num_of_vertices() << endl << endl;
		//C.compute_nCut();
		//C.print();
	}
	C.print2();
	fin.close();
	

	int cf[2][2];
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			cf[i][j] = 0;
	ifstream fin1;
	fin1.open("labels.in");
	for (int i = 0; i < total_documents; i++)
	{
		int cur_label;
		fin1 >> cur_label;
		cf[cur_label][C.return_label(Vertex(i, true))]++;
	}
	cout << "---------------------------------------------------------------------------------" << endl;
	cout << "Confusion Matrix: " << endl << endl;
	cout << cf[0][0] << " " << cf[0][1] << endl
		 << cf[1][0] << " " << cf[1][1] << endl;

	fin1.close();
}
