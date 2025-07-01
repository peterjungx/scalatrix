#ifndef SCALATRIX_NODE_HPP
#define SCALATRIX_NODE_HPP

#include "lattice.hpp"
#include "pitchset.hpp"
#include <string>

namespace scalatrix {

/**
 * Node represents a single musical note in a scale with both lattice coordinates and pitch information.
 * Simple struct with public fields for direct access.
 */
struct Node {
    Vector2i natural_coord;    // Integer coords in scale's natural system
    Vector2d tuning_coord;     // Floating-point coords in tuning system
    double pitch;              // Frequency in Hz
    bool isTempered;           // Whether pitch was adjusted to match a target
    PitchSetPitch temperedPitch; // Target pitch information
    PitchSetPitch closestPitch;  // Closest pitch in a pitch set

    Node() noexcept 
        : natural_coord(), tuning_coord(), pitch(0.0), isTempered(false), temperedPitch(), closestPitch() {}
    
    Node(const Vector2i& natural, const Vector2d& tuning, double freq) noexcept
        : natural_coord(natural), tuning_coord(tuning), pitch(freq), isTempered(false), temperedPitch(), closestPitch() {}

    // Comparison operator for sorting
    bool operator<(const Node& other) const noexcept {
        return tuning_coord.x < other.tuning_coord.x;
    }
};

} // namespace scalatrix

#endif // SCALATRIX_NODE_HPP