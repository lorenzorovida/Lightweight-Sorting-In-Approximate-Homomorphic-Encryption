//
// Created by Lorenzo Rovida on 11/06/25.
//


#ifndef PRACTICAL_SORTING_OF_ENCRYPTED_NUMBERS_PERMUTATIONSORTING_H
#define PRACTICAL_SORTING_OF_ENCRYPTED_NUMBERS_PERMUTATIONSORTING_H

#include "../src/FHEController.h"
// For multithreading
#include <cmath>
#include <omp.h>


using namespace lbcrypto;
using namespace std;
using namespace std::chrono;


class PermutationSorting {
    FHEController controller;
    int sigmoid_scaling;
    int degree_sigmoid;
    int degree_sinc;
    bool tieoffset;
    int n;
    double delta;
    bool toy;
    bool verbose;
    bool clean_permutation_matrix;

    public:
    PermutationSorting(FHEController controller,
                       int sigmoid_scaling,
                       int degree_sigmoid,
                       int degree_sinc,
                       bool tieoffset,
                       int n,
                       double delta,
                       bool toy,
                       bool verbose,
                       bool clean_permutation_matrix)
            : controller(controller),
              sigmoid_scaling(sigmoid_scaling),
              degree_sigmoid(degree_sigmoid),
              degree_sinc(degree_sinc),
              tieoffset(tieoffset),
              n(n),
              delta(delta),
              toy(toy),
              verbose(verbose),
              clean_permutation_matrix(clean_permutation_matrix) {}

        Ctxt sort(const Ctxt& in_exp, const Ctxt& in_rep);

    private:
        Ctxt compute_indexing(const Ctxt &c);
        Ctxt compute_tieoffset(const Ctxt &c);
        Ctxt compute_sorting(const Ctxt &indexes, const Ctxt &in_rep);
        void set_degrees(double d);
};


#endif //PRACTICAL_SORTING_OF_ENCRYPTED_NUMBERS_PERMUTATIONSORTING_H
