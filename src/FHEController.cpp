//
// Created by Lorenzo on 14/04/24.
//

#include "FHEController.h"

int FHEController::generate_context_network(int num_slots, int levels_required, bool toy_parameters) {
    CCParams<CryptoContextCKKSRNS> parameters;

    parameters.SetSecretKeyDist(SPARSE_TERNARY);
    vector<uint32_t> level_budget;

    level_budget = {2, 2};
    int dcrtBits = 54;
    int firstMod = 55;

    if (toy_parameters) {
        parameters.SetSecurityLevel(lbcrypto::HEStd_NotSet);
        parameters.SetRingDim(1 << 12);
    } else {
        parameters.SetSecurityLevel(lbcrypto::HEStd_128_classic);
        parameters.SetRingDim(1 << 16);
    }

    cout << "Levels required: " << levels_required << endl;

    parameters.SetNumLargeDigits(5);
    parameters.SetBatchSize(num_slots);

    ScalingTechnique rescaleTech = FLEXIBLEAUTO;

    parameters.SetScalingModSize(dcrtBits);
    parameters.SetScalingTechnique(rescaleTech);
    parameters.SetFirstModSize(firstMod);

    int levelsUsedBeforeBootstrap = levels_required + 1;

    int circuit_depth = levelsUsedBeforeBootstrap + FHECKKSRNS::GetBootstrapDepth(level_budget, SPARSE_TERNARY);

    parameters.SetMultiplicativeDepth(circuit_depth);

    context = GenCryptoContext(parameters);
    context->Enable(PKE);
    context->Enable(KEYSWITCH);
    context->Enable(LEVELEDSHE);
    context->Enable(ADVANCEDSHE);
    context->Enable(FHE);

    key_pair = context->KeyGen();

    print_moduli_chain(key_pair.publicKey->GetPublicElements()[0]);

    cout << endl;

    context->EvalMultKeyGen(key_pair.secretKey);

    context->EvalBootstrapSetup(level_budget, {0, 0}, num_slots);
    context->EvalBootstrapKeyGen(key_pair.secretKey, num_slots);


    return circuit_depth;
}


void FHEController::generate_context_permutation(int num_slots, int levels_required, bool toy) {
    CCParams<CryptoContextCKKSRNS> parameters;

    parameters.SetSecretKeyDist(lbcrypto::UNIFORM_TERNARY);

    int dcrtBits = 45;
    int firstMod = 48;

    if (toy) {
        parameters.SetSecurityLevel(lbcrypto::HEStd_NotSet);

        if (num_slots <= 1 << 14) parameters.SetRingDim(1 << 15);
        if (num_slots <= 1 << 13) parameters.SetRingDim(1 << 14);
        if (num_slots <= 1 << 12) parameters.SetRingDim(1 << 13);
        if (num_slots <= 1 << 11) parameters.SetRingDim(1 << 12);

        cout << "n: " << num_slots << endl;
    } else {
        parameters.SetSecurityLevel(lbcrypto::HEStd_128_classic);
        parameters.SetRingDim(1 << 16);
    }

    cout << "N: " << parameters.GetRingDim() << ", ";

    parameters.SetBatchSize(num_slots);

    ScalingTechnique rescaleTech = FLEXIBLEAUTO;

    parameters.SetScalingModSize(dcrtBits);
    parameters.SetScalingTechnique(rescaleTech);
    parameters.SetFirstModSize(firstMod);

    //This keeps memory small, at the cost of increasing the modulus
    parameters.SetNumLargeDigits(2);

    parameters.SetMultiplicativeDepth(levels_required);


    context = GenCryptoContext(parameters);
    context->Enable(PKE);
    context->Enable(KEYSWITCH);
    context->Enable(LEVELEDSHE);
    context->Enable(ADVANCEDSHE);
    //context->Enable(FHE);

    key_pair = context->KeyGen();

    print_moduli_chain(key_pair.publicKey->GetPublicElements()[0]);

    cout << ", λ: 128 bits" << endl;

    context->EvalMultKeyGen(key_pair.secretKey);

}

void FHEController::generate_rotation_keys_network(int num_slots) {
    vector<int> rotations;

    for (int i = 0; i < log2(num_slots); i++) {
        rotations.push_back(pow(2, i));
        rotations.push_back(-pow(2, i));
    }

    context->EvalRotateKeyGen(key_pair.secretKey, rotations);
}

void FHEController::generate_rotation_key(int index) {
    vector<int> rotations;

    rotations.push_back(index);

    context->EvalRotateKeyGen(key_pair.secretKey, rotations);
}

Ptxt FHEController::encode(const vector<double> &vec, int level, int num_slots) {
    Ptxt p = context->MakeCKKSPackedPlaintext(vec, 1, level, nullptr, num_slots);
    p->SetLength(num_slots);

    return p;
}

Ptxt FHEController::encode(double value, int level, int num_slots) {
    vector<double> repeated_value;
    for (int i = 0; i < num_slots; i++) repeated_value.push_back(value);

    return encode(repeated_value, level, num_slots);
}

