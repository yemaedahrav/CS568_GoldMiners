#include <mlpack/methods/kmeans/kmeans.hpp>
#include <bits/stdc++.h>

using namespace std;
using namespace arma;
using namespace mlpack::kmeans;
//apt install libmlpack-dev mlpack-bin libarmadillo-dev
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
    for (uword i = 0; i < a.n_cols; i++)
    {
        for (uword j = 0; j < a.n_rows; j++)
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
{
    int num_of_words;
    int num_of_documents;
    int num_of_clusters;
    mat a;
    bool fit_complete;

    base_clustering(int num_of_words, int num_of_documents, int num_of_clusters, mat a)
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


}

class spectral_coclustering: public base_clustering
{
public:
    Row<size_t> assignments;

    //Constructor
    spectral_coclustering(int num_of_words, int num_of_documents, int num_of_clusters, mat a):
                            base_clustering(num_of_words,num_of_documents,num_of_clusters,a){}

    void fit()
    {
        mat d1(num_of_words, num_of_words, fill::zeros);
        mat d2(num_of_documents, num_of_documents, fill::zeros);
        d1=scale_normalize(a,num_of_words,num_of_documents,0);
        d2=scale_normalize(a,num_of_words,num_of_documents,1);
        mat an = (d1 * a) * d2;
        //cout << d1 << endl << endl;
        //cout << d2 << endl;

        //an is the normalized matrix

        //Number of singular values to find
        int l = ceil(log2(num_of_clusters));
        int l_final = min(l + 1, min(num_of_words, num_of_documents));

        //left and right singular matrices
        mat U, V;

        //vector of singular values
        vec s;

        SVD(U,s,V,an,l_final);

        //cout << endl  << endl;
        //cout << U << endl << s << endl << V << endl;

        mat u1 = d1 * U.cols(1, l_final - 1);
        //cout << u1 << endl;

        mat u2 = d2 * V.cols(1, l_final - 1);
        //cout << u2 << endl;

        mat z1 = join_cols(u1, u2);
        //cout << z1 << endl;

        // The assignments will be stored in this vector.
        KMeans<> k;
        k.Cluster(z1.t(), num_of_clusters, assignments);
        fit_complete = true;
    }

    void print_assignments()
    {
        cout<<assignments;
    }

    void get_assignments()
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
    fstream fin("data.in");
    //word by document
    int w = 0;
    int d = 0;
    int k = 0;

    fin >> w >> d >> k;

    mat a(w, d);
    mat &b = a;

    string temp;
    double val;
    for (uword i = 0; i < w; i++)
    {
        for (uword j = 0; j < d; j++)
        {
            fin >> temp;
            val = process_inp(temp);
            a(i, j) = val;
        }
    }


    spectral_coclustering x(w, d, k, a);
    x.fit();
    Row<size_t> assignments=x.get_assignments();
    Row<size_t> true_labels(w+d);

    fstream fin2("labels.in");
    for(uword i=0;i<w+d;i++)
        fin2>>true_labels(i);
    fin2.close();

    mat conf_matrix(k,k,fill::zeros);
    for(uword i=0;i<w+d;i++)
        conf_matrix(labels(i),assignments(i))++;


    cout<<conf_matrix(i,j)<<" ";

    return 0;
}
