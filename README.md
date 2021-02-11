# CS568_GoldMiners
This repository contains all the coursework related to the CS568: Data Mining elective course offered at IITGuwahati. This is maintained by the team GoldMiners

The latest version of our code is in Week4/kpartition.cpp


Installation of Armadillo and mlpack and other required packages is needed for this implementation.
The following are the commands for their installation on Linux

$ apt install libmlpack-dev mlpack-bin libarmadillo-dev
$ sudo apt-get install libblas-dev liblapack-dev
$ apt-get install libatlas-base-dev


Instructions for compilation and executing the code:
While executing the following commands, one should have their input files named input.in ( containing word document matri) and labels.in( true labels) inside the pwd. Datasets are availabe in test_Datasets folder for this purpose.

$ g++ filename.cpp -larmadillo -lmlpack
$ ./a.out 


