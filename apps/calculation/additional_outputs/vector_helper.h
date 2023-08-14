#ifndef CALCULATION_ADDITIONAL_OUTPUTS_VECTOR_HELPER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_VECTOR_HELPER_H

#include <poincare/expression.h>

namespace Calculation {

namespace VectorHelper {

Poincare::Expression BuildVectorNorm(const Poincare::Expression exactOutput,
                                     Poincare::Context* context);

}

}  // namespace Calculation

#endif
