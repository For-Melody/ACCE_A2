#!/bin/sh
#SBATCH --time=00:15:00         
#SBATCH --nodes=1              

#generate ref output for 10 iterations

ITER=10

make KMEANS_seq

./KMEANS_seq ../test_files/input2D_2clusters.in 2 $ITER 0 0 ../test_files/2D_2clusters_10iter.out

./KMEANS_seq ../test_files/input2D_10clusters.in 10 $ITER 0 0 ../test_files/2D_10clusters_10iter.out

./KMEANS_seq ../test_files/input10D_10clusters.in 10 $ITER 0 0 ../test_files/10D_10clusters_10iter.out

./KMEANS_seq ../test_files/input100D_300clusters.in 300 $ITER 0 0 ../test_files/100D_300clusters_10iter.out

      
