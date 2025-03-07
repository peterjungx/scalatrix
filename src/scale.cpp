#include "scalatrix/scale.hpp"
#include "scalatrix/lattice.hpp"
#include <iostream>
#include <cassert>

namespace scalatrix {

bool Node::operator<(const Node& other) const noexcept {
    // Your implementation (e.g., compare tuning_coord.y)
    return false;
}

Scale::Scale(double base_freq) : base_freq_(base_freq) {}

/*static*/ 
Scale& Scale::fromAffine(const AffineTransform& A, const double base_freq, int N, int n_root) {

    static Scale _self;
    _self.base_freq_ = base_freq;
    
    AffineTransform M = AffineTransform(A);
    M.tx = 0;
    M.ty = 0;
    auto [r, s] = findClosestWithinStrip(M);

    Vector2d zr = M * r;
    Vector2d zs = M * s;

    //printf("r = (%d, %d) -> (%f, %f)\n", r.x, r.y, zr.x, zr.y);
    //printf("s = (%d, %d) -> (%f, %f)\n", s.x, s.y, zs.x, zs.y);

    std::vector<Node> right;
    std::vector<Node> left;

    int n_min = -n_root, n_max = N - n_root;

    // generate the nodes within the strip using the 3-gap theorem
    Node root;
    root.natural_coord = Vector2i(0, 0);
    root.tuning_coord = A * root.natural_coord;
    root.pitch = base_freq;

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
        last.pitch = base_freq * std::exp2(last.tuning_coord.x);
        Node node = last;
        right.push_back(node);
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
        last.pitch = base_freq * std::exp2(last.tuning_coord.x);
        Node node = last;
        left.push_back(node);
    }
    // reverse left
    std::reverse(left.begin(), left.end());
    // Combine (nodes = left + root + right)
    _self.nodes_.clear();
    _self.nodes_ = left;
    _self.nodes_.push_back(root);
    _self.nodes_.insert(_self.nodes_.end(), right.begin(), right.end());
    //std::cout << "Length of nodes: " << nodes.size() << "\n";

    return _self;
}

void Scale::print(int first, int num) const {
    // Paste your implementation
}

const std::vector<Node>& Scale::getNodes() const {
    return nodes_;
}

} // namespace scalatrix