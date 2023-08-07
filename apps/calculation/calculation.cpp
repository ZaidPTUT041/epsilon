#include "calculation.h"

#include <apps/apps_container_helper.h>
#include <apps/global_preferences.h>
#include <apps/shared/expression_display_permissions.h>
#include <apps/shared/poincare_helpers.h>
#include <escher/scrollable_multiple_layouts_view.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/matrix.h>
#include <poincare/nonreal.h>
#include <poincare/trigonometry.h>
#include <poincare/undefined.h>
#include <poincare/unit.h>
#include <string.h>

#include <algorithm>
#include <cmath>

#include "additional_outputs/scientific_notation_helper.h"
#include "app.h"
#include "poincare/expression_node.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

bool Calculation::operator==(const Calculation &c) {
  return strcmp(inputText(), c.inputText()) == 0 &&
         strcmp(approximateOutputText(NumberOfSignificantDigits::Maximal),
                c.approximateOutputText(NumberOfSignificantDigits::Maximal)) ==
             0 &&
         strcmp(approximateOutputText(NumberOfSignificantDigits::UserDefined),
                c.approximateOutputText(
                    NumberOfSignificantDigits::UserDefined)) == 0
         /* Some calculations can make appear trigonometric functions in their
          * exact output. Their argument will be different with the angle unit
          * preferences but both input and approximate output will be the same.
          * For example, i^(sqrt(3)) = cos(sqrt(3)*pi/2)+i*sin(sqrt(3)*pi/2) if
          * angle unit is radian and i^(sqrt(3)) =
          * cos(sqrt(3)*90+i*sin(sqrt(3)*90) in degree. */
         && strcmp(exactOutputText(), c.exactOutputText()) == 0;
}

Calculation *Calculation::next() const {
  const char *result =
      reinterpret_cast<const char *>(this) + sizeof(Calculation);
  for (int i = 0; i < k_numberOfExpressions; i++) {
    // Pass inputText, exactOutputText, ApproximateOutputText x2
    result = result + strlen(result) + 1;
  }
  return reinterpret_cast<Calculation *>(const_cast<char *>(result));
}

const char *Calculation::approximateOutputText(
    NumberOfSignificantDigits numberOfSignificantDigits) const {
  const char *exactOutput = exactOutputText();
  const char *approximateOutputTextWithMaxNumberOfDigits =
      exactOutput + strlen(exactOutput) + 1;
  if (numberOfSignificantDigits == NumberOfSignificantDigits::Maximal) {
    return approximateOutputTextWithMaxNumberOfDigits;
  }
  return approximateOutputTextWithMaxNumberOfDigits +
         strlen(approximateOutputTextWithMaxNumberOfDigits) + 1;
}

Expression Calculation::input() {
  return Expression::Parse(m_inputText, nullptr);
}

Expression Calculation::exactOutput() {
  /* Because the angle unit might have changed, we do not simplify again. We
   * thereby avoid turning cos(Pi/4) into sqrt(2)/2 and displaying
   * 'sqrt(2)/2 = 0.999906' (which is totally wrong) instead of
   * 'cos(pi/4) = 0.999906' (which is true in degree). */
  return Expression::Parse(exactOutputText(), nullptr);
}

Expression Calculation::approximateOutput(
    NumberOfSignificantDigits numberOfSignificantDigits) {
  // clang-format off
  /* Warning:
   * Since quite old versions of Epsilon, the Expression 'exp' was used to be
   * approximated again to ensure its content was in the expected form - a
   * linear combination of Decimal.
   * However, since the approximate output may contain units and that a
   * Poincare::Unit approximates to undef, thus it must not be approximated
   * anymore.
   * We have to keep two serializations of the approximation outputs:
   * - one with the maximal significant digits, to be used by 'Ans' or when
   *   handling 'OK' event on the approximation output.
   * - one with the displayed number of significant digits that we parse to
   *   create the displayed layout. If we used the other serialization to
   *   create the layout, the result of the parsing could be an Integer which
   *   does not take the number of significant digits into account when creating
   *   its layout. This would lead to wrong number of significant digits in the
   *   layout.
   *   For instance:
   *        Number of asked significant digits: 7
   *        Input: "123456780", Approximate output: "1.234567E8"
   *
   *  |--------------------------------------------------------------------------------------|
   *  | Number of significant digits | Approximate text | Parse expression    | Layout       |
   *  |------------------------------+------------------+---------------------+--------------|
   *  | Maximal                      | "123456780"      | Integer(123456780)  | "123456780"  |
   *  |------------------------------+------------------+---------------------+--------------|
   *  | User defined                 | "1.234567E8"     | Decimal(1.234567E8) | "1.234567E8" |
   *  |--------------------------------------------------------------------------------------|
   *
   */
  // clang-format on
  return Expression::Parse(approximateOutputText(numberOfSignificantDigits),
                           nullptr);
}

