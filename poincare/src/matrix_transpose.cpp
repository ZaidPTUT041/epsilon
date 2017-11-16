#include <poincare/matrix_transpose.h>
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include <poincare/division.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type MatrixTranspose::type() const {
  return Type::MatrixTranspose;
}

Expression * MatrixTranspose::clone() const {
  MatrixTranspose * a = new MatrixTranspose(m_operands, true);
  return a;
}

Expression * MatrixTranspose::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  Expression * op = editableOperand(0);
  if (op->type() == Type::Matrix) {
    Matrix * transpose = static_cast<Matrix *>(op)->createTranspose();
    return replaceWith(transpose, true);
  }
  if (!op->recursivelyMatches(Expression::isMatrix)) {
    return replaceWith(op, true);
  }
  return this;
}

template<typename T>
Expression * MatrixTranspose::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Expression * input = operand(0)->evaluate<T>(context, angleUnit);
  Expression * result = nullptr;
  if (input->type() == Type::Complex) {
    result = input->clone();
  } else {
    assert(input->type() == Type::Matrix);
    result = static_cast<Matrix *>(input)->createTranspose();
  }
  delete input;
  return result;
}

}
