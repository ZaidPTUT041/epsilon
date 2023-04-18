#ifndef ESCHER_EDITABLE_FUNCTION_CELL_H
#define ESCHER_EDITABLE_FUNCTION_CELL_H

#include <escher/layout_field.h>

#include "function_cell.h"

namespace Graph {

class EditableFunctionCell : public AbstractFunctionCell {
 public:
  EditableFunctionCell(Escher::Responder* parentResponder,
                       Escher::InputEventHandlerDelegate* inputEventHandler,
                       Escher::LayoutFieldDelegate* layoutFieldDelegate);
  Escher::LayoutField* layoutField() { return &m_layoutField; }

 private:
  constexpr static KDCoordinate k_expressionMargin = 5;
  int numberOfSubviews() const override { return 2; }
  void layoutSubviews(bool force = false) override;
  void updateSubviewsBackgroundAfterChangingState() override;
  const Escher::LayoutView* expressionView() const override {
    return m_layoutField.expressionView();
  }
  Escher::LayoutView* expressionView() override {
    return m_layoutField.expressionView();
  }
  Escher::View* mainView() override { return &m_layoutField; }
  Escher::LayoutField m_layoutField;
};

}  // namespace Graph

#endif
