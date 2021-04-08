#include <mlpack/methods/kmeans/kmeans.hpp>
#include <bits/stdc++.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;
using namespace arma;
using namespace mlpack::kmeans;

ofstream fout;
clock_t start_time;

/* Functions to get the peak memory and the time usage of the program */
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
{   clock_t elapsed_time = clock()-start_time;
    cout << "Time used by " + a + " " << (1000*elapsed_time)/CLOCKS_PER_SEC << " ms \n";
}

/* Function to normalise the data in the matrix A */
void scale_normalize(sp_mat &d, sp_mat &a, int& num_of_words, int& num_of_documents, int type)
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

/* Utility functions for K-Means clustering algorithm and the main function */
struct point
{
    vector <double> x;
    point(int size)
    {
        x.resize(size);
    }
    point()
    {

    }
};

point add(point a, point b)
{
    assert(a.x.size() == b.x.size());
    point c;
    c.x.assign(a.x.size(),0);

    for(int i = 0; i < a.x.size(); i++)
    {
        c.x[i] = a.x[i] + b.x[i];
    }
    return c;
}

double dist(point a, point b)
{
    assert(a.x.size() == b.x.size());
    double ans = 0;

    for(int i = 0; i < a.x.size(); i++)
    {
        ans += (a.x[i] - b.x[i])*(a.x[i] -b.x[i]);
    }
    return ans;
}

void kmeans_algorithm(mat z, int k, Row<size_t>& assignments)
{
    int iterations = 10;
    int r = z.n_rows;
    int c = z.n_cols;

    assignments.resize(r);

    vector <point> data;
    for(int i = 0; i < r; i++)
    {
        point temp(c);
        for(int j = 0; j < c; j++)
        {
            temp.x[j] = z(i,j);

        }
        data.push_back(temp);
    }

    vector <point> centroids;
    for(int i = 0; i < k; i++)
    {
        centroids.push_back(data[i]);
    }

    while(iterations--)
    {
        for(int i = 0; i < r; i++)
        {
            point a = data[i];
            double min = 1e18;
            int assign = -1;
            for(int j = 0; j < k; j++)
            {
                point center = centroids[j];
                double distance = dist(a,center);
                if( distance <= min)
                {
                    min = distance;
                    assign = j;
                }
            }
            assignments(i) = assign;
        }

        point zero;
        zero.x.assign(c,0);
        vector <point> new_centroids(k,zero);
        vector <int> freq(k,0);
        for(int i = 0; i < r; i++)
        {
            point a = data[i];
            int assign = assignments[i];

            new_centroids[assign] = add(a,new_centroids[assign]);
            freq[assign]++;
        }

        for(int i = 0; i < k; i++)
        {
            if(freq[i] == 0)
                continue;
            for(int j = 0; j < c; j++)
            {
                new_centroids[i].x[j] /= freq[i];
            }
        }
        centroids = new_centroids;
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

    base_clustering(int num_of_words, int num_of_documents, int num_of_clusters, sp_mat& a)
    {
        this->num_of_words = num_of_words;
        this->num_of_documents = num_of_documents;
        this->num_of_clusters = num_of_clusters;
        fit_complete = false;
        this->a = a;
    }

    void SVD(sp_mat& U, vec& s, sp_mat& V, sp_mat& an, int& l_final)
    {
        mat dense_U(U),dense_V(V);
        svds(dense_U, s, dense_V, an, l_final);
        U=sp_mat(dense_U);
        V=sp_mat(dense_V);
    }
};

class spectral_coclustering: public base_clustering
{
public:
    Row<size_t> assignments;

    spectral_coclustering(int num_of_words, int num_of_documents, int num_of_clusters,sp_mat& a):
                            base_clustering(num_of_words, num_of_documents, num_of_clusters, a){}

    void fit()
    {
        sp_mat d1(num_of_words, num_of_words);
        sp_mat d2(num_of_documents, num_of_documents);
       
        scale_normalize(d1,a,num_of_words,num_of_documents,0);
        scale_normalize(d2,a,num_of_words,num_of_documents,1);

        sp_mat an = (d1 * a) * d2;

        /* 

        ******************************** STORING INTERMEDIATE MATRICES TO FILE **********************************
        
        fout.open("Intermed_Matrices.txt");
        fout <<"The D1 matrix is:"<<endl<<d1 << endl;
        fout <<"The D2 matrix is:"<<endl<<d2 << endl;
	    fout <<"Normalized matrix An(=D1*A*D2) is:"<<endl<<an<<endl;
        fout.close();

        */
       
        /* Number of singular values to find from the SVD */
        int l = ceil(log2(num_of_clusters));
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
            /* left and right singular matrices, vector of singular values */
            sp_mat U, V;
            vec s;

            SVD(U,s,V,an,l_final);
            sp_mat u1 = d1 * U.cols(1, l_final - 1);
            sp_mat u2 = d2 * V.cols(1, l_final - 1);
            sp_mat z1 = join_cols(u1, u2);

            /* The K-Means algorithm */
            kmeans_algorithm(mat(z1), num_of_clusters, assignments);
            fit_complete = true;
        } 
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
    start_sw();

    /* Word by Document matrix A metadata */
    int w = 0;
    int d = 0;
    int k = 0;

    fstream fin("input.in");
    fin >> w >> d >> k;

    sp_mat a(w, d);
    
    int temp;
    while(fin>>temp){
        int r,c;
        r=temp;
        fin>>c;
        
        float cur_ele;
        fin>>cur_ele;
        a(r,c)=cur_ele;
    }
   
    spectral_coclustering x(w, d, k, a);
    
    x.fit();
    Row<size_t> assignments=x.get_assignments();
    fout.open("static_assignments.txt");
    for(uword i=0;i<d;i++)
        fout<<assignments(i+w)<<" ";
    fout.close();
    
    /* 
    ********************************* TRUE LABELS ************************************
    
    Row<size_t> true_labels(w+d);
    fstream fin2("labels.in");
    for(uword i=0;i<d;i++)
        fin2>>true_labels(i);
    fin2.close();

    */
        
    /*
    
    ****************** CONFUSION MATRIX CALCULATIONS AND FILE I/O ************************** 

    Mat<size_t> conf_matrix(k,k,fill::zeros);
    for(uword i=0;i<d;i++)
        conf_matrix(assignments(i+w),true_labels(i))++;
	
    fout.open("output.in");
    fout<< "The confusion matrix for the obtained clustering is: " <<endl; 
    fout<<conf_matrix<<" ";
    fout.close();
    
    */
    
    cout << "\nMax memory used at the end of the program\n";
    print_maxmem();

    stop_sw("Complete Algorithm");

    return 0;
}
