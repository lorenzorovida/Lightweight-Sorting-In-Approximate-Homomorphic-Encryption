#include <iostream>
#include "openfhe.h"
#include "chrono"
#include "../src/FHEController.h"
#include "Utils.h"
#include "PermutationSorting.h"

#include "schemelet/rlwe-mp.h"
#include "math/hermite.h"
#include <functional>


using namespace lbcrypto;
using namespace std;
using namespace std::chrono;

void read_arguments(int argc, char *argv[]);
void set_permutation_parameters(int n, double d);
void evaluate_sorting_accuracy(const Ctxt& result);


FHEController controller;
vector<double> input_values;

int n;
double delta;
int precision_digits;
bool toy;
bool verbose;

/*
 * Permutation-based parameters
 */
int sigmoid_scaling;
int degree_sigmoid;
int degree_sinc;
int circuit_depth;
bool tieoffset;


/*
 * Experimental
 */
bool clean_permutation_matrix;


int main(int argc, char *argv[]) {
    //read_arguments(argc, argv);

    //if (argc == 1 || (argc == 2 && string(argv[1]) == "--help"))
    //    return 0;

    toy = true;
    delta = 0.001;
    n = 128;
    tieoffset = true;
    clean_permutation_matrix = true;
    verbose = true;

    auto start_time = steady_clock::now();

    if (sigmoid_scaling == 0 || degree_sigmoid == 0 || degree_sinc == 0) {
        set_permutation_parameters(n, delta);
    }

    controller.generate_context_permutation(n * n, circuit_depth + 10, toy, n, delta);
    for (int i = 0; i < log2(n); i++) {
        controller.generate_rotation_key(pow(2, i) * n);
        controller.generate_rotation_key(pow(2, i));
    }

    /*
     * Comincia
     */
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    std::vector<double> recommendedValues;
    std::vector<int> classes(0);
    for (int i = 0; i < 128 * 2; i++) {
        recommendedValues.push_back(dist(rng));
        classes.push_back(i);
    }

    cout << endl << endl << "Given features vector: " recommendedValues << endl << endl;

    vector<Ctxt> sortedClasses;

    for (int i = 0; i < 128 * 2 / 128; i++) {
        std::vector<double> slice(recommendedValues.begin() + (i * 128), recommendedValues.begin() + ((i + 1) * 128));

        input_values = slice;

        Ctxt in_exp = controller.encrypt_expanded(slice, 0, n * n, n);
        Ctxt in_rep = controller.encrypt_repeated(slice, 0, n * n, n);

        PermutationSorting sorting =
                PermutationSorting(controller, sigmoid_scaling, degree_sigmoid, degree_sinc, tieoffset, n, delta, toy,
                                   verbose, clean_permutation_matrix);

        pair<Ctxt, Ctxt> result = sorting.sort(in_exp, in_rep);

        //evaluate_sorting_accuracy(result.first);

        std::vector<double> sliceClasses(classes.begin() + (i * 128), classes.begin() + ((i + 1) * 128));
        Ctxt sliceClassesCtxt = controller.encrypt_repeated(sliceClasses, 0, n * n, n);

        sliceClassesCtxt = controller.mult(sliceClassesCtxt, result.second);

        for (int j = 0; j < log2(n); j++) {
            int rotindex = pow(2, j);
            sliceClassesCtxt = controller.add(sliceClassesCtxt, controller.rot(sliceClassesCtxt, rotindex));
        }

        controller.print_expanded(sliceClassesCtxt, 128*128, 128, "Classes (part " + to_string(i) + "): ");

        print_duration(start_time, "The sorting took:");
    }




}


void evaluate_sorting_accuracy(const Ctxt& result) {
    cout << endl << "Final level: " << result->GetLevel() << "/" << circuit_depth << endl;

    vector<double> sorted_fhe = controller.decode(controller.decrypt(result));

    vector<double> results_fhe;
    for (int i = 0; i < n * n; i += n) {
        results_fhe.push_back(sorted_fhe[i] / 1.0);
    }

    sort(input_values.begin(), input_values.end());

    if (verbose) cout << endl << "Expected:  " << input_values << endl;
    if (verbose) cout << endl << "Obtained:  " << results_fhe << endl << endl;

    int corrects = 0;

    for (int i = 0; i < n; i++) {
        if (abs(input_values[i] - results_fhe[i]) < delta) corrects++;
    }
    cout << "Corrects (up to " << delta << "): " << GREEN_TEXT << corrects << RESET_COLOR "/" << GREEN_TEXT << n <<RESET_COLOR<< endl;

    cout << "Precision bits: " << GREEN_TEXT << precision_bits(input_values, results_fhe) << RESET_COLOR << endl;
}

