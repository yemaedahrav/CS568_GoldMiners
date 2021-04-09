#include <mlpack/methods/kmeans/kmeans.hpp>
#include <bits/stdc++.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;
using namespace arma;
using namespace mlpack::kmeans;

ofstream fout;
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
    cout << "Time used by " + a + " " << (1000*elapsed_time)/CLOCKS_PER_SEC << " ms \n";
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
        fout.open("Intermed_Matrices.txt");
        fout <<"The D1 matrix is:"<<endl<<d1 << endl;
        fout <<"The D2 matrix is:"<<endl<<d2 << endl;
	fout <<"Normalized matrix An(=D1*A*D2) is:"<<endl<<an<<endl;
        fout.close();
        

        //Number of singular values to find
        int l = ceil(log2(num_of_clusters));
        int l_final = min(l + 1, min(num_of_words, num_of_documents));

        //left and right singular matrices
        sp_mat U, V;

        //vector of singular values
        vec s;
        stop_sw("create normalized");
        start_sw();
        SVD(U,s,V,an,l_final);
        stop_sw("svd");
	    fout.open("SVD_output.txt");
        fout << "The matrix U having left singular vectors is:"<<endl<< U << endl<<"The matrix V of right singular vectors is"<<endl<< V << endl <<"The vector s of singular values is"<<endl << s << endl ;

        sp_mat u1 = d1 * U.cols(1, l_final - 1);
        //cout << u1 << endl;

        sp_mat u2 = d2 * V.cols(1, l_final - 1);
        //cout << u2 << endl;

        sp_mat z1 = join_cols(u1, u2);
        fout<<" The matrix z having constructed from U and V and having the l(logk)="<<l<<" eigen vectors of the laplacian is:"<<endl<<z1<<endl;
        fout.close();

        // The assignments will be stored in this vector.
        start_sw();
        KMeans<> k;
        k.Cluster(mat(z1.t()), num_of_clusters, assignments);
        fit_complete = true;
        stop_sw("kmeans");
    }

    void print_assignments()
    {
        cout<<assignments;
    }

    Row<size_t> get_assignments()
    {
        return assignments;
    }

    void print()
    {
        cout << a << endl;
    }
};


int main()
{
    
    fstream fin("input.in");
    //word by document
    int w = 0;
    int d = 0;
    int k = 0;

    fin >> w >> d >> k;

    sp_mat a(w, d);
    
    start_sw();
    int temp;
    while(fin>>temp){
	int r,c;
	r=temp;
	fin>>c;
	float cur_ele;
	fin>>cur_ele;
	a(r,c)=cur_ele;
    }
    stop_sw("input");
   
    spectral_coclustering x(w, d, k, a);
    
    x.fit();
    Row<size_t> assignments=x.get_assignments();
    fout.open("Cl_Assnmnts.txt");
    fout <<"The cluster assignments are: "<<endl;
    fout<<assignments<<endl;
    fout.close();
    
    Row<size_t> true_labels(w+d);
    fstream fin2("labels.in");
    for(uword i=0;i<d;i++)
        fin2>>true_labels(i);
    fin2.close();
    
    
    Mat<size_t> conf_matrix(k,k,fill::zeros);
    for(uword i=0;i<d;i++)
        conf_matrix(assignments(i+w),true_labels(i))++;
	
    fout.open("output.in");
    fout<< "The confusion matrix for the obtained clustering is: " <<endl; 
    fout<<conf_matrix<<" ";
    fout.close();

    cout << "\nMax memory used at the end of the program\n";
    print_maxmem();
    return 0;
}
