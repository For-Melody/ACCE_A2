# Create a new Vitis HLS project
open_project -reset KMEANS_HLS_optimized
set_top do_compute

add_files KMEANS.h
add_files KMEANS_HLS_optimized.cpp

add_files -tb test_KMEANS_optimized.cpp

# Add data files (for simulation)
add_files -tb [glob ./test_files/*.in]

# Create a solution
open_solution -reset "solution_KMEANS_HLS_optimized"

# Set the target FPGA part (modify as needed)
set_part {virtexuplusHBM}

# Set clock
create_clock -period 300MHz

# Run C simulation
csim_design -argv "input2D_10clusters.in 10 10 0 0 ./2D_10clusters.out"

# Run High-Level Synthesis (HLS)
csynth_design

# Run co-simulation (Attention, this might require a long time)
cosim_design -argv "input2D_10clusters.in 10 10 0 0 ./2D_10clusters.out"


exit

