#ifndef SCALATRIX_LATTICE_HPP
#define SCALATRIX_LATTICE_HPP

#include "affine_transform.hpp"
#include <utility>

namespace scalatrix {
    
std::pair<Vector2i, Vector2i> findClosestWithinStrip(const AffineTransform& M);

} // namespace scalatrix

#endif // SCALATRIX_LATTICE_HPP