# How to compile
To compile both the sequential version as well as the parallel version of the algorithm, just use

```
make all
```

To compile only the sequential version or only the parellel one, use, respectively

```
make seq
```

```
make par
```

For development purposes, it's recommended to build the project with

```
make build
```

Though it has some dependencies, since it runs a formatter and a linter before compilation. 
For compacting it into a .zip file, use

```
make zip
```

To clean up the directory from binaries and unused files, use

```
make clean
```

To change the compiler being used in the make statements, use

```
make <statement> COMPILER=<custom compiler>
```

To run benchmarks for comparison of performance of the codes and generate plots, use

```
./benchmark.sh
```

Note that it calls a python script, consider using a python environment for that.

Below is the list of dependencies:
- gcc            (replaceable with no guarantee)
- clang-format   (dev)
- cppcheck       (dev)
- python dependencies specified in "requirements.txt"
