/**
 * // NOTE: DO NOT MODIFY THIS FILE!!! Your code should be in the file KMEANS_cuda.cu
 * This file is a sequential reference of the kmeans algorithm.
 */

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "KMEANS.h"

/**
 * Function euclideanDistanceSquared: square of the Euclidean distance
 * we always compare the square of euclidean distance to avoid sqrt function calls 
 */
float euclideanDistanceSquared(float* point, float* center, int ndims)
{
    float dist = 0.0;
    for (int i = 0; i < ndims; i++) {
        dist += (point[i] - center[i]) * (point[i] - center[i]);
    }
    return dist;
}

extern void do_compute(const struct parameters* p, struct results* r)
{
    int it = 0;
    int changes = 0;
    float maxDist = FLT_MIN;

    int K = p->nclusters;
    int ndims = p->ndims;
    int npoints = p->npoints;
    float* data = p->data;
    int* classMap = r->classMap;
    float* centroids = r->centroids;

    // pointPerClass: number of points classified in each class
    // auxCentroids: mean of the points in each class
    // distCentroids: distance between the centroids in this and previous iteration
    int* pointsPerClass = (int*)malloc(K * sizeof(int));
    float* auxCentroids = (float*)malloc(K * ndims * sizeof(float));
    float* distCentroids = (float*)malloc(K * sizeof(float));
    if (pointsPerClass == NULL || auxCentroids == NULL || distCentroids == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        exit(-4);
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    do {
        it++;
        // 1. Calculate the distance from each point to the centroid
        // Assign each point to the nearest centroid.
        changes = 0;
        for (int i = 0; i < npoints; i++) {
            int class = 1;
            float minDist = FLT_MAX;
            for (int j = 0; j < K; j++) {
                float dist = euclideanDistanceSquared(&data[i * ndims], &centroids[j * ndims], ndims);
                if (dist < minDist) {
                    minDist = dist;
                    // Note that the centroid class id starts from 1
                    // centroid id = 0 indicates that the point is not assigned to a cluster yet
                    class = j + 1; 
                }
            }
            if (classMap[i] != class) {
                changes++;
            }
            classMap[i] = class;
        }

        // 2. Recalculates the centroids: calculates the mean within each cluster
        memset(pointsPerClass, 0, K * sizeof(int));
        for (int i = 0; i < K * ndims; i++)
            auxCentroids[i] = 0.0;

        for (int i = 0; i < npoints; i++) {
            int class = classMap[i];
            pointsPerClass[class - 1] = pointsPerClass[class - 1] + 1;
            for (int j = 0; j < ndims; j++) {
                auxCentroids[(class - 1) * ndims + j] += data[i * ndims + j];
            }
        }

        for (int i = 0; i < K; i++) {
            for (int j = 0; j < ndims; j++) {
                auxCentroids[i * ndims + j] /= pointsPerClass[i];
            }
        }

        // 3. check the centroid distance between 2 iterations.
        maxDist = FLT_MIN;
        for (int i = 0; i < K; i++) {
            distCentroids[i] = euclideanDistanceSquared(&centroids[i * ndims], &auxCentroids[i * ndims], ndims);
            if (distCentroids[i] > maxDist) {
                maxDist = distCentroids[i];
            }
        }
        maxDist = sqrt(maxDist);
        memcpy(centroids, auxCentroids, (K * ndims * sizeof(float)));
        
        r->outputMsg[it - 1] = (struct message) { it, changes, maxDist };
    } while ((changes > p->minChanges) && (it < p->maxIterations) && (maxDist > p->maxThreshold));

    clock_gettime(CLOCK_MONOTONIC, &end);
    double duration = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;

    // report the result (changes of the last iteration, number of iterations
    // executed, and maximum centroid distance of the last iter). 
    // make sure that the cluster id of each point (r->classMap) and the centroid
    // of each cluster (r->centroids) are updated, at least for the last iteration.
    // in this reference code they are updated in each iteration, but you can have
    // your own implementation
    r->changes = changes;
    r->iterations = it;
    r->maxDist = maxDist;

    r->time = duration; // report the time without memory allocation or one-off
                        // memory copy between host and device

    free(pointsPerClass);
    free(auxCentroids);
    free(distCentroids);
}
