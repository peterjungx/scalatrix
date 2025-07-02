#include "scalatrix/lattice.hpp"
#include <vector>
#include <cassert>
#include <cmath>

namespace scalatrix {

double isnull (double x) {
    return std::abs(x) < 1e-6;
}   

std::pair<Vector2i, Vector2i> findClosestWithinStrip(const AffineTransform& M) {

    auto M_inv = M.inverse();
    Vector2d v(1.0, 0.0);
    Vector2d zv = M_inv * v;

    Vector2i r, s;
    Vector2d z;
    bool has_first = false, has_second = false;
    if (isnull(zv.x)){
        s = Vector2i(0, zv.y>0?1:-1);
        has_first = true;
        z = M * Vector2i(1, 0);
        if (std::abs(z.y) < 1){
            r = Vector2i(z.y>0?1:-1, 0);
            has_second = true;
        }else{
            r = s;
        }
    }
    else if (isnull(zv.y)){
        s = Vector2i(zv.x>0?1:-1, 0);
        has_first = true;
        z = M * Vector2i(0, 1);
        if (std::abs(z.y) < 1){
            r = Vector2i(0, z.y>0?1:-1);
            has_second = true;
        }else{
            r = s;
        }
    }
    else if (isnull(zv.x - zv.y)){
        s = Vector2i(zv.x>0?1:-1, 0);
        has_first = true;
        z = M * Vector2i(1, 0);
        if (std::abs(z.y) < 1){
            r = Vector2i(0, z.y>0?1:-1);
            has_second = true;
        }else{
            r = s;
        }
    }
    else 
    {
        double e;
        bool x_large = std::abs(zv.x) > std::abs(zv.y);
        if (x_large){
            e = std::abs(zv.y/zv.x);
        }else{
            e = std::abs(zv.x/zv.y);
        }

        int a = std::floor(e);
        std::vector<int> num = {1, a};
        std::vector<int> den = {0, 1};
        double f = e - a;

        while (num.size() < 20) {
            if (x_large){
                r.x = den.back();
                r.y = (zv.x*zv.y>0?1:-1) * num.back();
            }else{
                r.x = num.back();
                r.y = (zv.x*zv.y>0?1:-1) * den.back();
            }
            z = M * Vector2d((double)r.x, (double)r.y);
            if (z.x < 0){
                z = -z;
                r = -r;
            }
            if (z.y < 1 && z.y > -1){
                if(!has_first){
                    has_first = true;
                    s = r;
                }
                else{
                    has_second = true;
                    break;
                }
            }
            a = std::floor(1/f);
            f = 1/f - a;
            num.push_back(a*num[num.size()-1] + num[num.size()-2]);
            den.push_back(a*den[den.size()-1] + den[den.size()-2]);
        }
    }
    if (!has_first){
        return {{0,0}, {0,0}};
    }
    if (!has_second){
        return {s, s};
    }

    Vector2i t;
    Vector2d zr, zs;
    bool changed = true;
    int cnt;

    zr = M * r;
    zs = M * s;
    if (zr.x > zs.x){
        std::swap(r, s);
        std::swap(zr, zs);
    }

    while (changed){
        changed = false;
        cnt = 0;
        if (zs.x>0){
            while(zs.x > 0 && zs.y > -1 && zs.y < 1){
                t = s;
                s -= r;
                zs = M * s;
                changed = true;
                cnt++;
            }
            s = t;
            zs = M * s;             
            if (cnt==1) {
                changed = false;
            }
            if (zr.x > zs.x){
                std::swap(r, s);
                std::swap(zr, zs);
                changed = true;
            }   
        }else{
            break;
        }
    } 
    assert(zr.x >= 0 && zr.x + zs.x > 0);
    assert(zr.x <= zs.x);
    return {r, s};
}

} // namespace scalatrix