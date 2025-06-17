//
// Created by Lorenzo on 16/06/25.
//

#ifndef PRACTICAL_SORTING_OF_ENCRYPTED_NUMBERS_NETWORKSORTING_H
#define PRACTICAL_SORTING_OF_ENCRYPTED_NUMBERS_NETWORKSORTING_H

#include "../src/FHEController.h"
#include "Utils.h"

using namespace lbcrypto;
using namespace std;
using namespace std::chrono;


class NetworkSorting {
    FHEController controller;
    int n;
    double relu_degree;
    bool verbose;

public:
    NetworkSorting(FHEController controller,
                       int n,
                       double relu_degree,
                       bool verbose)
            : controller(controller),
              n(n),
              relu_degree(relu_degree),
              verbose(verbose) {}
    /**
     *
     * @param in The input ciphertext
     * @return The sorted ciphertext, according to a bitonic sorting network
     */
    Ctxt sort(const Ctxt& in);

private:
    /**
     * Evaluates a layer of a Sorting Network. In particular, it performs the swap
     * operation exploiting the SIMD parallelism in order to evaluate a whole layer.
     * See notebooks for a implementation over clear numbers
     *
     * @param in The input vector
     * @param arrowsdelta The arrowsdelta value, i.e., the distance between compared elements
     * @param round The current round
     * @param stage The current stage
     * @param poly_degree The degree of the ReLU Chebyshev polynomial
     * @return The vector obtained by applying the swapping opeartions
     */
    Ctxt swap(const Ctxt &in, int arrowsdelta, int round, int stage);

    /**
     * Generates a set of four masks to be applied to the four comparison vectors
     *
     * @param encoding_level The level at which the masks must be encoded
     * @param num_slots The number of values of the input vector
     * @param round The current round
     * @param stage The current stage
     * @return Four masks to be applied to the four comparison vectors for extracting the
     * required values
     */
    vector<Ptxt> generate_layer_masks(int encoding_level, int length, int round, int stage, double mask_value = 1.0);
};


#endif //PRACTICAL_SORTING_OF_ENCRYPTED_NUMBERS_NETWORKSORTING_H
