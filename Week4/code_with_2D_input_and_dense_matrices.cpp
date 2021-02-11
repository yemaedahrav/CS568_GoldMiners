#include <mlpack/methods/kmeans/kmeans.hpp>
#include <bits/stdc++.h>

using namespace std;
using namespace arma;
using namespace mlpack::kmeans;
//apt install libmlpack-dev mlpack-bin libarmadillo-dev

ofstream fout;

double process_inp(string a)
{
    int k = a.size();
    if (a[k - 1] == ',')
    {
        a.pop_back();
    }
    double val = stod(a);
    return val;
}
bool isZero(double a, double threshold = 0.000001)
{
    if (a > threshold || a < (0 - threshold))
    {
        return false;
    }
    else
        return true;
}

bool isSparse(const mat &a)
{

    int ctr = 0;
    for (uword i = 0; i < a.n_rows; i++)
    {
        for (uword j = 0; j < a.n_cols; j++)
        {
            ctr += (isZero(a(i, j)) == false);
        }
    }
    if (ctr <= max(a.n_cols, a.n_rows))
        return true;
    else
        return false;
}

mat scale_normalize(mat a,int num_of_words,int num_of_documents,int type)
{
    if(type==0)
    {
        mat d1(num_of_words, num_of_words, fill::zeros);
        for (uword i = 0; i < num_of_words; i++)
        {
            double sum = 0;
            for (uword j = 0; j < num_of_documents; j++)
                sum += a(i, j);
            d1(i, i) = 1.0 / sqrt(sum);
        }
        return d1;
    }

    else
    {
        mat d2(num_of_documents, num_of_documents, fill::zeros);
        for (uword i = 0; i < num_of_documents; i++)
        {
            double sum = 0;
            for (uword j = 0; j < num_of_words; j++)
                sum += a(j, i);

            d2(i, i) = 1.0 / sqrt(sum);
        }
        return d2;
    }
}

class base_clustering
{   public:
    int num_of_words;
    int num_of_documents;
    int num_of_clusters;
    mat a;
    bool fit_complete;

    base_clustering(int num_of_words, int num_of_documents, int num_of_clusters,mat a)
    {
        this->num_of_words = num_of_words;
        this->num_of_documents = num_of_documents;
        this->num_of_clusters = num_of_clusters;
        fit_complete = false;
        this->a = a;
    }

    void SVD(mat& U,vec& s,mat& V,mat& an,int& l_final)
    {
        if (isSparse(an))
        {
            //cout << "sparse";
            sp_mat sparse_an(an);
            svds(U, s, V, sparse_an, l_final);
        }
        else
        {
            //cout << "not sparse";
            svd(U, s, V, an);
        }
    }
};

class spectral_coclustering: public base_clustering
{
public:
    Row<size_t> assignments;

    //Constructor
    spectral_coclustering(int num_of_words, int num_of_documents, int num_of_clusters,mat a):
                            base_clustering(num_of_words,num_of_documents,num_of_clusters,a){}

    void fit()
    {
        mat d1(num_of_words, num_of_words, fill::zeros);
        mat d2(num_of_documents, num_of_documents, fill::zeros);
        d1=scale_normalize(a,num_of_words,num_of_documents,0);
        d2=scale_normalize(a,num_of_words,num_of_documents,1);
        mat an = (d1 * a) * d2;
        fout.open("Intermed_Matrices.txt");
        fout <<"The D1 matrix is:"<<endl<<d1 << endl;
        fout <<"The D2 matrix is:"<<endl<<d2 << endl;
	fout <<"Normalized matrix An(=D1*A*D2) is:"<<endl<<an<<endl;
        fout.close();
        

        //Number of singular values to find
        int l = ceil(log2(num_of_clusters));
        int l_final = min(l + 1, min(num_of_words, num_of_documents));

        //left and right singular matrices
        mat U, V;

        //vector of singular values
        vec s;

        SVD(U,s,V,an,l_final);
        
	fout.open("SVD_output.txt");
        fout << "The matrix U having left singular vectors is:"<<endl<< U << endl<<"The matrix V of right singular vectors is"<<endl<< V << endl <<"The vector s of singular values is"<< s << endl ;

        mat u1 = d1 * U.cols(1, l_final - 1);
        //cout << u1 << endl;

        mat u2 = d2 * V.cols(1, l_final - 1);
        //cout << u2 << endl;

        mat z1 = join_cols(u1, u2);
        fout<<" The matrix z having constructed from U and V and having the l(logk)="<<l<<" eigen vectors of the laplacian is:"<<endl<<z1<<endl;
        fout.close();

        // The assignments will be stored in this vector.
        KMeans<> k;
        k.Cluster(z1.t(), num_of_clusters, assignments);
        fit_complete = true;
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

    mat a(w, d);
    
    for (uword i = 0; i < w; i++)
    {
        for (uword j = 0; j < d; j++)
        {
            fin >> a(i,j);
        }
    }
    
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

    return 0;
}
