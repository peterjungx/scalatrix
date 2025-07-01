# Scalatrix Test Suite

This directory contains comprehensive test cases for the scalatrix library using the Catch2 testing framework.

## Test Files

### Core Component Tests
- **test_affine_transform.cpp** - Tests for affine transformation functions (identity, translation, scaling, rotation, shear)
- **test_node.cpp** - Tests for Node class including construction, encapsulation, backward compatibility, tempering functionality, and deviation labels
- **test_scale.cpp** - Tests for Scale class including construction, fromAffine generation, node deviation labels, tempering, and retuning
- **test_mos.cpp** - Tests for MOS (Moment of Symmetry) class including construction, path generation, scale generation, retuning operations, coordinate mapping, and node labeling
- **test_pitch_sets.cpp** - Tests for pitch set generation functions (ET, JI, Harmonic Series) and prime list generation
- **test_label_calculator.cpp** - Tests for LabelCalculator functionality and note labeling systems

### Integration Tests
- **test_integration.cpp** - Comprehensive integration tests combining multiple scalatrix components to test complete workflows

## Building and Running

From the scalatrix/tests directory:

```bash
mkdir -p build && cd build
cmake ..
make
```

Run all tests:
```bash
ctest --verbose
```

Run individual test suites:
```bash
./test_scale
./test_node
./test_mos
./test_pitch_sets
./test_label_calculator
./test_integration
./test_affine_transform
```

## Test Coverage

The test suite covers:

### Core Functionality
- **Affine Transformations**: Identity, translation, scaling, rotation, shear, and complex transformations
- **Node Management**: Construction, encapsulation, backward compatibility, tempering, deviation labels
- **Scale Generation**: Construction from affine transforms, node sorting, frequency calculations
- **MOS Systems**: Construction from generators and parameters, path generation, scale generation
- **Pitch Sets**: Equal temperament, just intonation, harmonic series generation
- **Node Labeling**: Deviation labels, normalized labels, letter-based labeling

### Advanced Features
- **Retuning Operations**: One-point, two-point, three-point retuning and reset operations
- **Tempering**: Applying pitch sets to scales for just intonation and equal temperament
- **Coordinate Mapping**: Mapping coordinates between different MOS systems
- **Integration Workflows**: Complete workflows simulating the native_example.cpp

### Error Handling and Edge Cases
- Large and negative coordinates
- Different MOS system parameters
- Various pitch set configurations
- Deterministic behavior verification

## Notes

- Tests are based on actual output from the scalatrix_example to ensure accuracy
- All tests use reasonable tolerance values for floating-point comparisons
- Integration tests simulate real-world usage patterns from the native example
- Test data is validated against actual scalatrix library behavior

## Test Statistics

- **42 individual test cases** across 7 test files
- **All tests passing** (959+ assertions total)
- **Comprehensive coverage** of all major scalatrix components