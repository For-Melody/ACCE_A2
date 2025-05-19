// top level function or kernel function implementation

#include "KMEANS.h"

/**
 * Function euclideanDistanceSquared: square of the Euclidean distance
 * we always compare the square of euclidean distance to avoid sqrt function calls
 */
float euclideanDistanceSquared(float *point, float *center, int ndims) {
    float dist = 0.0;

    for (int i = 0; i < ndims; i++) {
        dist += (point[i] - center[i]) * (point[i] - center[i]);
    }

    return dist;
}

/**
 * do_compute: the entry of your top level function or kernel funcion that implements the K-Means algorithm
 * This is an almost complete copy of the sequential function except for the timing
 */
void do_compute(const struct parameters *p, struct results *r) {

    int it = 0;
    int changes = 0;
    float maxDist = FLT_MIN;
    double duration = -1; // only record duration in C simulation

    int K = p->nclusters;
    int ndims = p->ndims;
    int npoints = p->npoints;
    float *data = p->data;
    int *classMap = r->classMap;
    float *centroids = r->centroids;

    // pointPerClass: number of points classified in each class
    // auxCentroids: mean of the points in each class
    // distCentroids: distance between the centroids in this and previous iteration
    int *pointsPerClass = (int *)malloc(K * sizeof(int));
    float *auxCentroids = (float *)malloc(K * ndims * sizeof(float));
    float *distCentroids = (float *)malloc(K * sizeof(float));
    if (pointsPerClass == NULL || auxCentroids == NULL || distCentroids == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        exit(-4);
    }

#ifndef __SYNTHESIS__
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
#endif

    do {
        it++;
        // 1. Calculate the distance from each point to the centroid
        // Assign each point to the nearest centroid.
        changes = 0;

        for (int i = 0; i < npoints; i++) {
            int Class = 1;
            float minDist = FLT_MAX;
            for (int j = 0; j < K; j++) {
                float dist = euclideanDistanceSquared(&data[i * ndims], &centroids[j * ndims], ndims);
                if (dist < minDist) {
                    minDist = dist;
                    // Note that the centroid class id starts from 1
                    // centroid id = 0 indicates that the point is not assigned to a cluster yet
                    Class = j + 1;
                }
            }
            if (classMap[i] != Class) {
                changes++;
            }
            classMap[i] = Class;
        }

        // 2. Recalculates the centroids: calculates the mean within each cluster
        memset(pointsPerClass, 0, K * sizeof(int));
        for (int i = 0; i < K * ndims; i++)
            auxCentroids[i] = 0.0;

        for (int i = 0; i < npoints; i++) {
            int Class = classMap[i];
            pointsPerClass[Class - 1] = pointsPerClass[Class - 1] + 1;
            for (int j = 0; j < ndims; j++) {
                auxCentroids[(Class - 1) * ndims + j] += data[i * ndims + j];
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

        r->outputMsg[it - 1] = (struct message){it, changes, maxDist};
    } while ((changes > p->minChanges) && (it < p->maxIterations) && (maxDist > p->maxThreshold));

#ifndef __SYNTHESIS__
    clock_gettime(CLOCK_MONOTONIC, &end);
    duration = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
#endif

    // report the result (changes of the last iteration, number of iterations
    // executed, and maximum centroid distance of the last iter).
    // make sure that the cluster id of each point (r->classMap) and the centroid
    // of each cluster (r->centroids) are updated, at least for the last iteration.
    // in this reference code they are updated in each iteration, but you can have
    // your own implementation
    r->changes = changes;
    r->iterations = it;
    r->maxDist = maxDist;
    r->time = duration; // report the time only for C Simulation

    free(pointsPerClass);
    free(auxCentroids);
    free(distCentroids);
}
