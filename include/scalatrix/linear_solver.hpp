#ifndef SCALATRIX_LINEAR_SOLVER_HPP
#define SCALATRIX_LINEAR_SOLVER_HPP

#include <array>

namespace scalatrix {

// Simple 6x6 linear system solver using Gaussian elimination with partial pivoting
class LinearSolver6x6 {
public:
    // Solves the system Ax = b where A is 6x6 and b is 6x1
    // Returns the solution vector x
    static std::array<double, 6> solve(const std::array<std::array<double, 6>, 6>& A, 
                                       const std::array<double, 6>& b);
};

} // namespace scalatrix

#endif // SCALATRIX_LINEAR_SOLVER_HPP