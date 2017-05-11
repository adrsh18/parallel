# Delaunay Triangulation in Parallel
A parallel implementation of Delaunay Triangulation in MPI. 

**Reference for Serial Algorithm:** Primitives for the Manipulation of General Subdivisions and the Computation of Voronoi Diagrams – Guibas, L. and Stolfi, J.

### Compile
Get the files from the **source** directory and run *build.sh* OR

```bash
mpicc -o delaunay-triangulation logger.c utils.c delaunay.c main.c -lm -g
```
**Note:** Executable available in **binaries** directory was compiled using Intel MPI Library 2017 which follows MPI 3.1 standards.

### Run
```bash
mpirun -np <num_of_processors> ./delaunay-triangulation <input_file> <number_of_points_in_input> <output_files>
```

Example:
```bash
mpirun -np 8 ./delaunay-triangulation points.csv 1048576 output-edges.txt
```

### Auxillary Scripts

**point-generator.py** can generate random points such that coordinates follow Bivariate Uniform Distribution
```bash
python point-generator.py 1048576 points.csv
```

**validator.py** can validate the edges generated from this program against *scipy.spatial.Delaunay*
```bash
python validator.py input_points.csv generated_edges.txt
```
