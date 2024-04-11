# How to Compile
Each directory in the root directory has its own `Makefile` in it, containg the proper directives for each kind of lib or flag used. To compile using any of the Makefiles use:

```
make FILE=<path_to_file>
```
 
Where `<path_to_file>` is the path to the file inside the current directory. You can also clean up the directory using

```
make clean
```
