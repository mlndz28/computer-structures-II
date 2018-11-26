Matrix multiplier using MPI
==========

Multiplies a vector and a matrix using MPI to communicate between parallel processes.

## Usage

```sh
make	# compile using mpic++
mpirun -n <number of processes> bin/mult -s <matrix width and length>
```
