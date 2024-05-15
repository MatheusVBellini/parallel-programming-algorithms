In order to compile the programs, use

```
make all
```

To run them, use

```
make run NP=<int>
```

Where <int> is the number of processes you want to instanciate. This option defaults to 2.

To remove binaries from the folder, use

```
make clean
```

The code depends on MPI to work, ensure any kind of MPI implementation is available in your computer.
