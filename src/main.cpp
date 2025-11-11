#include <iostream>
#include "openfhe.h"
#include "chrono"
#include "../src/FHEController.h"
#include "Utils.h"
#include "PermutationSorting.h"
#include "NetworkSorting.h"




using namespace lbcrypto;
using namespace std;
using namespace std::chrono;

void read_arguments(int argc, char *argv[]);
void set_permutation_parameters(int n, double d);
void set_network_parameters(double d);
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
 * Network-based parameters
 */
int relu_degree;
double input_scale;


/*
 * Experimental
 */
bool clean_permutation_matrix;


SortingType sortingType = NONE;

int main(int argc, char *argv[]) {
    read_arguments(argc, argv);

    if (argc == 1 || (argc == 2 && string(argv[1]) == "--help"))
        return 0;

    if (sortingType == NONE) {
        cerr << "You must pick a sorting method. Add either --permutation or --network" << endl;
        return 1;
    } else {
        cout << "Selected sorting type: " << to_string(sortingType) << endl;
    }

    Ctxt result;

    auto start_time = steady_clock::now();

    if (sortingType == PERMUTATION) {
        if (sigmoid_scaling == 0 || degree_sigmoid == 0 || degree_sinc == 0) {
            set_permutation_parameters(n, delta);
        }

        cout << setprecision(precision_digits) << fixed;

        cout << endl << "Ciphertext: " << endl << input_values << endl << endl << "δ: " << delta << ", ";

        controller.generate_context_permutation(n * n, circuit_depth + 13, toy);

        for (int i = 0; i < log2(n); i++) {
            controller.generate_rotation_key(pow(2, i) * n);
            controller.generate_rotation_key(pow(2, i));
        }

        Ctxt in_exp = controller.encrypt_expanded(input_values, 0, n*n, n);
        Ctxt in_rep = controller.encrypt_repeated(input_values, 0, n*n, n);

        PermutationSorting sorting =
                PermutationSorting(controller, sigmoid_scaling, degree_sigmoid, degree_sinc, tieoffset, n, delta, toy, verbose, clean_permutation_matrix);

        result = sorting.sort(in_exp, in_rep);

    } else if (sortingType == NETWORK) {
        set_network_parameters(delta);

        cout << setprecision(precision_digits) << fixed;

        cout << endl << "Ciphertext: " << endl << input_values << endl << endl << "δ: " << delta << ", n: " << n << endl;

        // Levels required by max(0, x) approximation
        int levels_consumption = poly_evaluation_cost(relu_degree);

        // One more level for the masking operation
        levels_consumption += 1;

        circuit_depth = controller.generate_context_network(n, levels_consumption, toy);
        controller.generate_rotation_keys_network(n);

        for (std::size_t i = 0; i < input_values.size(); i++) {
            input_values[i] *= input_scale;
        }

        Ctxt in = controller.encrypt(input_values, circuit_depth - levels_consumption - 3, n);

        NetworkSorting sorting =
                NetworkSorting(controller, n, relu_degree, verbose);

        result = sorting.sort(in);
    }

    print_duration(start_time, "The sorting took:");

    evaluate_sorting_accuracy(result);
}


void evaluate_sorting_accuracy(const Ctxt& result) {
    cout << "Level final: " << result->GetLevel() << "/" << circuit_depth << endl << endl;

    vector<double> sorted_fhe = controller.decode(controller.decrypt(result));

    vector<double> results_fhe;

    if (sortingType == PERMUTATION) {
        for (int i = 0; i < n * n; i += n) {
            results_fhe.push_back(sorted_fhe[i] / input_scale);
        }
    } else if (sortingType == NETWORK){
        for (int i = 0; i < n; i += 1) {
            results_fhe.push_back(sorted_fhe[i]);
        }
    }

    sort(input_values.begin(), input_values.end());

    cout << endl << "Expected:  " << input_values << endl;
    cout << endl << "Obtained:  " << results_fhe << endl << endl;

    int corrects = 0;

    for (int i = 0; i < n; i++) {
        if (abs(input_values[i] - results_fhe[i]) < delta) corrects++;
    }
    cout << "Corrects (up to " << delta << "): " << GREEN_TEXT << corrects << RESET_COLOR "/" << GREEN_TEXT << n <<RESET_COLOR<< endl;

    cout << "Precision bits: " << GREEN_TEXT << precision_bits(input_values, results_fhe) << RESET_COLOR << endl;
}

void set_permutation_parameters(int n, double d) {
    int partial_depth = 0;

    if (d >= 0.1) {
        precision_digits = 1;
        sigmoid_scaling = 650;
        degree_sigmoid = 1006;
        partial_depth = 10;
    } else if (d >= 0.01) {
        precision_digits = 2;
        sigmoid_scaling = 650;
        degree_sigmoid = 1006;
        partial_depth = 10;
    } else if (d >= 0.001) {
        precision_digits = 3;
        sigmoid_scaling = 9170;
        degree_sigmoid = 16000;
        partial_depth = 14;
    } else if (d >= 0.0001) {
        //Still todo
        precision_digits = 4;
        sigmoid_scaling = 16000;
        degree_sigmoid = 32000;
        partial_depth = 15;

        cout << endl << "k: " << sigmoid_scaling << ", d: " << degree_sigmoid << endl << endl;
        degree_sinc = 495;
    } else {
        cerr << "The required min distance '" << d << "' is too small!" << endl;
    }

    if (n <= 8) {
        degree_sinc = 59;
        partial_depth += 6;
    } else if (n == 16) {
        degree_sinc = 59;
        partial_depth += 6;
    } else if (n == 32) {
        degree_sinc = 119;
        partial_depth += 7;
    } else if (n == 64) {
        degree_sinc = 247;
        partial_depth += 8;
    } else if (n == 128) {
        degree_sinc = 495;
        partial_depth += 9;
    }

    input_scale = 1.0;

    circuit_depth = partial_depth + 1; //For the last matrix mult

    cout << "Circuit depth: " << circuit_depth << endl;

}

void set_network_parameters(double d) {
    if (d >= 0.1) {
        precision_digits = 1;
        relu_degree = 119;
    } else if (d >= 0.01) {
        precision_digits = 2;
        relu_degree = 495;
    } else if (d >= 0.001) {
        precision_digits = 3;
        relu_degree = 495;
    } else if (d >= 0.0001) {
        //Still todo
        precision_digits = 4;
        relu_degree = 495;
    } else {
        cerr << "The required min distance '" << d << "' is too small!" << endl;
    }

    input_scale = 0.95;
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

            cout << "n: " << n << endl << "δ: " << delta << endl << endl;

        } catch (...) {
            cerr << "A problem occured in parsing the input vector" << endl;
        }
    }

    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "--permutation") {
            sortingType = PERMUTATION;
        }
        if (string(argv[i]) == "--network") {
            sortingType = NETWORK;
        }
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
        if (string(argv[i]) == "--relu") {
            relu_degree = stoi(argv[i+1]);
        }
        if (string(argv[i]) == "--clean_permutation_matrix") {
            clean_permutation_matrix = true;
        }

    }

    if (random_elements) {
        input_values = generate_close_randoms(n, delta);
    }
}


