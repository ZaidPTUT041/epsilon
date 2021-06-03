#include "one_proportion_statistic.h"

#include <cmath>
#include <poincare/normal_distribution.h>

#include "probability/app.h"

namespace Probability {

void OneProportionStatistic::computeTest() {
  Data::Data * data = App::app()->snapshot()->data();
  float p0 = data->hypothesisParams()->firstParam();
  // TODO weird to know the position
  float x = data->testInputParams()->paramAtIndex(0);
  int n = data->testInputParams()->paramAtIndex(1);
  float p = _pEstimate(x, n);
  // Z
  m_z = _z(p0, p, n);
  // p-value
  char fakeOp = static_cast<char>(data->hypothesisParams()->op());
  m_pValue = _pVal(m_z, fakeOp);

}

void OneProportionStatistic::computeInterval() {
  InputParameters * params = App::app()->snapshot()->data()->testData()->inputParameters();
  // TODO super weird
  float x = params->paramAtIndex(0);
  int n = params->paramAtIndex(1);
  float confidenceLevel = params->threshold();
  m_pEstimate = _pEstimate(x, n);
  m_zCritical = _zCritical(confidenceLevel);
  m_SE = _SE(m_pEstimate, n);
  m_ME = _ME(m_zCritical, m_SE);
}

float OneProportionStatistic::_pEstimate(float x, float n) {
  return x / n;
}


float OneProportionStatistic::_z(float p0, float p, int n) {
  return std::abs((p - p0) / sqrt(p0 * (1 - p0) / n));
}

float OneProportionStatistic::_SE(float pEstimate, int n) {
  return sqrt(pEstimate * (1 - pEstimate) / n);
}

}  // namespace Probability
