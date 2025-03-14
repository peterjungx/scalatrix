#include "scalatrix/scale.hpp"
#include "scalatrix/lattice.hpp"
#include <iostream>
#include <cassert>

namespace scalatrix {

bool Node::operator<(const Node& other) const noexcept {
    // Your implementation (e.g., compare tuning_coord.y)
    return false;
}

Scale::Scale(double base_freq, int N) : base_freq_(base_freq) {
    initNodes(N);
}

void Scale::initNodes(int N){
    nodes_.clear();
    nodes_.reserve(N);
    for (int i = 0; i < N; ++i) {
        nodes_.push_back(Node());
    }
}

/*static*/ 
Scale& Scale::fromAffine(const AffineTransform& A, const double base_freq, int N, int n_root) {

    static Scale _self;
    _self.initNodes(N);
    _self.base_freq_ = base_freq;

    _self.recalcWithAffine(A, N, n_root);
    return _self;
}

void Scale::recalcWithAffine(const AffineTransform& A, int N, int n_root) {
    
    AffineTransform M = AffineTransform(A);
    M.tx = 0;
    M.ty = 0;
    auto [r, s] = findClosestWithinStrip(M);

    Vector2d zr = M * r;
    Vector2d zs = M * s;

    //printf("r = (%d, %d) -> (%f, %f)\n", r.x, r.y, zr.x, zr.y);
    //printf("s = (%d, %d) -> (%f, %f)\n", s.x, s.y, zs.x, zs.y);

    int n_min = -n_root, n_max = N - n_root;

    // generate the nodes within the strip using the 3-gap theorem
    Node root;
    root.natural_coord = Vector2i(0, 0);
    root.tuning_coord = A * root.natural_coord;
    root.pitch = base_freq_;

    nodes_[n_root] = root;

    Node last = root;

    // forward pass
    for (int n = 1; n < n_max; ++n) {
        if (0 <= last.tuning_coord.y + zr.y && last.tuning_coord.y + zr.y < 1) {
            last.natural_coord += r;
        } else if (0 <= last.tuning_coord.y + zs.y && last.tuning_coord.y + zs.y < 1) {
            last.natural_coord += s;
        } else {
            last.natural_coord += (r + s);
        }
        last.tuning_coord = A * last.natural_coord;
        last.pitch = base_freq_ * std::exp2(last.tuning_coord.x);
        nodes_[n_root + n] = last;
    }
    //std::cout << "Length of right: " << right.size() << "\n";

    // backward pass
    last = root;
    for (int n = -1; n >= n_min; --n) {
        if (0 <= last.tuning_coord.y - zr.y && last.tuning_coord.y - zr.y < 1) {
            last.natural_coord -= r;
        } else if (0 <= last.tuning_coord.y - zs.y && last.tuning_coord.y - zs.y < 1) {
            last.natural_coord -= s;
        } else {
            last.natural_coord -= (r + s);
        }
        last.tuning_coord = A * last.natural_coord;
        last.pitch = base_freq_ * std::exp2(last.tuning_coord.x);
        nodes_[n_root + n] = last;
    }
}

void Scale::retuneWithAffine(const AffineTransform& A) {
    for (int n = 0; n < nodes_.size(); ++n) {
        Node& node = nodes_[n];
        node.tuning_coord = A * node.natural_coord;
        node.pitch = base_freq_ * std::exp2(node.tuning_coord.x);
    }
}

void Scale::print(int first, int num) const {
    for (int i = first; i < first + num; ++i) {
        if (i < 0 || i >= nodes_.size()) {
            std::cout << "Node " << i << " out of range\n";
        } else {
            const Node& node = nodes_[i];
            
            // print natural coordinates, tuning coordinates, and pitch in single line
            std::cout.precision(3);
            std::cout << "Node " << i << ": (" << node.natural_coord.x << ", " << node.natural_coord.y << ") -> ("
                      << node.tuning_coord.x << ", " << node.tuning_coord.y << ") ";

            std::cout.precision(5);     
            std::cout << node.pitch << " Hz\n";

        }
    }
}

const std::vector<Node>& Scale::getNodes() const {
    return nodes_;
}


// destructor
Scale::~Scale() {

    // free the memory
    // https://stackoverflow.com/questions/13944886/is-stdvector-memory-freed-upon-a-clear
    nodes_.clear();
    //nodes_.shrink_to_fit();
    std::vector<Node>(nodes_).swap(nodes_);
    
}

} // namespace scalatrix