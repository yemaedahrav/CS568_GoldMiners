### CS568 GoldMiners
This repository contains all the coursework related to the CS568: Data Mining elective course offered at IITGuwahati. This is maintained by the team GoldMiners

The latest version of our code is in Week4/kpartition.cpp

## Installation

Installation of Armadillo, Boost and mlpack and other required packages is needed for this implementation.
The following are the commands for their installation on Linux

```
$ sudo apt-get install libboost-all-dev
$ apt install libmlpack-dev mlpack-bin libarmadillo-dev
$ sudo apt-get install libblas-dev liblapack-dev
$ apt-get install libatlas-base-dev
```

## Compilation and Execution

Instructions for compilation and executing the code:
While executing the following commands, one should have their input files named input.in ( containing word document matri) and labels.in( true labels) inside the pwd. Datasets are availabe in test_Datasets folder for this purpose.
```
$ g++ filename.cpp -larmadillo -lmlpack
$ ./a.out 
```

## File Structure

```
1. Data_Processing_files_and_code
This contains files like *combine_matrix* , *combine_words* , *2D_to_coordinate* format conversion code files used to preprocess datasets for the algorithm
	
2. Test_Datasets
        2.1 Datasets_2d
	Contains *MED_CISI, MED_CRAN, MED_CISI_CRAN* and the *toy_Dataset* for this week in mxn matrix format.
	
	2.2 Datasets_CC
	Contains all previously stated datasets in coordinate format
	
3.Intermediate_files_dense
These were obtained by running the code which is using dense matrix and reading input from 2D format. These are the ones to be used for presentation as all the files have a simple 2D matrix which is easier to analyze. But our final code remains to be the sparse aka updated one.
	
4.Intermediate_files_sparse
Obtained by running current sparse version
	
5.code_coordinate_format_input_and_all_matrices_to_sparse
Updated code
	
6.code_with_2D_input_and_dense_matrices*
Old 2D code
```	
Note: Kparitition.cpp in Week4 is the latest updated, fastest running code for now. It is the one on which memory profiling has been done
