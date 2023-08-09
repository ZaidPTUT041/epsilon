#include "trigonometry_helper.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/multiplication.h>
#include <poincare/trigonometry.h>
#include <poincare/unit.h>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

namespace TrigonometryHelper {

Expression ExtractExactAngleFromDirectTrigo(Expression input,
                                            Expression exactOutput,
                                            Context* context) {
  /* Trigonometry additional results are displayed if either input or output is
   * a direct function. Indeed, we want to capture both cases:
   * - > input: cos(60)
   *   > output: 1/2
   * - > input: 2cos(2) - cos(2)
   *   > output: cos(2)
   * However if the result is complex, it is treated as a complex result. */
  Preferences* preferences = Preferences::sharedPreferences;
  assert(!exactOutput.isScalarComplex(preferences));
  Expression directTrigoFunction;
  if (Trigonometry::isDirectTrigonometryFunction(exactOutput)) {
    directTrigoFunction = exactOutput;
  } else if (Trigonometry::isDirectTrigonometryFunction(input) &&
             !input.deepIsSymbolic(
                 context, SymbolicComputation::DoNotReplaceAnySymbol)) {
    /* Do not display trigonometric additional informations, in case the symbol
     * value is later modified/deleted in the storage and can't be retrieved.
     * Ex: 0->x; tan(x); 3->x; => The additional results of tan(x) become
     * inconsistent. And if x is deleted, it crashes. */
    directTrigoFunction = input;
  } else {
    return Expression();
  }
  assert(!directTrigoFunction.isUninitialized() &&
         !directTrigoFunction.isUndefined());
  Expression exactAngle = directTrigoFunction.childAtIndex(0);
  assert(!exactAngle.isUninitialized() && !exactAngle.isUndefined());
  Expression unit;
  PoincareHelpers::CloneAndReduceAndRemoveUnit(&exactAngle, context,
                                               ReductionTarget::User, &unit);
  if (!unit.isUninitialized()) {
    if (!unit.isPureAngleUnit()) {
      return Expression();
    }
    assert(static_cast<Unit&>(unit).representative() ==
           Unit::k_angleRepresentatives + Unit::k_radianRepresentativeIndex);
    /* After a reduction, all angle units are converted to radians, so we
     * convert exactAngle again here to fit the angle unit that will be used
     * in reductions below. */
    exactAngle = Multiplication::Builder(
        exactAngle,
        Trigonometry::UnitConversionFactor(Preferences::AngleUnit::Radian,
                                           preferences->angleUnit()));
  }
  // The angle must be real.
  if (!std::isfinite(
          PoincareHelpers::ApproximateToScalar<double>(exactAngle, context))) {
    return Expression();
  }
  assert(!exactAngle.isUninitialized() && !exactAngle.isUndefined());
  return exactAngle;
}

}  // namespace TrigonometryHelper

}  // namespace Calculation
