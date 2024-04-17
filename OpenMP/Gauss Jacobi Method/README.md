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

Below is the list of dependencies:
- gcc
- clang-format   (dev)
- cppcheck       (dev)

