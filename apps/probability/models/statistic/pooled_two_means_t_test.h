#ifndef PROBABILITY_MODELS_STATISTIC_POOLED_TWO_MEANS_TEST_H
#define PROBABILITY_MODELS_STATISTIC_POOLED_TWO_MEANS_TEST_H

#include "interfaces/distributions.h"
#include "interfaces/significance_tests.h"
#include "two_means_t_test.h"

namespace Probability {

class PooledTwoMeansTTest : public TwoMeansTTest {
public:
  I18n::Message title() const override { return PooledTwoMeans::Title(); }
  void computeTest() override { PooledTwoMeans::ComputeTest(this); }
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_POOLED_TWO_MEANS_TEST_H */