Ctxt FHEController::encrypt(const vector<double> &vec, int level, int num_slots) {
    Ptxt p = encode(vec, level, num_slots);

    return context->Encrypt(p, key_pair.publicKey);
}

Ctxt FHEController::encrypt_expanded(const vector<double> &vec, int level, int num_slots, int repetitions) {
    vector<double> repeated;

    for (std::size_t i = 0; i < vec.size(); i++) {
        for (int j = 0; j < repetitions; j++) {
            repeated.push_back(vec[i]);
        }
    }

    Ptxt p = encode(repeated, level, num_slots);

    return context->Encrypt(p, key_pair.publicKey);
}

Ctxt FHEController::encrypt_repeated(const vector<double> &vec, int level, int num_slots, int repetitions) {
    vector<double> repeated;

    for (int i = 0; i < repetitions; i++) {
        for (std::size_t j = 0; j < vec.size(); j++) {
            repeated.push_back(vec[j]);
        }
    }

    Ptxt p = encode(repeated, level, num_slots);

    return context->Encrypt(p, key_pair.publicKey);
}

vector<double> FHEController::decode(const Ptxt& p) {
    return p->GetRealPackedValue();
}

Ptxt FHEController::decrypt(const Ctxt &c) {
    Ptxt p;
    context->Decrypt(key_pair.secretKey, c, &p);

    return p;
}

Ctxt FHEController::add(const Ctxt &a, const Ctxt &b) {
    return context->EvalAdd(a, b);
}

Ctxt FHEController::add(const Ctxt &a, const Ptxt &b) {
    Ptxt temp(b);
    return context->EvalAdd(a, temp);
}

Ctxt FHEController::add(const Ctxt &a, double d) {
    Ptxt temp(encode(d, a->GetLevel(), a->GetSlots()));
    return context->EvalAdd(a, temp);
}

Ctxt FHEController::add_tree(vector<Ctxt> v) {
    return context->EvalAddMany(v);
}

Ctxt FHEController::sub(const Ctxt &a, const Ctxt &b) {
    return context->EvalSub(a, b);
}

Ctxt FHEController::sub(const Ctxt &c, const Ptxt &p) {
    Ptxt temp(b);
    return context->EvalSub(c, temp);
}

Ctxt FHEController::mult(const Ctxt &c, const Ptxt& p) {
    return context->EvalMult(c, p);
}

Ctxt FHEController::mult(const Ctxt &c1, const Ctxt &c2) {
    return context->EvalMult(c1, c2);
}

Ctxt FHEController::mult(const Ctxt &c, double v) {
    return context->EvalMult(c, encode(v, c->GetLevel(), c->GetSlots()));
}

Ctxt FHEController::rot(const Ctxt& c, int index) {
    return context->EvalRotate(c, index);
}

Ctxt FHEController::bootstrap(const Ctxt &c) {
    return context->EvalBootstrap(c);
}


Ctxt FHEController::rotsum(const Ctxt &in, int n) {
    Ctxt result = add(in, rot(in, n));

    for (int i = 1; i < log2(n); i++) {
        result = add(result, rot(result, n * pow(2, i)));
    }

    return result;
}

Ctxt FHEController::sigmoid(const Ctxt &in, int n, int degree, int scaling) {
    return context->EvalChebyshevFunction([scaling, n](double x) -> double {
        return 1/(n + n * pow(2.71828182846, -scaling*x));

    }, in, -1, 1, degree);
}


Ctxt FHEController::sinc(const Ctxt &in, int poly_degree, int n) {
    return context->EvalChebyshevFunction([n](double x) -> double { return sin(3.14159265358979323846 * x * n) / (3.14159265358979323846 * x * n); },
                                          in,
                                          -1,
                                          1, poly_degree);
}

Ctxt FHEController::relu(const Ctxt &in, int poly_degree, int n) {
    return context->EvalChebyshevFunction([](double x) -> double { if (x > 0) return x; return 0; },
                                          in,
                                          -1,
                                          1, poly_degree);
}

void FHEController::print(const Ctxt &c, int slots, string prefix) {
    if (slots == 0) {
        slots = c->GetSlots();
    }

    cout << prefix;

    Ptxt result;
    context->Decrypt(key_pair.secretKey, c, &result);
    result->SetSlots(slots);
    vector<double> v = result->GetRealPackedValue();

    cout << "[ ";

    for (int i = 0; i < slots; i += 1) {
        string segno = "";
        if (v[i] > 0) {
            segno = "";
        } else {
            segno = "-";
            v[i] = -v[i];
        }


        if (i == slots - 1) {
            cout << segno << v[i] << " ]";
        } else {
            if (abs(v[i]) <= 0.00001)
                cout << "0.0000" << " ";
            else
                cout << segno << v[i] << " ";
        }
    }

    cout << endl;
}

void FHEController::print_moduli_chain(const DCRTPoly& poly){
    int num_primes = poly.GetNumOfElements();
    double total_bit_len = 0.0;
    for (int i = 0; i < num_primes; i++) {
        auto qi = poly.GetParams()->GetParams()[i]->GetModulus();
        total_bit_len += log(qi.ConvertToDouble()) / log(2);
    }
    std::cout << "log(QP): " << ((int)total_bit_len);
}