#include "scalatrix/label_calculator.hpp"

namespace scalatrix {

std::string LabelCalculator::accidentalString(const MOS& mos, Vector2i v, bool swap) {
    int acc = floor( ((swap?-1:1)* (v.y * mos.a0 - v.x * mos.b0) - 2.0) / mos.n0 + 1);
    if (mos.L_vec.x == 1){
        acc *= -1;
    }
    std::string result = "";
    if (acc != 0) {
        while (acc < 0) {
            acc += 1;
            // Use direct UTF-8 byte sequences for better Windows compatibility
            result += "\xe2\x99\xad"; // UTF-8 bytes for ♭ (U+266D)
        }
        while (acc > 0) {
            acc -= 1;
            // Use direct UTF-8 byte sequences for better Windows compatibility
            result += "\xe2\x99\xaf"; // UTF-8 bytes for ♯ (U+266F)
        }
    }
    return result;
}

std::string LabelCalculator::nodeLabelDigit(const MOS& mos, Vector2i v) {
    int dia = (v.x + v.y + 128*mos.n) % mos.n;
    std::string result = std::to_string(dia+1);
    result = accidentalString(mos, v) + result;
    return result;
}

std::string LabelCalculator::nodeLabelLetter(const MOS& mos, Vector2i v) {
    int dia = (v.x + v.y + 2 + 128*mos.n) % mos.n;
    char letter = 'A' + dia;
    std::string result(1, letter);
    result = accidentalString(mos, v) + result;
    return result;
}

std::string LabelCalculator::nodeLabelLetterWithOctaveNumber(const MOS& mos, Vector2i v, int middle_C_octave) {
    std::string result = nodeLabelLetter(mos, v);
    int octave = middle_C_octave + floor((.0 + v.x + v.y) / mos.n);
    result += std::to_string(octave);
    return result;
}

std::string LabelCalculator::deviationLabel(const Node& node, double thresholdCents, 
                                            bool compareWithTempered) {
    // Select which pitch to use as reference
    const PitchSetPitch& referencePitch = node.closestPitch;
    
    // If no reference pitch is set, return empty string
    if (referencePitch.label.empty()) {
        return "";
    }
    
    // Calculate the actual pitch of this node
    double actualPitchLog2fr = compareWithTempered ? node.temperedPitch.log2fr : node.tuning_coord.x;
    
    // Calculate deviation in cents
    double deviationCents = 1200.0 * (actualPitchLog2fr - referencePitch.log2fr);
    
    // If deviation is small enough, use plain label
    if (std::abs(deviationCents) < thresholdCents) {
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

} // namespace scalatrix