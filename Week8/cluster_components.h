#include <bits/stdc++.h>
#include <mlpack/methods/kmeans/kmeans.hpp>
#include <sys/types.h>
#include <unistd.h>
using namespace std;
using namespace arma;
using namespace mlpack::kmeans;
Row<size_t> cluster_components(sp_mat& a);
