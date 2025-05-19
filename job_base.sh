#!/bin/sh
#SBATCH --time=00:45:00         
#SBATCH --nodes=1              

module load vivado/2024.1

vitis_hls -f run_KMEANS_HLS_base.tcl

      
