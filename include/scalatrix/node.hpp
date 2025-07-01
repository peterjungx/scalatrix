#ifndef SCALATRIX_NODE_HPP
#define SCALATRIX_NODE_HPP

#include "lattice.hpp"
#include "pitchset.hpp"
#include <string>
#include <cstdio>
#include <cmath>
#include <utility>

namespace scalatrix {

/**
 * Node represents a single musical note in a scale with both lattice coordinates and pitch information.
 * 
 * A Node contains:
 * - natural_coord: Integer coordinates in the scale's natural lattice system
 * - tuning_coord: Floating-point coordinates in the tuning system (used for pitch calculation)
 * - pitch: The actual frequency value in Hz
 * - isTempered: Whether this node has been adjusted to match a target pitch set
 * - temperedPitch: Information about the target pitch this node was tempered to
 */
class Node {
private:
    Vector2i natural_coord_;    // Integer coords in scale's natural system
    Vector2d tuning_coord_;     // Floating-point coords in tuning system
    double pitch_;              // Frequency in Hz
    bool isTempered_;           // Whether pitch was adjusted to match a target
    PitchSetPitch temperedPitch_; // Target pitch information

public:
    // Constructors
    Node() noexcept 
        : natural_coord_(), tuning_coord_(), pitch_(0.0), isTempered_(false), temperedPitch_(),
          natural_coord(natural_coord_), tuning_coord(tuning_coord_), pitch(pitch_), 
          isTempered(isTempered_), temperedPitch(temperedPitch_) {}
    
    Node(const Vector2i& natural, const Vector2d& tuning, double freq) noexcept
        : natural_coord_(natural), tuning_coord_(tuning), pitch_(freq), isTempered_(false), temperedPitch_(),
          natural_coord(natural_coord_), tuning_coord(tuning_coord_), pitch(pitch_), 
          isTempered(isTempered_), temperedPitch(temperedPitch_) {}

    // Copy constructor and assignment operator
    Node(const Node& other) 
        : natural_coord_(other.natural_coord_), tuning_coord_(other.tuning_coord_), 
          pitch_(other.pitch_), isTempered_(other.isTempered_), temperedPitch_(other.temperedPitch_),
          natural_coord(natural_coord_), tuning_coord(tuning_coord_), pitch(pitch_), 
          isTempered(isTempered_), temperedPitch(temperedPitch_) {}
    
    Node& operator=(const Node& other) {
        if (this != &other) {
            natural_coord_ = other.natural_coord_;
            tuning_coord_ = other.tuning_coord_;
            pitch_ = other.pitch_;
            isTempered_ = other.isTempered_;
            temperedPitch_ = other.temperedPitch_;
        }
        return *this;
    }

    // Move constructor and assignment operator
    Node(Node&& other) noexcept 
        : natural_coord_(std::move(other.natural_coord_)), tuning_coord_(std::move(other.tuning_coord_)), 
          pitch_(other.pitch_), isTempered_(other.isTempered_), temperedPitch_(std::move(other.temperedPitch_)),
          natural_coord(natural_coord_), tuning_coord(tuning_coord_), pitch(pitch_), 
          isTempered(isTempered_), temperedPitch(temperedPitch_) {}
    
    Node& operator=(Node&& other) noexcept {
        if (this != &other) {
            natural_coord_ = std::move(other.natural_coord_);
            tuning_coord_ = std::move(other.tuning_coord_);
            pitch_ = other.pitch_;
            isTempered_ = other.isTempered_;
            temperedPitch_ = std::move(other.temperedPitch_);
        }
        return *this;
    }

    // Destructor
    ~Node() = default;

    // Getters
    const Vector2i& getNaturalCoord() const { return natural_coord_; }
    const Vector2d& getTuningCoord() const { return tuning_coord_; }
    double getPitch() const { return pitch_; }
    bool getIsTempered() const { return isTempered_; }
    const PitchSetPitch& getTemperedPitch() const { return temperedPitch_; }

    // Setters
    void setNaturalCoord(const Vector2i& coord) { natural_coord_ = coord; }
    void setTuningCoord(const Vector2d& coord) { tuning_coord_ = coord; }
    void setPitch(double freq) { pitch_ = freq; }
    void setIsTempered(bool tempered) { isTempered_ = tempered; }
    void setTemperedPitch(const PitchSetPitch& pitch) { 
        temperedPitch_ = pitch; 
        isTempered_ = true;
    }

    // Clear tempering information
    void clearTempered() {
        isTempered_ = false;
        temperedPitch_ = PitchSetPitch{};
    }

    // Comparison operator for sorting
    bool operator<(const Node& other) const noexcept {
        return tuning_coord_.x < other.tuning_coord_.x;
    }

    /**
     * Generate a label showing the tempered pitch with optional deviation in cents.
     * 
     * @param thresholdCents If deviation is less than this, show plain label (default 0.1)
     * @param forceExactLabel If true, always show plain label without deviation
     * @return String with format "label" or "label+/-XX.Xct" depending on deviation
     */
    std::string deviationLabel(double thresholdCents = 0.1, bool forceExactLabel = false) const {
        // If no temperedPitch is set, return empty string
        if (temperedPitch_.label.empty()) {
            return "";
        }
        
        // Calculate the actual pitch of this node
        double actualPitchLog2fr = tuning_coord_.x;
        
        // Calculate deviation in cents
        double deviationCents = 1200.0 * (actualPitchLog2fr - temperedPitch_.log2fr);
        
        // If deviation is small enough or exact label is forced, use plain label
        if (std::abs(deviationCents) < thresholdCents || forceExactLabel) {
            return temperedPitch_.label;
        }
        
        // Otherwise, append deviation
        char deviationStr[32];
        if (deviationCents > 0) {
            std::snprintf(deviationStr, sizeof(deviationStr), "%s+%.1fct", 
                         temperedPitch_.label.c_str(), deviationCents);
        } else {
            std::snprintf(deviationStr, sizeof(deviationStr), "%s%.1fct", 
                         temperedPitch_.label.c_str(), deviationCents);
        }
        
        return std::string(deviationStr);
    }

    // Legacy field access for backward compatibility - these are references to the private members
    Vector2i& natural_coord;
    Vector2d& tuning_coord;
    double& pitch;
    bool& isTempered;
    PitchSetPitch& temperedPitch;
};

} // namespace scalatrix

#endif // SCALATRIX_NODE_HPP