#ifndef SCALATRIX_NODE_HPP
#define SCALATRIX_NODE_HPP

#include "lattice.hpp"
#include "pitchset.hpp"
#include <string>
#include <cstdio>
#include <cmath>

namespace scalatrix {

/**
 * Enum to specify which pitch to use for deviation calculation
 */
enum class DeviationReference {
    TEMPERED,  // Calculate deviation from temperedPitch
    CLOSEST    // Calculate deviation from closestPitch
};

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

    /**
     * Generate a label showing the pitch with optional deviation in cents.
     * 
     * @param thresholdCents If deviation is less than this, show plain label (default 0.1)
     * @param forceExactLabel If true, always show plain label without deviation
     * @param reference Which pitch to use as reference: TEMPERED (default) or CLOSEST
     * @return String with format "label" or "label+/-XX.Xct" depending on deviation
     */
    std::string deviationLabel(double thresholdCents = 0.1, bool forceExactLabel = false, DeviationReference reference = DeviationReference::TEMPERED) const {
        // Select which pitch to use as reference
        const PitchSetPitch& referencePitch = (reference == DeviationReference::TEMPERED) ? temperedPitch : closestPitch;
        
        // If no reference pitch is set, return empty string
        if (referencePitch.label.empty()) {
            return "";
        }
        
        // Calculate the actual pitch of this node
        double actualPitchLog2fr = tuning_coord.x;
        
        // Calculate deviation in cents
        double deviationCents = 1200.0 * (actualPitchLog2fr - referencePitch.log2fr);
        
        // If deviation is small enough or exact label is forced, use plain label
        if (std::abs(deviationCents) < thresholdCents || forceExactLabel) {
            return referencePitch.label;
        }
        
        // Otherwise, append deviation
        char deviationStr[32];
        if (deviationCents > 0) {
            std::snprintf(deviationStr, sizeof(deviationStr), "%s+%.1fct", 
                         referencePitch.label.c_str(), deviationCents);
        } else {
            std::snprintf(deviationStr, sizeof(deviationStr), "%s%.1fct", 
                         referencePitch.label.c_str(), deviationCents);
        }
        
        return std::string(deviationStr);
    }
};

} // namespace scalatrix

#endif // SCALATRIX_NODE_HPP