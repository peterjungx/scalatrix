#ifndef SCALATRIX_LABEL_CALCULATOR_HPP
#define SCALATRIX_LABEL_CALCULATOR_HPP

#include <string>
#include "scalatrix/mos.hpp"

namespace scalatrix {

class LabelCalculator {
public:
    static std::string nodeLabelDigit(const MOS& mos, Vector2i v) {
        return mos.nodeLabelDigit(v);
    }

    static std::string nodeLabelLetter(const MOS& mos, Vector2i v) {
        return mos.nodeLabelLetter(v);
    }

    static std::string nodeLabelLetterWithOctaveNumber(const MOS& mos, Vector2i v, int middle_C_octave = 4) {
        return mos.nodeLabelLetterWithOctaveNumber(v, middle_C_octave);
    }

    std::string noteLabelNormalized(MOS& mos, Vector2i v) {
        if (mos.generator > 4.0/7 && mos.generator < 3.0/5 && mos.equave > 0.9 && mos.equave < 1.2)
        {
            Vector2i diatonic_coord = diatonic_mos.mapFromMOS (mos, v);
            return diatonic_mos.nodeLabelLetter(diatonic_coord);
        }
        return mos.nodeLabelDigit(v);
    }

    LabelCalculator() {
        diatonic_mos = MOS::fromParams (5, 2, 1, 1.0, .585);
    }
private:
    MOS diatonic_mos;
};

}


#endif