#ifndef APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_H

#include <apps/i18n.h>
#include <poincare/layout.h>

#include "probability/models/data_enums.h"
#include "probability/models/hypothesis_params.h"

namespace Probability {

struct ParameterRepr {
  Poincare::Layout m_symbol;
  I18n::Message m_description;
};

/* A Statistic is something that is computed from a sample and whose distribution is known.
 * From its distribution, we can compute statistical test results and confidence intervals.
 */
class Statistic {
public:
  Statistic() : m_threshold(-1) {}
  virtual ~Statistic() = default;

  virtual void computeTest() = 0;
  virtual void computeInterval() = 0;

  virtual float normalizedDensityFunction(float x) const = 0;
  virtual float densityFunction(float x) {
    return normalizedDensityFunction((x - estimate()) / standardError());
  };
  virtual float cumulativeNormalizedDistributionFunction(float x) const = 0;
  virtual float cumulativeNormalizedInverseDistributionFunction(float proba) const = 0;

  // Input
  int numberOfParameters() { return numberOfStatisticParameters() + 1 /* threshold */; }
  float paramAtIndex(int i);
  virtual bool isValidParamAtIndex(int i, float p);
  void setParamAtIndex(int i, float p);
  Poincare::Layout paramSymbolAtIndex(int i) const { return paramReprAtIndex(i).m_symbol; }
  I18n::Message paramDescriptionAtIndex(int i) const { return paramReprAtIndex(i).m_description; }
  float threshold() const { return m_threshold; }
  void setThreshold(float s) { m_threshold = s; }
  HypothesisParams * hypothesisParams() { return &m_hypothesisParams; }
  virtual bool isValidH0(float h0) { return true; }
  virtual bool validateInputs() { return true; };

  // Test statistic
  virtual Poincare::Layout testCriticalValueSymbol() = 0;
  /* Returns the abscissa on the normalized density curve
   * corresponding to the input sample. */
  virtual float testCriticalValue() = 0;
  /* The p-value is the probability of obtaining a results at least
   * as extreme as what was observed with the sample */
  virtual float pValue() = 0;
  virtual bool hasDegreeOfFreedom() = 0;
  virtual float degreeOfFreedom() { return -1; }
  /* Returns the value above/below (depending on the operator) which the probability
   * of landing is inferior to a given significance level. */
  virtual float zAlpha() = 0;
  bool canRejectNull();

  // Confidence interval
  virtual const char * estimateSymbol() { return nullptr; }
  virtual Poincare::Layout estimateLayout() { return Poincare::Layout(); }
  virtual I18n::Message estimateDescription() { return I18n::Message::Default; }
  /* The estimate is the center of the confidence interval,
   * and estimates the parameter of interest. */
  virtual float estimate() = 0;
  Poincare::Layout intervalCriticalValueSymbol();
  /* Returns the critical value above which the probability
   * of landing is inferior to a given confidence level,
   * for the normalized distribution. */
  virtual float intervalCriticalValue() = 0;
  /* Returns the variance estimated from the sample. */
  virtual float standardError() = 0;
  /* Returns the half-width of the confidence interval. */
  virtual float marginOfError() = 0;

  int indexOfThreshold() { return numberOfStatisticParameters(); }
  void initThreshold(Data::SubApp subapp);
  /* Instanciate correct Statistic based on Test and TestType. */
  static void initializeStatistic(Statistic * statistic,
                                  Data::Test t,
                                  Data::TestType type,
                                  Data::CategoricalType categoricalType);

protected:
  virtual int numberOfStatisticParameters() const = 0;
  virtual ParameterRepr paramReprAtIndex(int i) const = 0;
  virtual float * paramArray() = 0;

  float computePValue(float z, HypothesisParams::ComparisonOperator op) const;
  float computeZAlpha(float significanceLevel, HypothesisParams::ComparisonOperator op) const;
  float computeIntervalCriticalValue(float confidenceLevel) const;

  /* Threshold is either the confidence level or the significance level */
  float m_threshold;
  /* Hypothesis chosen */
  HypothesisParams m_hypothesisParams;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_H */
