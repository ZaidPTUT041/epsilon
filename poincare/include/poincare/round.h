#ifndef POINCARE_ROUND_H
#define POINCARE_ROUND_H

#include <poincare/function.h>

namespace Poincare {

class Round : public Function {
public:
  Round();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  Evaluation * privateEvaluate(Context & context, AngleUnit angleUnit) const override;
};

}

#endif


