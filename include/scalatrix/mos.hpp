#ifndef SCALATRIX_MOS_HPP
#define SCALATRIX_MOS_HPP

#include "scalatrix/affine_transform.hpp"
#include "scalatrix/scale.hpp"

namespace scalatrix {

class MOS {
public:

    int a, b, n, a0, b0, n0, mode;
    int repetitions, depth;
    double equave; // log2(frequency ratio) of equave (interval of equivalence)
    double period; // log2(frequency ratio) of MOS period
    double generator; 
    std::vector<bool> path;
    AffineTransform impliedAffine;
    IntegerAffineTransform mosTransform;
    Vector2i v_gen;
    Scale base_scale;


    static MOS& fromParams(int a, int b, int m, double e, double g);
    //static MOS& fromImpliedAffine(const AffineTransform& A, int repetitions);
    static MOS& fromG(int depth, int m, double g, double e, int repetitions = 1);
    void adjustParams(int a, int b, int m, double e, double g);
    //void adjustParamsFromImpliedAffine(const AffineTransform& A);

    double angle() const;
    double angleStd() const;

    AffineTransform calcImpliedAffine() const;

    double gFromAngle(double angle);

    std::string accidentalString(Vector2i v) const;
    std::string nodeLabelDigit(Vector2i v) const;
    std::string nodeLabelLetter(Vector2i v) const;
    std::string nodeLabelLetterWithOctaveNumber(Vector2i v) const;
};

} // namespace scalatrix

#endif // SCALATRIX_MOS_HPP