#include "scalatrix/scale.hpp"
#include "scalatrix/lattice.hpp"
#include <iostream>
#include <cassert>

namespace scalatrix {

void Scale::initNodes(int N){
    nodes_.clear();
    nodes_.reserve(N);
    for (int i = 0; i < N; ++i) {
        nodes_.push_back(Node());
    }
}

Scale::Scale(double base_freq, int N, int root_node_idx) : base_freq_(base_freq), root_idx_(root_node_idx) {
    initNodes(N);
}  

/*static*/ 
Scale& Scale::fromAffine(const AffineTransform& A, const double base_freq, int N, int root_node_idx) {

    static Scale _self;
    _self.initNodes(N);
    _self.base_freq_ = base_freq;
    _self.root_idx_ = root_node_idx;

    _self.recalcWithAffine(A, N, root_node_idx);
    return _self;
}

void Scale::recalcWithAffine(const AffineTransform& A, int N, int root_node_idx) {
    
    AffineTransform M = AffineTransform(A);
    M.tx = 0;
    M.ty = 0;
    auto [r, s] = findClosestWithinStrip(M);

    Vector2d zr = M * r;
    Vector2d zs = M * s;

    //printf("r = (%d, %d) -> (%f, %f)\n", r.x, r.y, zr.x, zr.y);
    //printf("s = (%d, %d) -> (%f, %f)\n", s.x, s.y, zs.x, zs.y);

    int n_min = -root_node_idx, n_max = N - root_node_idx;

    // generate the nodes within the strip using the 3-gap theorem
    Node root;
    root.natural_coord = Vector2i(0, 0);
    root.tuning_coord = A * root.natural_coord;
    root.pitch = base_freq_;

    nodes_[root_node_idx] = root;

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
        nodes_[root_node_idx + n] = last;
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
        nodes_[root_node_idx + n] = last;
    }
}

void Scale::retuneWithAffine(const AffineTransform& A) {
    for (int n = 0; n < nodes_.size(); ++n) {
        Node& node = nodes_[n];
        node.tuning_coord = A * node.natural_coord;
        node.pitch = base_freq_ * std::exp2(node.tuning_coord.x);
        node.isTempered = false;
    }
}

bool Node::operator<(const Node& other) const noexcept {
    return tuning_coord.x < other.tuning_coord.x;
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
            std::cout << node.pitch << " Hz";

            if (node.isTempered) {
                std::cout << " (" << node.temperedPitch.label << ")";
            }
            std::cout << "\n";

        }
    }
}

void Scale::temperToPitchSet(PitchSet& pitchset){
    // find the closest pitch in pitchset to each node in base_scale
    for (auto& node : nodes_) {
        double node_pitch_log2fr = log2(node.pitch/base_freq_);
        double closest_pitch_log2fr = 0.0;
        double min_dist = 1e6;
        PitchSetPitch closest_pitch;
        for (auto& pitch : pitchset) {
            double dist = std::abs(pitch.log2fr - node_pitch_log2fr);
            if (dist < min_dist) {
                min_dist = dist;
                closest_pitch_log2fr = pitch.log2fr;
                closest_pitch = pitch;
            }
        }
        node.tuning_coord.x = closest_pitch_log2fr;
        node.pitch = base_freq_ * exp2(closest_pitch_log2fr);
        node.isTempered = true;
        node.temperedPitch = closest_pitch;
    }
};


std::vector<Node>& Scale::getNodes(){
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