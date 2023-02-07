# DLLSMA: Dual-level Local Search Memetic Algorithm for VRP with Time Windows

This solver

# References

DIMACS 12th Implementation Challenge workshop paper:
Team HustSmart: Zhouxing Su, Yunhao Li, Shihao Huang, Zhipeng Lü, Junwen Ding, Fuda Ma, Weibo Lin
[https://drive.google.com/file/d/1dDh3qiVbD5kexKbgEHQtkuSk3LBMjUOC/view?usp=sharing](https://drive.google.com/file/d/1dDh3qiVbD5kexKbgEHQtkuSk3LBMjUOC/view?usp=sharing)


# Compiling && Running

## linux:build with CMake

You need to create a `Deploy` folder to save the execute file(if `Deploy` is not exist).
```shell
mkdir Deploy
mkdir build
cd build
cmake ..
make
```

To running under linux, An example as follows.
```shell
cd Deploy
./DLLSMA -instancePath ../Instances/Homberger/C1_8_2.txt
```

The optional command line parameters.
```shell
[-time:int] the time limit
[-bksDataFileBasePath:string] the bks file path
[-seed:int] the seed of random
[-customersWeight0:int] the weight of when eject customers from solution
[-customersWeight0:int] the weight of when insert customers to solution
[-tag:string] the tag of one run to distinguish each run
[-psizemulpsum:int]
[-ejectLSMaxIter:int] the max iter to limit ejectLocalSearch
[-dimacsGo:bool] to close the info log and print the bks format dimacs style
[-infoFlag:bool] switch to control the std output of info
[-debugFlag:bool] switch to control the std output of debug
[-errorFlag:bool] switch to control the stderr output of error info
```


## windows:build with visual studio

Open the solution with `BuildwithVS2022/BuildwithVS2022.sln`.  There are two projects(`linux` and `windows`) in the solution.If you install the wsl on your computer you can build the `linux` project.The windows project can build directly.




# Test with GoogleTest

The test case files and project files is in `Test` folder.

## linux:

You need to install `GooleTest`:

[https://github.com/google/googletest/tree/main/googletest](https://github.com/google/googletest/tree/main/googletest)

Build the test project as follows.  You need to create a `Deploy` folder to save the test execute file(if `Deploy` is not exist in `Test` folder).

```shell
cd Test
mkdir Deploy
mkdir build
cd build
cmake ..
make
```
Running the test with:

```shell
cd Deploy
./vrpTest
```

## windows

The folder `TestBuildwithVS2022` is a `Google Test Project` create with visuall studio.To open the project with opening the `TestBuildwithVS2022.sln`.


# Code structure

* Algorithm_Parameters:the parameters of the solver.
* EAX: 