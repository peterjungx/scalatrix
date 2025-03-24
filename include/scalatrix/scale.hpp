#ifndef SCALATRIX_SCALE_HPP
#define SCALATRIX_SCALE_HPP

#include "lattice.hpp"
#include "affine_transform.hpp"
#include "pitchset.hpp"
#include <string>
#include <vector>

namespace scalatrix {

struct Node {
    Vector2i natural_coord;  // Integer coords in scale's natural system
    Vector2d tuning_coord;   // Floating-point coords in tuning system
    double pitch;
    bool isTempered;
    PitchSetPitch temperedPitch;
    Node() noexcept : natural_coord(), tuning_coord(), pitch(0.0), isTempered(false), temperedPitch() {}
    bool operator<(const Node& other) const noexcept;
    // assignment operator
    Node& operator=(const Node& other) {
        natural_coord = other.natural_coord;
        tuning_coord = other.tuning_coord;
        pitch = other.pitch;
        return *this;
    }
};

class Scale {
public:
    Scale(double base_freq = 261.63, int N = 128, int root_node_idx = 60);
    

    static Scale& fromAffine(const AffineTransform& M, const double base_freq, int N, int n_root);
    void print(int first = 58, int num = 5) const;
    std::vector<Node>& getNodes();
    void recalcWithAffine(const AffineTransform& A, int N, int n_root);
    void retuneWithAffine(const AffineTransform& A);
    int getRootIdx() const { return root_idx_; }
    void temperToPitchSet(PitchSet& pitchset);

    ~Scale();

private:
    void initNodes(int N);
    std::vector<Node> nodes_;
    double base_freq_;
    int root_idx_;
};

} // namespace scalatrix

#endif // SCALATRIX_SCALE_HPP
