#include <poincare/helpers.h>
#include <poincare/layout_helper.h>
#include <poincare/list_complex.h>
#include <poincare/list_sort.h>
#include <poincare/point_evaluation.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

int ListSortNode::numberOfChildren() const {
  return ListSort::s_functionHelper.numberOfChildren();
}

int ListSortNode::serialize(char* buffer, int bufferSize,
                            Preferences::PrintFloatMode floatDisplayMode,
                            int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ListSort::s_functionHelper.aliasesList().mainAlias());
}

Layout ListSortNode::createLayout(Preferences::PrintFloatMode floatDisplayMode,
                                  int numberOfSignificantDigits,
                                  Context* context) const {
  return LayoutHelper::Prefix(
      ListSort(this), floatDisplayMode, numberOfSignificantDigits,
      ListSort::s_functionHelper.aliasesList().mainAlias(), context);
}

Expression ListSortNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return ListSort(this).shallowReduce(reductionContext);
}

template <typename T>
Evaluation<T> ListSortNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  Evaluation<T> child = childAtIndex(0)->approximate(T(), approximationContext);
  if (child.type() != EvaluationNode<T>::Type::ListComplex) {
    return Complex<T>::Undefined();
  }
  ListComplex<T> listChild = static_cast<ListComplex<T>&>(child);
  listChild = listChild.sort();
  return std::move(listChild);
}

Expression ListSort::shallowReduce(ReductionContext reductionContext) {
  Expression child = childAtIndex(0);
  if (child.type() != ExpressionNode::Type::List) {
    if (!child.deepIsList(reductionContext.context())) {
      return replaceWithUndefinedInPlace();
    }
    return *this;
  }
  if (recursivelyMatches(Expression::IsUndefined, nullptr)) {
    // Cannot sort if a child is the expression Undefined
    replaceWithInPlace(child);
    return child;
  }

  List list = static_cast<List&>(child);
  ApproximationContext approximationContext(reductionContext, true);
  Evaluation<float> approximatedList = list.approximateToEvaluation<float>(
      reductionContext.context(), reductionContext.complexFormat(),
      reductionContext.angleUnit());
  bool listOfDefinedScalars = approximatedList.isListOfDefinedScalars();
  bool listOfDefinedPoints = approximatedList.isListOfDefinedPoints();
  if (!listOfDefinedScalars && !listOfDefinedPoints) {
    return *this;
  }

  struct Pack {
    List* list;
    ListComplex<float>* listComplex;
    bool scalars;
  };
  Pack pack{&list, static_cast<ListComplex<float>*>(&approximatedList),
            listOfDefinedScalars};
  Helpers::Sort(
      // Swap
      [](int i, int j, void* context, int n) {
        Pack* pack = static_cast<Pack*>(context);
        List* list = reinterpret_cast<List*>(pack->list);
        ListComplex<float>* listComplex =
            reinterpret_cast<ListComplex<float>*>(pack->listComplex);
        assert(list->numberOfChildren() == n);
        assert(list->numberOfChildren() == listComplex->numberOfChildren());
        assert(0 <= i && i < n && 0 <= j && j < n);
        list->swapChildrenInPlace(i, j);
        listComplex->swapChildrenInPlace(i, j);
      },
      // Compare
      Helpers::EvaluateAndCompareInList, &pack, child.numberOfChildren());
  replaceWithInPlace(child);
  return child;
}

template Evaluation<float> ListSortNode::templatedApproximate<float>(
    const ApproximationContext& approximationContext) const;
template Evaluation<double> ListSortNode::templatedApproximate<double>(
    const ApproximationContext& approximationContext) const;

}  // namespace Poincare
