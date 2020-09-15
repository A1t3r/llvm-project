//===- ConstraintSystem.h -  A system of linear constraints. --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_ANALYSIS_CONSTRAINTSYSTEM_H
#define LLVM_ANALYSIS_CONSTRAINTSYSTEM_H

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"

#include <string>

namespace llvm {

class ConstraintSystem {
  /// Current linear constraints in the system.
  /// An entry of the form c0, c1, ... cn represents the following constraint:
  ///   c0 >= v0 * c1 + .... + v{n-1} * cn
  SmallVector<SmallVector<int64_t, 8>, 4> Constraints;

  /// Current greatest common divisor for all coefficients in the system.
  uint32_t GCD = 1;

  // Eliminate constraints from the system using Fourier–Motzkin elimination.
  bool eliminateUsingFM();

  /// Print the constraints in the system, using \p Names as variable names.
  void dump(ArrayRef<std::string> Names) const;

  /// Print the constraints in the system, using x0...xn as variable names.
  void dump() const;

  /// Returns true if there may be a solution for the constraints in the system.
  bool mayHaveSolutionImpl();

public:
  void addVariableRow(const SmallVector<int64_t, 8> &R) {
    assert(Constraints.empty() || R.size() == Constraints.back().size());
    for (const auto &C : R) {
      auto A = std::abs(C);
      GCD = APIntOps::GreatestCommonDivisor({32, (uint32_t)A}, {32, GCD})
                .getZExtValue();
    }
    Constraints.push_back(R);
  }

  /// Returns true if there may be a solution for the constraints in the system.
  bool mayHaveSolution();
};
} // namespace llvm

#endif // LLVM_ANALYSIS_CONSTRAINTSYSTEM_H
