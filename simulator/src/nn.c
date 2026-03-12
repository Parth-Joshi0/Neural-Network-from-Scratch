#include "nn.h"
#include <math.h>
#include <stdio.h>

int nn_load(Network* nn, const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return -1;

    fread(nn->w1, sizeof(nn->w1), 1, f);
    fread(nn->b1, sizeof(nn->b1), 1, f);
    fread(nn->w2, sizeof(nn->w2), 1, f);
    fread(nn->b2, sizeof(nn->b2), 1, f);
    fread(nn->w3, sizeof(nn->w3), 1, f);
    fread(nn->b3, sizeof(nn->b3), 1, f);

    fclose(f);
    return 0;
}

void nn_forward(Network* nn, float* input, float* output) {
    float h1[NN_H1], h2[NN_H2];

    for (int i = 0; i < NN_H1; i++) {
        float z = nn->b1[i];
        for (int j = 0; j < NN_INPUT; j++)
            z += nn->w1[i][j] * input[j];
        h1[i] = tanhf(z);
    }

    for (int i = 0; i < NN_H2; i++) {
        float z = nn->b2[i];
        for (int j = 0; j < NN_H1; j++)
            z += nn->w2[i][j] * h1[j];
        h2[i] = tanhf(z);
    }

    for (int i = 0; i < NN_OUTPUT; i++) {
        float z = nn->b3[i];
        for (int j = 0; j < NN_H2; j++)
            z += nn->w3[i][j] * h2[j];
        output[i] = tanhf(z);
    }
}