Layout Calculation::createInputLayout() {
  ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    Expression e = input();
    if (!e.isUninitialized()) {
      return e.createLayout(Preferences::PrintFloatMode::Decimal,
                            PrintFloat::k_numberOfStoredSignificantDigits,
                            App::app()->localContext());
    }
  }
  return Layout();
}

Layout Calculation::createExactOutputLayout(bool *couldNotCreateExactLayout) {
  ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    Expression e = exactOutput();
    if (!e.isUninitialized()) {
      return e.createLayout(Preferences::PrintFloatMode::Decimal,
                            PrintFloat::k_numberOfStoredSignificantDigits,
                            App::app()->localContext());
    }
  }
  *couldNotCreateExactLayout = true;
  return Layout();
}

Layout Calculation::createApproximateOutputLayout(
    bool *couldNotCreateApproximateLayout) {
  ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    Expression e = approximateOutput(NumberOfSignificantDigits::UserDefined);
    if (!e.isUninitialized()) {
      return PoincareHelpers::CreateLayout(e, App::app()->localContext());
    }
  }
  *couldNotCreateApproximateLayout = true;
  return Layout();
}

KDCoordinate Calculation::height(bool expanded) {
  KDCoordinate h = expanded ? m_expandedHeight : m_height;
  assert(h >= 0);
  return h;
}

void Calculation::setHeights(KDCoordinate height, KDCoordinate expandedHeight) {
  m_height = height;
  m_expandedHeight = expandedHeight;
}

static bool ShouldOnlyDisplayExactOutput(Expression input) {
  /* If the input is a "store in a function", do not display the approximate
   * result. This prevents x->f(x) from displaying x = undef. */
  assert(!input.isUninitialized());
  return input.type() == ExpressionNode::Type::Store &&
         input.childAtIndex(1).type() == ExpressionNode::Type::Function;
}

Calculation::DisplayOutput Calculation::displayOutput(Context *context) {
  if (m_displayOutput != DisplayOutput::Unknown) {
    return m_displayOutput;
  }
  Expression inputExp = input();
  Expression outputExp = exactOutput();
  if (inputExp.isUninitialized() || outputExp.isUninitialized() ||
      ShouldOnlyDisplayExactOutput(inputExp)) {
    m_displayOutput = DisplayOutput::ExactOnly;
  } else if (
      /* If the exact and approximate outputs are equal (with the
       * UserDefined number of significant digits), do not display the exact
       * output. Indeed, in this case, the layouts are identical. */
      strcmp(approximateOutputText(NumberOfSignificantDigits::UserDefined),
             exactOutputText()) == 0 ||
      // If the exact result is 'undef'
      strcmp(exactOutputText(), Undefined::Name()) == 0 ||
      // If the approximate output is 'nonreal'
      strcmp(approximateOutputText(NumberOfSignificantDigits::Maximal),
             Nonreal::Name()) == 0 ||
      // If the approximate output is 'undef'
      strcmp(approximateOutputText(NumberOfSignificantDigits::Maximal),
             Undefined::Name()) == 0 ||
      // Other conditions are factorized in ExpressionDisplayPermissions
      ExpressionDisplayPermissions::ShouldOnlyDisplayApproximation(
          inputExp, outputExp,
          approximateOutput(NumberOfSignificantDigits::UserDefined), context)) {
    m_displayOutput = DisplayOutput::ApproximateOnly;
  } else if (inputExp.recursivelyMatches(Expression::IsApproximate, context) ||
             outputExp.recursivelyMatches(Expression::IsApproximate, context) ||
             inputExp.recursivelyMatches(Expression::IsPercent, context)) {
    m_displayOutput = DisplayOutput::ExactAndApproximateToggle;
  } else {
    m_displayOutput = DisplayOutput::ExactAndApproximate;
  }
  return m_displayOutput;
}

