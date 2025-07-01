#include <scalatrix/node.hpp>
#include <iostream>

using namespace scalatrix;

int main() {
    std::cout << "=== Node closestPitch vs temperedPitch Demo ===" << std::endl;
    
    // Create a node with some tuning
    Node node;
    node.setTuningCoord({0.51, 0.0}); // Actual pitch at 0.51 (612 cents)
    
    // Set tempered pitch (what the scale was tempered to)
    PitchSetPitch temperedPitch = {"C", 0.5};  // Tempered to C at 600 cents
    node.setTemperedPitch(temperedPitch);
    
    // Set closest pitch (nearest pitch in a reference pitch set)
    PitchSetPitch closestPitch = {"C#", 0.52}; // Closest is C# at 624 cents  
    node.setClosestPitch(closestPitch);
    
    std::cout << "Node actual pitch: " << node.getTuningCoord().x << " (log2fr)" << std::endl;
    std::cout << "Tempered to: " << node.getTemperedPitch().label 
              << " at " << node.getTemperedPitch().log2fr << " (log2fr)" << std::endl;
    std::cout << "Closest pitch: " << node.getClosestPitch().label 
              << " at " << node.getClosestPitch().log2fr << " (log2fr)" << std::endl;
    std::cout << std::endl;
    
    // Show deviation from tempered pitch (default behavior)
    std::string temperedLabel = node.deviationLabel(0.1, false, DeviationReference::TEMPERED);
    std::cout << "Deviation from tempered pitch: " << temperedLabel << std::endl;
    
    // Show deviation from closest pitch
    std::string closestLabel = node.deviationLabel(0.1, false, DeviationReference::CLOSEST);
    std::cout << "Deviation from closest pitch: " << closestLabel << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== Legacy Compatibility ===" << std::endl;
    
    // Demonstrate that legacy field access still works
    node.temperedPitch.label = "D";
    node.temperedPitch.log2fr = 0.52;
    node.closestPitch.label = "D#";
    node.closestPitch.log2fr = 0.53;
    
    std::cout << "After using legacy field access:" << std::endl;
    std::cout << "Tempered: " << node.getTemperedPitch().label 
              << " at " << node.getTemperedPitch().log2fr << std::endl;
    std::cout << "Closest: " << node.getClosestPitch().label 
              << " at " << node.getClosestPitch().log2fr << std::endl;
              
    return 0;
}