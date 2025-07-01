#include "scalatrix/node.hpp"
#include "scalatrix/label_calculator.hpp"

namespace scalatrix {

// Deprecated method - forwarding to LabelCalculator
std::string Node::deviationLabel(double thresholdCents, bool forceExactLabel, 
                                 DeviationReference reference) const {
    return LabelCalculator::deviationLabel(*this, thresholdCents, forceExactLabel, reference);
}

} // namespace scalatrix