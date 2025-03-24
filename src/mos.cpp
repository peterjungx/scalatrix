#include "scalatrix/mos.hpp"
#include "scalatrix/params.hpp" 

#include <cmath>
#include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>


namespace scalatrix {


int gcd(int a, int b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

double MOS::angleStd() const {
    // inverse of
    // double generator = 1.0 / (1.0 + tan((1 - angle) * M_PI * .5)); 
    

    double angle = 0.0;
    if (generator > 0.0) { 
        angle = M_PI_2 - atan2(1.0 / generator - 1.0, 1.0); 
    }
    return angle;
}

double MOS::angle() const {
    double angle = angleStd();
    for (int d = 0; d < path.size(); d++) {
        if (path[d]) {
            angle = atan2(tan(angle)-1, 1);
        }else{
            angle = atan2(1, 1/tan(angle)-1);
        }
    }
    return angle;
}

std::vector<bool> calcPath(int a, int b){
    std::vector<bool> path;
    int a_ = a;
    int b_ = b;
    while (a_ > 1 || b_ > 1) {
        if (a_ > b_) {
            a_ -= b_;
            path.push_back(false);
        } else {
            b_ -= a_;
            path.push_back(true);
        }
    }
    std::reverse(path.begin(), path.end());
    return path;
}


Vector2i applyPath(const std::vector<bool> path, const Vector2i& v) {
    int a = v.x;
    int b = v.y;
    for (bool p : path) {
        if (p) {
            b += a;
        } else {
            a += b;
        }
        std::cout << " " << a << " " << b << "\n";
    }
    return {a,b};
}

MOS& MOS::fromParams(int a, int b, int m, double e, double g){
    static MOS _self;
    _self.adjustParams(a, b, m, e, g);
    return _self;
}

void MOS::updateVectors(){
    Vector2i v1 = {1, 0};
    Vector2i v2 = {0, 1};
    double v1_fr = this->impliedAffine.apply(v1).x;
    double v2_fr = this->impliedAffine.apply(v2).x;
    if (v1_fr > v2_fr) {
        this->L_vec = v1;
        this->s_vec = v2;
        this->L_fr = v1_fr;
        this->s_fr = v2_fr;
    } else {
        this->L_vec = v2;
        this->s_vec = v1;
        this->L_fr = v2_fr;
        this->s_fr = v1_fr;
    }
    this->chroma_vec = this->L_vec - this->s_vec;
    this->chroma_fr = this->L_fr - this->s_fr;
}

void MOS::adjustParams(int a, int b, int m, double e, double g){
    assert(a > 0);
    assert(b > 0);
    assert(0.0 <= g && g <= 1.0);

    int n = a + b;
    int r = gcd(a, b);
    int a0 = a / r;
    int b0 = b / r;
    int n0 = a0 + b0;

    this->a = a;
    this->b = b;
    this->n = n;
    this->a0 = a0;
    this->b0 = b0;
    this->n0 = n0;
    this->mode = m;
    this->repetitions = r;
    
    this->equave = e;
    this->period = e / r;
    this->generator = g;

    this->path = calcPath(a0, b0);
    this->depth = this->path.size();
    this->v_gen = applyPath(this->path, {1, 0});
    this->impliedAffine = calcImpliedAffine();

    this->updateVectors();

    this->base_scale = Scale::fromAffine(this->impliedAffine, 1.0, n+1, 0);


    this->mosTransform = IntegerAffineTransform::linearFromTwoDots(
        {1, 0}, {1, 1},
        v_gen, {a0, b0}
    );
}

MOS& MOS::fromG(int depth, int m, double g, double e, int repetitions){
    static MOS _self;
    _self.repetitions = repetitions;
    int a0 = 1;
    int b0 = 1;
    double a_len = g;
    double b_len = 1.0 - g;
    for (int i = 0; i < depth; i++) {
        if (a_len > b_len) {
            b0+=a0;
            a_len -= b_len;
        } else {
            a0+=b0;
            b_len -= a_len;
        }
    }
    _self.adjustParams(a0*repetitions, b0*repetitions, m, e, g);
    return _self;
}

double MOS::gFromAngle(double angle){
    std::vector<bool> reversed_path = path;
    std::reverse(reversed_path.begin(), reversed_path.end());

    double angle_ = angle;
    for (int d = 0; d < reversed_path.size(); d++) {
        if (reversed_path[d]) {
            angle_ = atan2(tan(angle_)+1, 1);
        }else{
            angle_ = atan2(1, 1/tan(angle_)+1);
        }
    }

    // print the angle
    //std::cout << "Calculated AngleSTD: " << M_2_PI * angle_ << "\n";

    return 1.0 / (1.0 + tan((M_PI_2 - angle_)));
}


//static MOS& fromImpliedAffine(const AffineTransform& A, int repetitions){
//    static MOS _self;
//    _self.repetitions = repetitions;
//    _self.adjustParamsFromImpliedAffine(A);
//    return _self;
//};
//
//void adjustParamsFromImpliedAffine(const AffineTransform& A, int repetitions){
//
//}

AffineTransform MOS::calcImpliedAffine() const {
    double q = 0.5/n0;
    return affineFromThreeDots(
        {0,0}, {(double)v_gen.x, (double)v_gen.y}, {(double)a0,(double)b0},
        {0,q*(2*mode+1)}, {generator * period, q*(2*mode+3)}, {period, q*(2*mode+1)}
    );
};

std::string MOS::accidentalString(Vector2i v, bool swap) const{
    
    int acc = floor( ((swap?-1:1)* (v.y * a0 - v.x * b0) - 2.0) / n0 + 1);
    if (L_vec.x == 1){
        acc *= -1;
    }
    std::string result = "";
    if (acc != 0) {
        while (acc < 0) {
            acc += 1;
            result += "♭";
        }
        while (acc > 0) {
            acc -= 1;
            result += "♯";
        }
    }
    return result;
}

std::string MOS::nodeLabelDigit(Vector2i v) const{
    int dia = (v.x + v.y + 128*n) % n;
    std::string result = std::to_string(dia+1);
    result += accidentalString(v);
    return result;
};
std::string MOS::nodeLabelLetter(Vector2i v) const{
    int dia = (v.x + v.y + 2 + 128*n) % n;
    char letter = 'A' + dia;
    std::string result(1, letter);
    result += accidentalString(v);
    return result;
};
std::string MOS::nodeLabelLetterWithOctaveNumber(Vector2i v, int middle_C_octave) const{
    std::string result = nodeLabelLetter(v);
    int octave = middle_C_octave + floor((.0 + v.x + v.y) / n);
    result += std::to_string(octave);
    return result;
};


}

