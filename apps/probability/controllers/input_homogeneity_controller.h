#ifndef PROBABILITY_CONTROLLERS_INPUT_HOMOGENEITY_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_INPUT_HOMOGENEITY_CONTROLLER_H

#include "probability/abstract/categorical_controller.h"
#include "probability/gui/input_homogeneity_table_cell.h"

namespace Probability {

class InputHomogeneityController : public InputCategoricalController {
public:
  InputHomogeneityController(StackViewController * parent, Escher::ViewController * homogeneityResultsController, HomogeneityTest * statistic, InputEventHandlerDelegate * inputEventHandlerDelegate);

  // ViewController
  const char * title() override { return I18n::translate(I18n::Message::InputHomogeneityControllerTitle); }

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override { m_inputHomogeneityTable.unselectTopLeftCell(t, previousSelectedCellX, previousSelectedCellY); }

private:
  int indexOfSignificanceCell() const override { return k_indexOfTableCell + 1; }
  EditableCategoricalTableCell * categoricalTableCell() override { return &m_inputHomogeneityTable; }

  InputHomogeneityTableCell m_inputHomogeneityTable;
};

}  // namespace Probability

#endif /* PROBABILITY_CONTROLLERS_INPUT_HOMOGENEITY_CONTROLLER_H */