Calculation::EqualSign
Calculation::exactAndApproximateDisplayedOutputsEqualSign(Context *context) {
  // TODO: implement a UserCircuitBreaker
  if (m_equalSign != EqualSign::Unknown) {
    return m_equalSign;
  }
  if (m_displayOutput == DisplayOutput::ExactOnly ||
      m_displayOutput == DisplayOutput::ApproximateOnly) {
    /* Do not compute the equal sign if not needed.
     * We don't override m_equalSign here in case it needs to be computed later
     * */
    return EqualSign::Approximation;
  }
  /* Displaying the right equal symbol is less important than displaying a
   * result, so we do not want exactAndApproximateDisplayedOutputsEqualSign to
   * create a pool failure that would prevent from displaying a result that we
   * managed to compute. We thus encapsulate the method in an exception
   * checkpoint: if there was not enough memory on the pool to compute the equal
   * sign, just return EqualSign::Approximation.
   * We can safely use an exception checkpoint here because we are sure of not
   * modifying any pre-existing node in the pool. We are sure there cannot be a
   * Store in the exactOutput. */
  ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    Expression exactOutputExpression = exactOutput();
    if (input().recursivelyMatches(Expression::IsPercent, context)) {
      /* When the input contains percent, the exact expression is not fully
       * reduced so we need to reduce it again prior to computing equal sign */
      PoincareHelpers::CloneAndSimplify(
          &exactOutputExpression, context, ReductionTarget::User,
          SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
    }
    m_equalSign = Expression::ExactAndApproximateExpressionsAreEqual(
                      exactOutputExpression,
                      approximateOutput(NumberOfSignificantDigits::UserDefined))
                      ? EqualSign::Equal
                      : EqualSign::Approximation;
    return m_equalSign;
  } else {
    /* Do not override m_equalSign in case there is enough room in the pool
     * later to compute it. */
    return EqualSign::Approximation;
  }
}

bool Calculation::ForbidAdditionalResults(Expression input,
                                          Expression exactOutput,
                                          Expression approximateOutput) {
  /* Special case for Store:
   * Store nodes have to be at the root of the expression, which prevents
   * from creating new expressions with store node as a child. We don't
   * return any additional outputs for them to avoid bothering with special
   * cases. */
  return Preferences::sharedPreferences->examMode().forbidAdditionalResults() ||
         input.isUninitialized() || exactOutput.isUninitialized() ||
         approximateOutput.isUninitialized() ||
         approximateOutput.isUndefined() ||
         input.type() == ExpressionNode::Type::Store ||
         exactOutput.type() == ExpressionNode::Type::List ||
         approximateOutput.type() == ExpressionNode::Type::List ||
         approximateOutput.recursivelyMatches(
             [](const Expression e, Context *c) {
               return e.isOfType({ExpressionNode::Type::Infinity});
             },
             nullptr);
}

static bool expressionIsInterestingFunction(Expression e) {
  assert(!e.isUninitialized());
  if (e.isOfType({ExpressionNode::Type::Opposite,
                  ExpressionNode::Type::Parenthesis})) {
    return expressionIsInterestingFunction(e.childAtIndex(0));
  }
  return !e.isNumber() &&
         !e.isOfType({ExpressionNode::Type::ConstantMaths,
                      ExpressionNode::Type::UnitConvert}) &&
         !e.recursivelyMatches(Expression::IsSequence) &&
         e.numberOfNumericalValues() == 1;
}

bool Calculation::HasFunctionAdditionalResults(Expression input,
                                               Expression approximateOutput) {
  // We want a single numerical value and to avoid showing the identity function
  assert(!input.isUninitialized());
  assert(!approximateOutput.isUndefined());
  return approximateOutput.type() != ExpressionNode::Type::Nonreal &&
         expressionIsInterestingFunction(input);
}

bool Calculation::HasIntegerAdditionalResults(Expression exactOutput) {
  assert(!exactOutput.isUninitialized());
  return exactOutput.isBasedIntegerCappedBy(
      k_maximalIntegerWithAdditionalInformation);
}

bool Calculation::HasRationalAdditionalResults(Expression exactOutput) {
  // Find forms like [12]/[23] or -[12]/[23]
  assert(!exactOutput.isUninitialized());
  return exactOutput.isDivisionOfIntegers() ||
         (exactOutput.type() == ExpressionNode::Type::Opposite &&
          exactOutput.childAtIndex(0).isDivisionOfIntegers());
}

