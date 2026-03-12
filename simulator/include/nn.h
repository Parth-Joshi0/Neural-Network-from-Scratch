#ifndef NN_H
#define NN_H

#define NN_INPUT  12
#define NN_H1     24
#define NN_H2     16
#define NN_OUTPUT  2

typedef struct {
    float w1[NN_H1][NN_INPUT];  float b1[NN_H1];
    float w2[NN_H2][NN_H1];     float b2[NN_H2];
    float w3[NN_OUTPUT][NN_H2]; float b3[NN_OUTPUT];
} Network;

int  nn_load(Network* nn, const char* path);
void nn_forward(Network* nn, float* input, float* output);

#endif