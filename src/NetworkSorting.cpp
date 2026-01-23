//
// Created by Lorenzo on 16/06/25.
//

#include "NetworkSorting.h"

Ctxt NetworkSorting::sort(const Ctxt& in) {
    int iterations = (log2(n) * (log2(n) + 1)) / 2;

    Ctxt clone_in = in->Clone();

    int current_iteration = 1;

    /*
     * These loops represent the actual bitonic sorting network
     */
    for (int i = 0; i < log2(n); i++) {
        for (int j = 0; j < i + 1; j++) {
            int arrowsdelta = pow(2, i - j);
            int stage = i - j;
            int round = j;

            auto start_time_local = steady_clock::now();

            clone_in = swap(clone_in, arrowsdelta, stage, round);

            if (verbose) print_duration(start_time_local, "Swap");
            start_time_local = steady_clock::now();

            if (current_iteration < iterations) clone_in = controller.bootstrap(clone_in);

            if (verbose) print_duration(start_time_local, "Bootstrapping");

            if (verbose) controller.print(clone_in, n);

            if (verbose) cout << "Layer " << current_iteration << " / " << iterations << " done." << endl;
            current_iteration++;

        }
    }

    return clone_in;
}


Ctxt NetworkSorting::swap(const Ctxt &in, int arrowsdelta, int round, int stage) {
    Ctxt rot_pos = controller.rot(in, arrowsdelta);
    Ctxt rot_neg = controller.rot(in, -arrowsdelta);

    // This performs the evaluation of the min function
    Ctxt m1 = controller.sub(in, controller.relu(controller.sub(in, rot_pos), relu_degree, n));

    // The other values are obtained in function of m1
    Ctxt m3 = controller.sub(controller.add(in, rot_pos), m1);
    Ctxt m4 = controller.rot(m1, -arrowsdelta);
    Ctxt m2 = controller.sub(controller.add(in, rot_neg), m4);

    vector<Ptxt> masks = generate_layer_masks(m1->GetLevel(), m1->GetSlots(), round, stage);

    return controller.add_tree({controller.mult(m1, masks[0]),
                                controller.mult(m2, masks[1]),
                                controller.mult(m3, masks[2]),
                                controller.mult(m4, masks[3])});

}


vector<Ptxt> NetworkSorting::generate_layer_masks(int encoding_level, int num_slots, int round, int stage, double mask_value) {
    vector<double> mask_1, mask_2, mask_3, mask_4;

    for (int i = 0; i < num_slots / (pow(2, round + 2)); i++) {

        for (int times = 0; times < pow(2, stage); times++) {
            for (int j = 0; j < pow(2, round); j++) {
                mask_1.push_back(mask_value);
                mask_2.push_back(0);
                mask_3.push_back(0);
                mask_4.push_back(0);
            }

            for (int j = 0; j < pow(2, round); j++) {
                mask_1.push_back(0);
                mask_2.push_back(mask_value);
                mask_3.push_back(0);
                mask_4.push_back(0);
            }
        }

        if ((i + 1) * pow(2, stage + round + 1) >= num_slots) {
            break;
        }

        for (int times = 0; times < pow(2, stage); times++) {
            for (int j = 0; j < pow(2, round); j++) {
                mask_1.push_back(0);
                mask_2.push_back(0);
                mask_3.push_back(mask_value);
                mask_4.push_back(0);
            }

            for (int j = 0; j < pow(2, round); j++) {
                mask_1.push_back(0);
                mask_2.push_back(0);
                mask_3.push_back(0);
                mask_4.push_back(mask_value);
            }
        }

        if ((i + 1) * pow(2, stage + round + 2) >= num_slots) {
            break;
        }

    }

    return {controller.encode(mask_1, encoding_level, num_slots),
            controller.encode(mask_2, encoding_level, num_slots),
            controller.encode(mask_3, encoding_level, num_slots),
            controller.encode(mask_4, encoding_level, num_slots)};
}