void set_permutation_parameters(int n, double d) {
    int partial_depth = 0;

    if (d == 0.1) {
        precision_digits = 1;
        sigmoid_scaling = 650;
        degree_sigmoid = 1006;
        partial_depth = 10;

    } else if (d == 0.01) {
        precision_digits = 2;
        sigmoid_scaling = 360;
        degree_sigmoid = 495;
        partial_depth = 9;
        partial_depth += 4; // Metto due clean

    } else if (d == 0.001) {
        precision_digits = 3;
        sigmoid_scaling = 2400;
        degree_sigmoid = 2031;
        partial_depth = 11;
        partial_depth += 6; // Metto tre clean

    } else if (d == 0.0001) {
        precision_digits = 4;
        sigmoid_scaling = 3500;
        degree_sigmoid = 4030;
        partial_depth = 12;
        partial_depth += 14; // Metto sette clean


    } else {
        cerr << "The required min distance '" << d << "' is too small!" << endl;
    }

    if (tieoffset) partial_depth += 2; //Tieoffset derivative

    cout << setprecision(precision_digits) << fixed;

    if (n <= 8) {
        degree_sinc = 59;
        partial_depth += 6;


        if (delta == 0.0001) {
            degree_sinc = 247;
            partial_depth += 2;
        }
    } else if (n == 16) {
        degree_sinc = 119;
        partial_depth += 7;

        if (delta == 0.0001) {
            degree_sinc = 495;
            partial_depth += 3;
        }
    } else if (n == 32) {
        degree_sinc = 247;
        partial_depth += 8;

        partial_depth += 2; //One clean

        if (delta == 0.0001) {
            degree_sinc = 495;
            partial_depth += 2;
        }
    } else if (n == 64) {
        degree_sinc = 495;
        partial_depth += 9;

        partial_depth += 2; //One clean

    } else if (n == 128) {
        degree_sinc = 495;
        partial_depth += 9;

        partial_depth += 4; //Two clean

        if (delta == 0.0001) {
            partial_depth += 2; //One clean
        }
    }

    circuit_depth = partial_depth + 1; //For the last matrix mult

    if (verbose) cout << "Circuit depth: " << circuit_depth << endl;

}

void read_arguments(int argc, char *argv[]) {
    if (argc == 1) {
        cerr << "Usage: ./Sort [input] [sorting mode] [options]\n"
                "\n"
                "Required Input (choose ONE):\n"
                "  --random <num_values>     Generate <num_values> random values (must be a power of two)\n"
                "  --file <filename>         Read numeric values from the specified file\n"
                "  --inline \"[a,b,c,...]\"  Provide an inline vector of numeric values\n"
                "\n"
                "Required Sorting Mode (choose ONE):\n"
                "  --network                 Use network-based sorting\n"
                "  --permutation             Use permutation-based sorting\n"
                "\n"
                "Optional Flags:\n"
                "  --toy                     Enable toy mode\n"
                "  --verbose                 Enable detailed output\n"
                "  --tieoffset               Apply tie-offset adjustment\n"
                "  --delta <value>           Manually set the delta (value spacing)\n"
                "  --relu <degree>           Set ReLU degree (integer parameter)\n"
                "\n"
                "Examples:\n"
                "  ./program --random 8 --network\n"
                "  ./program --file input.txt --permutation\n"
                "  ./program \"[1.2, 3.4, 2.1, 4.0]\" --network\n"
                "  ./program --random 16 --permutation --verbose --delta 0.05\n"
                "\n"
                "Notes:\n"
                "  - Exactly one input method and one sorting mode must be specified.\n"
                "  - For --random, the number of values must be a power of two.\n"
                "  - If reading from file, the file must contain space-, comma-, or newline-separated numbers." << endl;
        return;
    }

    bool random_elements = false;

    if (argc > 2 && string(argv[1]) == "--random") {
        int num_values = stoi(string(argv[2]));

        random_elements = true;

        if (floor(log2(num_values)) != ceil(log2(num_values))) {
            cerr << "The number of values must be a power of two" << endl;
        }

        n = num_values;

    } else if (argc > 2 && string(argv[1]) == "--file") {
        ifstream f(argv[2]); //taking file as inputstream
        string str;
        if (f) {
            ostringstream ss;
            ss << f.rdbuf(); // reading data
            str = ss.str();
        } else {
            cout << "Could not find \"" << string(argv[2]) << "\"" << endl;
        }
        input_values = parse_input_vector("[ " + str + " ]");

        n = input_values.size();

        vector<double> input_values_clone(input_values);
        sort(input_values_clone.begin(), input_values_clone.end());

        double min_diff = 1.0;

        for (std::size_t i = 1; i < input_values_clone.size(); i++) {
            double diff = input_values_clone[i] - input_values_clone[i - 1];
            if (diff < min_diff) {
                min_diff = diff;
            }
        }

        delta = min_diff;

    }
    else if (argc > 2 && string(argv[1]) == "--inline" && string(argv[2]).front() == '[' && string(argv[2]).back() == ']') {
        try {
            input_values = parse_input_vector(argv[2]);
            n = input_values.size();

            double min_diff = 1.0;

            for (std::size_t i = 1; i < input_values.size(); i++) {
                double diff = input_values[i] - input_values[i - 1];
                if (abs(diff) < min_diff) {
                    min_diff = abs(diff);
                }
            }

            delta = min_diff;

            if (verbose) cout << "n: " << n << endl << "δ: " << delta << endl << endl;

        } catch (...) {
            cerr << "A problem occured in parsing the input vector" << endl;
        }
    }

    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "--toy") {
            toy = true;
        }
        if (string(argv[i]) == "--verbose") {
            verbose = true;
        }
        if (string(argv[i]) == "--tieoffset") {
            tieoffset = true;
        }
        if (string(argv[i]) == "--delta") {
            delta = stod(argv[i+1]);
        }
        if (string(argv[i]) == "--clean_permutation_matrix") {
            clean_permutation_matrix = true;
        }

    }

    if (random_elements) {
        input_values = generate_close_randoms(n, delta);
    }
}

