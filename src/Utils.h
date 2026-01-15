//
// Created by Lorenzo on 14/04/24.
//

#ifndef SORTING_UTILS_H
#define SORTING_UTILS_H

#include <iostream>
#include <algorithm> // for shuffle
#include <numeric>   // for iota

#include <vector>
#include <random>
#include <iomanip>

//#define GREEN_TEXT "\033[1;32m"
//#define RED_TEXT "\033[1;31m"
//#define YELLOW_TEXT "\033[1;33m"
//#define RESET_COLOR "\033[0m"

#define GREEN_TEXT ""
#define RED_TEXT ""
#define YELLOW_TEXT ""
#define RESET_COLOR ""

using namespace std;

enum SortingType {
    NONE, PERMUTATION, NETWORK
};

static inline  string to_string(SortingType type) {
    switch (type) {
        case NONE: return "NONE";
        case PERMUTATION: return "Permutation-based";
        case NETWORK: return "Network-based";
        default: return "UNKNOWN";
    }
}

static inline  vector<double> generate_random_vector(int num_values) {
    //Generates a vector of num_values elements uniformely sampled from (0, 1)

    vector<double> vec(num_values);

    //Integers
    iota(vec.begin(), vec.end(), 0);

    //in (0, 1)
    //iota(vec.begin(), vec.end(), 0.5);
    //for (double& value : vec) {
        //value /= num_values;
    //}

    random_device rd;
    mt19937 g(rd());
    shuffle(vec.begin(), vec.end(), g);

    return vec;
}


static inline int poly_evaluation_cost(int degree) {
    //Cost for running the Paterson-Stockmeyer algorithm
    //https://github.com/openfheorg/openfhe-development/blob/main/src/pke/examples/FUNCTION_EVALUATION.md

    if (degree <= 5+1) {
        return 3;
    } else if (degree <= 13+1) {
        return 4;
    } else if (degree <= 27+1) {
        return 5;
    } else if (degree <= 59+1) {
        return 6;
    } else if (degree <= 119+1) {
        return 7;
    } else if (degree <= 247+1) {
        return 8;
    } else if (degree <= 495+1) {
        return 9;
    } else if (degree <= 1007+1) {
        return 10;
    } else if (degree <= 2031+1) {
        return 11;
    } else {
        cerr << "Use a valid degree!" << endl;
        return 0;
    }

}

static inline vector<double> parse_input_vector(const std::string& input) {
    std::vector<double> result;
    std::istringstream iss(input);
    char discard;
    double value;

    // Discard the opening bracket
    iss >> discard;

    // Parse the doubles
    while (iss >> value) {
        result.push_back(value);

        // Discard the comma or closing bracket
        iss >> std::ws >> discard;
    }

    if (floor(log2(result.size())) != ceil(log2(result.size()))) {
        cerr << "The number of values must be a power of two!" << endl;
        return {};
    }

    return result;
}

static inline double infinity_norm(const std::vector<double>& vec1, const std::vector<double>& vec2) {
    double max_diff = 0.0;
    for (std::size_t i = 0; i < vec1.size(); i++) {
        double diff = abs(vec1[i] - vec2[i]);
        if (diff > max_diff) {
            max_diff = diff;
        }
    }

    return max_diff;
}

static inline double precision_bits(const std::vector<double>& vec1, const std::vector<double>& vec2) {
    return -log2(infinity_norm(vec1, vec2));
}

static inline void print_duration(chrono::time_point<steady_clock, nanoseconds> start, const string &title) {
    auto ms = duration_cast<milliseconds>(steady_clock::now() - start);

    auto secs = duration_cast<seconds>(ms);
    ms -= duration_cast<milliseconds>(secs);
    auto mins = duration_cast<minutes>(secs);
    secs -= duration_cast<seconds>(mins);

    if (mins.count() < 1) {
        cout << "⌛(" << title << "): " << secs.count() << ":" << ms.count() << "s" << endl;
    } else {
        cout << "⌛(" << title << "): " << mins.count() << "." << secs.count() << ":" << ms.count() << endl;
    }
}

static inline vector<string> tokenizer(string s, char del)
{
    stringstream ss(s);
    string word;

    vector<string> tokens;

    while (!ss.eof()) {
        getline(ss, word, del);
        tokens.push_back(word);
    }

    return tokens;
}

static inline bool isSameUpToThreeDecimals(double a, double b) {
    return std::round(a * 1000) == std::round(b * 1000);
}

static inline bool isSameUpToNDecimals(double a, double b, int n) {
    return std::round(a * n) == std::round(b * n);
}

static inline std::vector<double> generate_close_randoms(int n, double max_distance = 0.01) {
    if (n * max_distance > 1) {
        cout << "INFO: the random input vector will contain duplicates." << endl;
    }

    std::vector<double> values;
    //int steps = static_cast<int>(1.0 / max_distance);

    for (double x = 0.0; x < 1; x+= max_distance) {
        values.push_back(x);
    }

    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator


    while (values.size() < static_cast<std::size_t>(n)) {
        std::uniform_int_distribution<> distr(0, values.size()); // define the range
        int random = distr(gen);
        values.push_back(values[random]);
    }

    //Ok, not *truly random* duplicates but still...

    std::shuffle(values.begin(), values.end(), gen);

    return {values.begin(), values.begin() + n};
}


//Copy pasted from https://github.com/openfheorg/openfhe-development/blob/main/src/core/lib/math/chebyshev.cpp
/*
static inline std::vector<double> EvalChebyshevCoefficients(std::function<double(double)> func, double a, double b, uint32_t degree) {
    if (!degree) {
        OPENFHE_THROW("The degree of approximation can not be zero");
    }
    // the number of coefficients to be generated should be degree+1 as zero is also included
    size_t coeffTotal{degree + 1};
    double bMinusA = 0.5 * (b - a);
    double bPlusA  = 0.5 * (b + a);
    double PiByDeg = M_PI / static_cast<double>(coeffTotal);
    std::vector<double> functionPoints(coeffTotal);
    for (size_t i = 0; i < coeffTotal; ++i)
        functionPoints[i] = func(std::cos(PiByDeg * (i + 0.5)) * bMinusA + bPlusA);

    double multFactor = 2.0 / static_cast<double>(coeffTotal);
    std::vector<double> coefficients(coeffTotal);
    for (size_t i = 0; i < coeffTotal; ++i) {
        for (size_t j = 0; j < coeffTotal; ++j)
            coefficients[i] += functionPoints[j] * std::cos(PiByDeg * i * (j + 0.5));
        coefficients[i] *= multFactor;
    }
    return coefficients;
}
*/


#endif //SORTING_UTILS_H
