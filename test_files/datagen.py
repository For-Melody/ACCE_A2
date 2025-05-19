import numpy as np
import argparse

def generate_kmeans_test_data(k=3, points_per_cluster=100, std_dev=1.0, dimensions=2, seed=None, output_file="./test_files/kmeans_test_cases.txt"):
    """
    Generates synthetic test cases for K-means clustering and saves to a TXT file with tab (`\t`) as a delimiter.

    Parameters:
    - k: Number of clusters
    - points_per_cluster: Number of points per cluster
    - std_dev: Standard deviation of cluster points
    - dimensions: Number of features (dimensions)
    - seed: Random seed for reproducibility
    - output_file: File to save test cases (TXT)
    """
    if seed is not None:
        np.random.seed(seed)

    data = []
    labels = []

    # Generate k clusters
    for cluster_id in range(k):
        center = np.random.uniform(-1000, 1000, dimensions)  # Random cluster center
        points = np.random.normal(loc=center, scale=std_dev, size=(points_per_cluster, dimensions))
        
        data.append(points)
        labels.extend([cluster_id] * points_per_cluster)

    # Combine into an array
    data = np.vstack(data)
    np.random.shuffle(data)

    print(data.shape)
    # Save test cases to TXT file
    with open(output_file, "w") as f:
        # f.write("# K-Means Test Cases\n")
        # f.write("# Format: Feature_1\tFeature_2\t... Feature_N\tCluster_Label\n")
        # f.write("# Format: Feature_1\tFeature_2\t... Feature_N\n")
        print(f"{k * points_per_cluster} {dimensions}", file=f)
        for i in range(len(data)):
            # line = "\t".join(map(str, data[i])) + f"\t{labels[i]}\n"

            line = " ".join([str(int(v)) for v in data[i]])
            print(line, file=f)

    print(f"✅ Test cases generated and saved to {output_file}")

k = 100
points_per_cluster = 1000
std_dev = 100.0
dimensions = 100
seed = 42
output_file = './input100D_100clusters.in'

generate_kmeans_test_data(k, points_per_cluster, std_dev, dimensions, seed, output_file)