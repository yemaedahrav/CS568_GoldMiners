#include <bits/stdc++.h>
#include <mlpack/methods/kmeans/kmeans.hpp>
#include <sys/types.h>
#include <unistd.h>
#include "cluster_components.h"

using namespace std;
using namespace arma;
using namespace mlpack::kmeans;

clock_t start_time;
void print_maxmem()
{
    pid_t p = getpid();
    string pid = to_string(p);
    string command = "grep VmPeak /proc/"+pid+"/status";
    char * c = &command[0];
    system(c);
}
void start_sw()
{
    start_time = clock();
}
void stop_sw(string a)
{
    clock_t elapsed_time = clock()-start_time;
    //cout << "Time used by " + a + " " << (1000*elapsed_time)/CLOCKS_PER_SEC << " ms \n";
}
void scale_normalize(sp_mat &d,sp_mat &a,int& num_of_words,int& num_of_documents,int type)
{
    if(type==0)
    {
        vector<double> sum(num_of_words+1,0);
        for(auto it = a.begin(); it != a.end(); ++it)
        {
            sum[it.row()] += *it;
        }

        for (uword i = 0; i < num_of_words; i++)
        {
            if(abs(sum[i]) <= 0.00001)
            {
                d(i,i) = 10000000;
                continue;
            }
            d(i, i) = 1.0 / sqrt(sum[i]);
        }
    }

    else
    {
        vector<double> sum(num_of_documents+1,0);
        for(auto it = a.begin(); it != a.end(); ++it)
        {
            sum[it.col()] += *it;
        }
        for (uword i = 0; i < num_of_documents; i++)
        {
                if(abs(sum[i]) <= 0.00001)
                {
                    d(i,i) = 10000000;
                    continue;
                }
                d(i, i) = 1.0 / sqrt(sum[i]);
        }
    }
}

class base_clustering
{   
public:
    int num_of_words;
    int num_of_documents;
    int num_of_clusters;
    sp_mat a;
    bool fit_complete;

    base_clustering(int num_of_words, int num_of_documents, int num_of_clusters,sp_mat& a)
    {
        this->num_of_words = num_of_words;
        this->num_of_documents = num_of_documents;
        this->num_of_clusters = num_of_clusters;
        fit_complete = false;
        this->a = a;
    }

    void SVD(sp_mat& U,vec& s,sp_mat& V,sp_mat& an,int& l_final)
    {
        mat dense_U(U),dense_V(V);
        svds(dense_U, s,dense_V,an, l_final);
        U=sp_mat(dense_U);
        V=sp_mat(dense_V);
    }
};

class spectral_coclustering: public base_clustering
{
public:
    Row<size_t> assignments;

    //Constructor
    spectral_coclustering(int num_of_words, int num_of_documents, int num_of_clusters,sp_mat& a):
                            base_clustering(num_of_words,num_of_documents,num_of_clusters,a){}

    void fit()
    {
        sp_mat d1(num_of_words, num_of_words);
        sp_mat d2(num_of_documents, num_of_documents);
        start_sw();
        scale_normalize(d1,a,num_of_words,num_of_documents,0);
        scale_normalize(d2,a,num_of_words,num_of_documents,1);
        start_sw();
        sp_mat an = (d1 * a) * d2;
        

        //Number of singular values to find
        int l = ceil(log2(num_of_clusters));
        cout <<"l : " <<  l << endl;
        cout << "words : " << num_of_words << endl;
        cout << "docs : " << num_of_documents << endl;
        int l_final = min(l + 1, min(num_of_words, num_of_documents));
        if(l_final <= 1)
        {
            assignments.resize(num_of_words+num_of_documents);
            for(int i = 0; i < num_of_words + num_of_documents; i++)
            {   
                assignments[i] = 0;
            }
            fit_complete = true;

        }
        else
        {
        //left and right singular matrices
        sp_mat U, V;

        //vector of singular values
        vec s;
        stop_sw("create normalized");
        start_sw();
        SVD(U,s,V,an,l_final);
        stop_sw("svd");
	    cout << U << endl;
        sp_mat u1 = d1 * U.cols(1, l_final - 1);
        //cout << u1 << endl;

        sp_mat u2 = d2 * V.cols(1, l_final - 1);
        //cout << u2 << endl;

        sp_mat z1 = join_cols(u1, u2);
        cout << z1 << endl;
        // The assignments will be stored in this vector.
        start_sw();
        KMeans<> k;
        k.Cluster(mat(z1.t()), num_of_clusters, assignments);
        fit_complete = true;
        stop_sw("kmeans");
        }
    }

    void print_assignments()
    {
        //cout<<assignments;
    }

    Row<size_t> get_assignments()
    {
        return assignments;
    }

    void print()
    {
        //cout << a << endl;
    }
};


Row<size_t> cluster_components(sp_mat& a)
{
    int w = a.n_rows;
    int d = a.n_cols;
    int k = 2;
    
    //cout << " w d " << w << " " << d << endl;
    //cout << a << endl;
    spectral_coclustering x(w, d, k, a);
    x.fit();
    Row<size_t> assignments=x.get_assignments();
    return assignments;
    /*
    Mat<size_t> conf_matrix(k,k,fill::zeros);
    for(uword i=0;i<d;i++)
        conf_matrix(assignments(i+w),true_labels(i))++;
	
    fout.open("output.in");
    fout<< "The confusion matrix for the obtained clustering is: " <<endl; 
    fout<<conf_matrix<<" ";
    fout.close();

    //cout << "\nMax memory used at the end of the program\n";
    //print_maxmem();
    return 0; */
}

// int main(){
//  return 0;

// }