//
// Created by Lorenzo on 11/06/25.
//

#include "PermutationSorting.h"

Ctxt PermutationSorting::sort(const Ctxt& in_exp, const Ctxt& in_rep) {
    Ctxt indexing = compute_indexing(in_exp, in_rep);

    if (tieoffset) {
        Ctxt offset = compute_tieoffset(in_exp, in_rep);
        indexing = controller.add(indexing, offset);
    }

    if (verbose) controller.print(indexing, n*n, "Indexes: ");

    //Indexes sono corretti, solo scalati per 1/n

    Ctxt ordered = compute_sorting(indexing, in_rep);

    return ordered;

}

Ctxt PermutationSorting::compute_indexing(const Ctxt &in_exp, const Ctxt &in_rep){
    Ctxt difference = controller.sub(in_exp, in_rep);

    Ctxt cmp = controller.sigmoid(difference, n, degree_sigmoid, -sigmoid_scaling);
    Ctxt indexes = controller.rotsum(cmp, n);

    return controller.sub(indexes, controller.encode(0.5 / n, 0, n*n));
}

Ctxt PermutationSorting::compute_tieoffset(const Ctxt &in_exp, const Ctxt &in_rep){
    int d_tie = 1007;

    if (delta >= 0.1) {
        d_tie = 247;
    } else if (delta >= 0.01) {
        d_tie = 495;
    } else if (delta >= 0.001) {
        d_tie = 1007;
    } else if (delta >= 0.0001) {
        d_tie = 4007;
    }


    Ctxt eq = controller.sinc(controller.sub(in_exp, in_rep), d_tie, 1.0 / delta);

    Ctxt eqclone = controller.add(eq, controller.encode(0, eq->GetLevel(), n*n));

    for (int i = 0; i < log2(n); i++) {
        int rotindex = pow(2, i);
        eqclone = controller.add(eqclone, controller.rot(eqclone, n * rotindex));
    }

    Ctxt sx = controller.mult(eqclone, 0.5 / n);

    vector<double> triangular_matrix;

    for (int rows = 0; rows < n; rows++) {
        for (int cols = n - rows; cols < n; cols++) {
            triangular_matrix.push_back(0);
        }
        for (int cols = 0; cols < n - rows; cols++) {
            triangular_matrix.push_back(1 / (double) n);
        }
    }

    Ptxt triang = controller.encode(triangular_matrix, eq->GetLevel(), n*n);

    Ctxt dx = controller.mult(eq, triang);

    for (int i = 0; i < log2(n); i++) {
        int rotindex = pow(2, i);
        dx = controller.add(dx, controller.rot(dx, n * rotindex));
    }

    Ctxt offset = controller.sub(sx, dx);
    offset = controller.add(offset, 0.5 / n);

    return offset;
}

Ctxt PermutationSorting::compute_sorting(const Ctxt &indexes, const Ctxt &in_rep) {
    vector<double> zeros;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            zeros.push_back(i / (double) n);
        }
    }

    Ctxt permutation_delta = controller.sub(indexes, controller.encode(zeros, 0, n*n));

    Ctxt permutation_matrix = controller.sinc(permutation_delta, degree_sinc, n);

    Ctxt sorted = controller.mult(in_rep, permutation_matrix);

    for (int i = 0; i < log2(n); i++) {
        int rotindex = pow(2, i);
        sorted = controller.add(sorted, controller.rot(sorted, rotindex));
    }

    return sorted;
}


