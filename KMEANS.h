#pragma once

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <cmath>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

struct message {
    int it;
    int changes;
    float maxDist;
};

// Hint: you might need to change some of these struct-s
struct parameters {
    int minChanges;     // termination condition: minimum changes per iter
    int maxIterations;  // termination condition: maximum number of iterations
    float maxThreshold; // termination condition: minimum centroid update precision per iter
    float *data;        // the coordinate of points. data[i * ndims + j] represents the j-th dimension (counting from 0) of point i
    int npoints;        // number of points in the euclidean space
    int nclusters;      // number of clusters
    int ndims;          // number of dimensions of each point
};

struct results {
    struct message *outputMsg; // compute information of each iteration (for logging)
    int *classMap;             // the result cluster id for each point. classMap[i] represents that point i is in the classMap[i]-th cluster
    float *centroids;          // the centroids of each result cluster. centroids[i * ndims + j] represents the j-th dimension (counting from 0) of cluster i
    int changes;               // termination condition: change of the last iter
    int iterations;            // termination condition: number of iterations executed
    float maxDist;             // termination condition: centroid update precision of the last iter
    double time;               // the computation time
};

float euclideanDistanceSquared(float *point, float *center, int ndims);

void do_compute(const struct parameters *p, struct results *r);
