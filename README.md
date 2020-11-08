# Uintah that uses Hypre's MPI EndPoint version

## Hypre using MPI End Points:
This is the modified Uintah that uses Hypre's MPI EndPoint version. More details are presented in "Sahasrabudhe D., Berzins M. (2020) Improving Performance of the Hypre Iterative Solver for Uintah Combustion Codes on Manycore Architectures Using MPI Endpoints and Kernel Consolidation. In: Krzhizhanovskaya V. et al. (eds) Computational Science – ICCS 2020. ICCS 2020. Lecture Notes in Computer Science, vol 12137. Springer, Cham. https://doi.org/10.1007/978-3-030-50371-0_13"  
Hypre's EndPoint version is present at https://github.com/damu1000/hypre_ep

## Installation  

git clone https://github.com/damu1000/Uintah.git uintah_src  
mkdir 1_mpi  
mkdir 2_ep  
mkdir work_dir

# Install MPI Only version  
cd 1_mpi  
#configure line is for intel compilers and KNL. --enable-kokkos will automatically download and install kokkos.  
../uintah_src/src/configure --enable-64bit --enable-optimize="-std=c++11 -O2 -g -fopenmp -mkl -fp-model precise -xMIC-AVX512" --enable-assertion-level=0 --with-mpi=built-in --with-hypre=../../hypre_cpu/src/build/ LDFLAGS="-ldl" --enable-examples --enable-arches --enable-kokkos  
make -j16 sus  
cp ./StandAlone/sus ../work_dir/1_mpi  

# Install EP version  
cd ../2_ep  
#Note the difference between mpi only line and ep line: --with-hypre option is changed from hypre_cpu to hypre_ep and CXXFLAGS is added.  
../uintah_src/src/configure --enable-64bit --enable-optimize="-std=c++11 -O2 -g -fopenmp -mkl -fp-model precise -xMIC-AVX512" --enable-assertion-level=0 --with-mpi=built-in --with-hypre=../../hypre_ep/src/build/ LDFLAGS="-ldl" CXXFLAGS="-I../../hypre_ep/src/ -DUSE_MPI_EP" --enable-examples --enable-arches --enable-kokkos  
make -j16 sus  
cp ./StandAlone/sus ../work_dir/2_ep

# Run

#There are two examples - solvertest1.ups and RMCRT_ML_solvertest.ups: solvertest1 is a lapace equation built within Uintah and solved using Hypre. RMCRT_ML_solvertest is a dummy combination of the RMCRT task within Uintah followed by solvertest1.  
cd ../work_dir  
#copy both input spec files:  
cp ../uintah_src/src/StandAlone/inputs/Examples/solvertest1.ups ./  
cp ../uintah_src/src/StandAlone/inputs/Examples/RMCRT_ML_solvertest.ups ./  

#modify both input files as follows:  
#1. Ensure ups file has: <outputTimestepInterval>0</outputTimestepInterval> and <checkpoint cycle = "0" interval = "0"/>. This will turn off the exporting of the output to the disk. Keep the output on if changes made within Uintah or Hypre are to be tested.  
#2. Update <resolution> and <patches> as needed. <resolution> is the total number of cells within the domain and <patches> gives the nuber patches in three dimensions. e.g. if <resolution> is set to [128,128,128] and <patches> is set to [4,4,4], then there will 64 patches in total (4 in each dimension) and each patch will have the size [128/4,128/4,128/4] i.e. 32 cubed.  
#3. RMCRT_ML_solvertest.ups has two levels of the mesh. Each level has its own resolution settings. The thumb rule is to reduce fine level (level 1) resolution by a factor of 4 while setting coarse level (level 0) resolution.  
#4. Adjust max_Timesteps and maxiterations to the required value.

#The following examples are to occupy 64 cores on a KNL node:  
export OMP_NESTED=true
export OMP_PROC_BIND=spread,spread
export OMP_PLACES=threads

#run the MPI only version. Ensure there are at least 64 patches:  
export OMP_NUM_THREADS=1  
mpirun -np 64 ./1_mpi -npartitions 1 -nthreadsperpartition 1 <input filename>.ups  

#run the MPI EP version. Ensure there are at least 64 patches:  
export OMP_NUM_THREADS=16  
export HYPRE_THREADS=4,4  
export HYPRE_BINDING=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63  
#HYPRE_THREADS indicate number of thread teams and threads per team used by hypre. teams x threads MUST match OMP_NUM_THREADS  
mpirun -np 4 ./2_ep -npartitions 16 -nthreadsperpartition 1 <input filename>.ups  



