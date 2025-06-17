#ifndef SORTING_FHECONTROLLER_H
#define SORTING_FHECONTROLLER_H

#include "openfhe.h"
#include "ciphertext-ser.h"
#include "scheme/ckksrns/ckksrns-ser.h"
#include "ciphertext-ser.h"
#include "cryptocontext-ser.h"
#include "key/key-ser.h"

using namespace lbcrypto;
using namespace std;
using namespace std::chrono;

// Define aliases for clarity
using Ptxt = Plaintext;
using Ctxt = Ciphertext<DCRTPoly>;

class FHEController {
    CryptoContext<DCRTPoly> context; // Crypto context for the FHE system

public:
    FHEController() {}

    /**
     * Generate the cryptocontext for the evaluation of the bitonic sorting network
     *
     * @param num_slots The number of slots in the ciphertext.
     * @param levels_required The required circuit depth
     * @param toy_parameters Choose whether to use toy parameters (true) or 128-bit security parameters (false)
     * @return the total depth of the circuit, including the bootstrapping operation
     */
    int generate_context_network(int num_slots, int levels_required, bool toy_parameters);

    /**
     * Generate the rotation keys required by the network-based sorting
     *
     * @param num_slots The number of slots in the ciphertext.
     */
    void generate_rotation_keys_network(int num_slots);

    /**
     * Generate the cryptocontext for the evaluation of the permutation sorting network
     *
     * @param num_slots The number of slots in the ciphertext.
     * @param levels_required The required circuit depth
     * @param toy_parameters Choose whether to use toy parameters (true) or 128-bit security parameters (false)
     */
    void generate_context_permutation(int num_slots, int levels_required, bool toy_parameters);

    /**
     * Generate a rotation key
     *
     * @param index The index of the rotation
     */
    void generate_rotation_key(int index);

    /**
      * Basic FHE operations
      */

    // Encode a vector of doubles into a plaintext
    Ptxt encode(const vector<double>& vec, int level, int num_slots);

    // Encodes a value in a plaintext (it will be repeated)
    Ptxt encode(double value, int level, int num_slots);

    // Encrypt a vector of doubles
    Ctxt encrypt(const vector<double>& vec, int level = 0, int plaintext_num_slots = 0);

    // Encrypt a vector in expanded encoding
    Ctxt encrypt_expanded(const vector<double>& vec, int level = 0, int plaintext_num_slots = 0, int repetitions = 1);

    // Encrypt a vector in repeated encoding
    Ctxt encrypt_repeated(const vector<double>& vec, int level = 0, int plaintext_num_slots = 0, int repetitions = 1);

    //Decodes a ciphertexxt
    vector<double> decode(const Ptxt& p);

    // Decrypt a ciphertext
    Ptxt decrypt(const Ctxt& c);


    // Add two ciphertexts/plaintexts
    Ctxt add(const Ctxt& c1, const Ctxt& c2);
    Ctxt add(const Ctxt& c, const Ptxt& p);
    Ctxt add(const Ctxt& c, double d);

    // Add multiple ciphertexts using a tree structure
    Ctxt add_tree(vector<Ctxt> v);

    // Subtract two ciphertexts/plaintexts
    Ctxt sub(const Ctxt& c1, const Ctxt& c2);
    Ctxt sub(const Ctxt& c, const Ptxt& p);

    // Multiply two ciphertexts/plaintext
    Ctxt mult(const Ctxt& c, const Ptxt& p);
    Ctxt mult(const Ctxt& c, double d);
    Ctxt mult(const Ctxt& c1, const Ctxt& c2);

    // Rotate a ciphertext by a specified index
    Ctxt rot(const Ctxt& c, int index);

    // Perform bootstrapping operation on a ciphertext
    Ctxt bootstrap(const Ctxt& c);

    /**
      * Permutation-based operations
      */

    // Approximation of the k-scaled Sigmoid
    Ctxt sigmoid(const Ctxt& in, int n, int degree, int scaling);

    // Rotate-and-sum elements at distance n
    Ctxt rotsum(const Ctxt& in, int n);

    // Approximation of sinc function
    Ctxt sinc(const Ctxt& in, int degree, int n);


    /**
      * Network-based operations
      */

    // Approximation of the ReLU function
    Ctxt relu(const Ctxt& in, int degree, int n);


    /**
      * Utilities
      */

    // Print the values of slots in a ciphertext
    void print(const Ctxt& c, int slots = 0, string prefix = "");

private:
    KeyPair<DCRTPoly> key_pair; // Key pair for the FHE system

    void print_moduli_chain(const DCRTPoly& poly);
};

#endif //SORTING_FHECONTROLLER_H
