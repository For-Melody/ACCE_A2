#include "KMEANS.h"
#include <hls_stream.h>
#include <hls_math.h>
#include <cstring>

#define MAX_POINTS 100000   
#define MAX_NDIMS       64   
#define MAX_K           32   


// Compute square of the Euclidean distance
float euclideanDistanceSquared(const float* point, const float* center, int ndims) {
#pragma HLS INLINE off
    float dist = 0.0f;
    for (int i = 0; i < ndims; i++) {
#pragma HLS PIPELINE II=1
        float diff = point[i] - center[i];
        dist += diff * diff;
    }
    return dist;
}

// Top-level kernel for K-Means
void do_compute(const parameters& p, results& r) {
    // AXI-Lite control interface for parameters and results
#pragma HLS INTERFACE s_axilite port=p         bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=r         bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=return    bundle=CTRL_BUS
    // AXI Master interfaces for bulk data
#pragma HLS INTERFACE m_axi     port=p.data    offset=slave bundle=DATA_BUS depth=MAX_POINTS*MAX_NDIMS
#pragma HLS INTERFACE m_axi     port=r.centroids offset=slave bundle=DATA_BUS depth=MAX_K*MAX_NDIMS
#pragma HLS INTERFACE m_axi     port=r.classMap offset=slave bundle=DATA_BUS depth=MAX_POINTS

    int it = 0;
    int changes = 0;
    float maxDist = 0.0f;

    // Read parameters
    int K = p.nclusters;
    int ndims = p.ndims;
    int npoints = p.npoints;
    const float* data = p.data;
    int* classMap = r.classMap;
    float* centroids = r.centroids;

    // Static buffers replace malloc/free
    static int   pointsPerClass[MAX_K];
    static float auxCentroids[MAX_K * MAX_NDIMS];
    static float distCentroids[MAX_K];

#ifndef __SYNTHESIS__
    // Timing for C simulation only
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
#endif

    do {
        it++;
        changes = 0;
        maxDist = 0.0f;

        // 1. Assign points to nearest centroid
        for (int i = 0; i < npoints; i++) {
#pragma HLS PIPELINE II=1
            int bestClass = 0;
            float minDist = FLT_MAX;
            for (int j = 0; j < K; j++) {
#pragma HLS UNROLL factor=2
                float d2 = euclideanDistanceSquared(&data[i * ndims], &centroids[j * ndims], ndims);
                if (d2 < minDist) {
                    minDist = d2;
                    bestClass = j;
                }
            }
            int assigned = bestClass + 1;
            if (classMap[i] != assigned) changes++;
            classMap[i] = assigned;
        }

        // 2. Reset and accumulate for new centroids
        for (int k = 0; k < K; k++) {
#pragma HLS PIPELINE
            pointsPerClass[k] = 0;
        }
        for (int i = 0; i < K * ndims; i++) {
#pragma HLS PIPELINE
            auxCentroids[i] = 0.0f;
        }
        for (int i = 0; i < npoints; i++) {
#pragma HLS PIPELINE II=1
            int cls = classMap[i] - 1;
            pointsPerClass[cls]++;
            for (int d = 0; d < ndims; d++) {
#pragma HLS UNROLL factor=2
                auxCentroids[cls * ndims + d] += data[i * ndims + d];
            }
        }
        for (int k = 0; k < K; k++) {
            for (int d = 0; d < ndims; d++) {
#pragma HLS PIPELINE
                auxCentroids[k * ndims + d] /= pointsPerClass[k];
            }
        }

        // 3. Compute centroid shift and update
        float localMax = 0.0f;
        for (int k = 0; k < K; k++) {
#pragma HLS PIPELINE II=1
            float d2 = euclideanDistanceSquared(&centroids[k * ndims], &auxCentroids[k * ndims], ndims);
            if (d2 > localMax) localMax = d2;
            distCentroids[k] = d2;
        }
        maxDist = hls::sqrtf(localMax);
        // update centroids array
        for (int i = 0; i < K * ndims; i++) {
#pragma HLS PIPELINE
            centroids[i] = auxCentroids[i];
        }

        // record iteration
        r.outputMsg[it - 1] = (message){ it, changes, maxDist };

    } while ((changes > p.minChanges) && (it < p.maxIterations) && (maxDist > p.maxThreshold));

#ifndef __SYNTHESIS__
    clock_gettime(CLOCK_MONOTONIC, &end);
    r.time = double(end.tv_sec - start.tv_sec) + double(end.tv_nsec - start.tv_nsec) / 1e9;
#else
    r.time = 0;
#endif

    // Report final results
    r.changes = changes;
    r.iterations = it;
    r.maxDist = maxDist;
}