Calculation::AdditionalInformations Calculation::additionalInformations() {
  Context *globalContext =
      AppsContainerHelper::sharedAppsContainerGlobalContext();
  Expression i = input();
  Expression a = approximateOutput(NumberOfSignificantDigits::Maximal);
  Expression e = displayOutput(globalContext) !=
                         Calculation::DisplayOutput::ApproximateOnly
                     ? exactOutput()
                     : a;
  if (ForbidAdditionalResults(i, e, a)) {
    return AdditionalInformations{};
  }
  Preferences *preferences = Preferences::sharedPreferences;
  /* Using the approximated output instead of the user input to guess the
   * complex format makes additional results more consistent when the user has
   * created complexes in Complex mode and then switched back to Real mode. */
  Preferences::ComplexFormat complexFormat =
      Preferences::UpdatedComplexFormatWithExpressionInput(
          preferences->complexFormat(), a, nullptr);
  bool isComplex = a.hasDefinedComplexApproximation<double>(
      nullptr, complexFormat, preferences->angleUnit());
  /* Trigonometry additional results are displayed if either input or output is
   * a direct or inverse trigonometric function. Indeed, we want to capture both
   * cases:
   *
   * - > input: cos(60)
   *   > output: 1/2
   * - > input: 2cos(2) - cos(2)
   *   > output: cos(2)
   * However if the result is complex, it is treated as a complex result instead
   */
  if (!isComplex) {
    /* If only the input is trigonometric, but it contains symbols, do not
     * display trigonometric additional informations, in case the symbol value
     * is later modified/deleted in the storage and can't be retrieved.
     * Ex: 0->x; tan(x); 3->x;
     * => The additional results of tan(x) become inconsistent. And if x is
     * deleted, it crashes. */
    bool inputHasSymbols = i.deepIsSymbolic(
        globalContext, SymbolicComputation::DoNotReplaceAnySymbol);
    if ((Trigonometry::isInverseTrigonometryFunction(i) && !inputHasSymbols) ||
        Trigonometry::isInverseTrigonometryFunction(e)) {
      // The angle cannot be complex since Expression a isn't
      return AdditionalInformations{.inverseTrigonometry = true};
    }
    Expression directExpression;
    if (Trigonometry::isDirectTrigonometryFunction(e)) {
      directExpression = e;
    } else if (Trigonometry::isDirectTrigonometryFunction(i) &&
               !inputHasSymbols) {
      directExpression = i;
    }

    if (!directExpression.isUninitialized()) {
      Expression angle = directExpression.childAtIndex(0);
      Expression unit;
      PoincareHelpers::CloneAndReduceAndRemoveUnit(
          &angle, globalContext, ReductionTarget::User, &unit);
      // The angle must be real.
      if ((unit.isPureAngleUnit() || unit.isUninitialized()) &&
          std::isfinite(PoincareHelpers::ApproximateToScalar<double>(
              angle, globalContext))) {
        return AdditionalInformations{.directTrigonometry = true};
      }
    }
  }
  if (e.hasUnit()) {
    Expression unit;
    Expression eClone = e.clone();
    PoincareHelpers::CloneAndReduceAndRemoveUnit(
        &eClone, globalContext, ReductionTarget::User, &unit,
        SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined,
        UnitConversion::None);
    double value =
        PoincareHelpers::ApproximateToScalar<double>(eClone, globalContext);
    if (!unit.isUninitialized() &&
        (Unit::ShouldDisplayAdditionalOutputs(
             value, unit,
             GlobalPreferences::sharedGlobalPreferences->unitFormat()) ||
         UnitComparison::ShouldDisplayUnitComparison(value, unit))) {
      /* Sometimes with angle units, the reduction with UnitConversion::None
       * will be defined but not the reduction with UnitConversion::Default,
       * which will make the unit list controller crash.  */
      unit = Expression();
      PoincareHelpers::CloneAndReduceAndRemoveUnit(
          &e, globalContext, ReductionTarget::User, &unit);
      if (!unit.isUninitialized()) {
        return AdditionalInformations{.unit = true};
      }
    }
    return AdditionalInformations{};
  }
  if (e.type() == ExpressionNode::Type::Matrix) {
    if (static_cast<const Matrix &>(e).vectorType() !=
        Array::VectorType::None) {
      return AdditionalInformations{.vector = true};
    }
    return AdditionalInformations{.matrix = true};
  }
  if (isComplex) {
    return AdditionalInformations{.complex = true};
  }
  AdditionalInformations additionalInformations = {};
  if (a.type() != ExpressionNode::Type::Nonreal &&
      preferences->displayMode() != Preferences::PrintFloatMode::Scientific) {
    // There should be no units at this point
    assert(!a.hasUnit());
    additionalInformations.scientificNotation =
        ScientificNotationHelper::HasAdditionalOutputs(a, globalContext);
  }
  if (HasFunctionAdditionalResults(i, a)) {
    additionalInformations.function = true;
  }
  if (HasIntegerAdditionalResults(e)) {
    additionalInformations.integer = true;
  } else if (HasRationalAdditionalResults(e)) {
    additionalInformations.rational = true;
  }
  return additionalInformations;
}

}  // namespace Calculation
