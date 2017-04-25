#include "editable_cell_table_view_controller.h"
#include "../apps_container.h"
#include "../constant.h"
#include "text_field_delegate_app.h"
#include <assert.h>

using namespace Poincare;

namespace Shared {

EditableCellTableViewController::EditableCellTableViewController(Responder * parentResponder, KDCoordinate topMargin,
  KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin) :
  TabTableController(parentResponder, this, topMargin, rightMargin, bottomMargin, leftMargin, this, true)
{
}

bool EditableCellTableViewController::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return TextFieldDelegate::textFieldShouldFinishEditing(textField, event)
     || (event == Ion::Events::Down && selectableTableView()->selectedRow() < numberOfRows()-1)
     || (event == Ion::Events::Up && selectableTableView()->selectedRow() > 0)
     || (event == Ion::Events::Right && selectableTableView()->selectedColumn() < numberOfColumns()-1)
     || (event == Ion::Events::Left && selectableTableView()->selectedColumn() > 0);  }

bool EditableCellTableViewController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  AppsContainer * appsContainer = ((TextFieldDelegateApp *)app())->container();
  Context * globalContext = appsContainer->globalContext();
  float floatBody = Expression::approximate(text, *globalContext);
  if (isnan(floatBody) || isinf(floatBody)) {
    app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }
  if (!setDataAtLocation(floatBody, selectableTableView()->selectedColumn(), selectableTableView()->selectedRow())) {
    app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  selectableTableView()->reloadData();
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    selectableTableView()->selectCellAtLocation(selectableTableView()->selectedColumn(), selectableTableView()->selectedRow()+1);
  } else {
    selectableTableView()->handleEvent(event);
  }
  return true;
}

void EditableCellTableViewController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  if (previousSelectedCellX == t->selectedColumn() && previousSelectedCellY == t->selectedRow()) {
    return;
  }
  if (cellAtLocationIsEditable(previousSelectedCellX, previousSelectedCellY)) {
    EvenOddEditableTextCell * myCell = (EvenOddEditableTextCell *)t->cellAtLocation(previousSelectedCellX, previousSelectedCellY);
    myCell->setEditing(false);
    if (app()->firstResponder() == myCell->editableTextCell()->textField()) {
      app()->setFirstResponder(t);
    }
  }
  if (cellAtLocationIsEditable(t->selectedColumn(), t->selectedRow())) {
    EvenOddEditableTextCell * myCell = (EvenOddEditableTextCell *)t->selectedCell();
    app()->setFirstResponder(myCell);
  }
}

int EditableCellTableViewController::numberOfRows() {
  int numberOfModelElements = numberOfElements();
  if (numberOfModelElements >= maxNumberOfElements()) {
    return 1 + numberOfModelElements;
  }
  return 2 + numberOfModelElements;
}

KDCoordinate EditableCellTableViewController::rowHeight(int j) {
  return k_cellHeight;
}

void EditableCellTableViewController::willDisplayCellAtLocationWithDisplayMode(HighlightCell * cell, int i, int j, Expression::FloatDisplayMode floatDisplayMode) {
  EvenOddCell * myCell = (EvenOddCell *)cell;
  /* We set the cell even or odd state only if the cell is not being edited.
   * Otherwise, the cell background is white whichever it is an odd or even cell
   * and we do not want to redraw the cell twice (in the even/odd color and
   * then in white) to avoid screen blinking. */
  // The cell is editable
  if (cellAtLocationIsEditable(i, j)) {
    EvenOddEditableTextCell * myEditableValueCell = (EvenOddEditableTextCell *)cell;
    char buffer[Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
    // Special case 1: last row
    if (j == numberOfRows() - 1) {
      /* Display an empty line only if there is enough space for a new element in
       * data */
      if (numberOfElements() < maxNumberOfElements() && !myEditableValueCell->isEditing()) {
        myCell->setEven(j%2 == 0);
        buffer[0] = 0;
        myEditableValueCell->setText(buffer);
        return;
      }
    }
    if (!myEditableValueCell->isEditing()) {
      myCell->setEven(j%2 == 0);
      Complex::convertFloatToText(dataAtLocation(i, j), buffer, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, floatDisplayMode);
      myEditableValueCell->setText(buffer);
    }
    return;
  } else {
    myCell->setEven(j%2 == 0);
  }
}

void EditableCellTableViewController::didBecomeFirstResponder() {
  if (selectableTableView()->selectedRow() >= 0) {
    int selectedRow = selectableTableView()->selectedRow();
    selectedRow = selectedRow >= numberOfRows() ? numberOfRows()-1 : selectedRow;
    int selectedColumn = selectableTableView()->selectedColumn();
    selectedColumn = selectedColumn >= numberOfColumns() ? numberOfColumns() - 1 : selectedColumn;
    selectableTableView()->selectCellAtLocation(selectedColumn, selectedRow);
    TabTableController::didBecomeFirstResponder();
  }
}

void EditableCellTableViewController::viewWillAppear() {
  TabTableController::viewWillAppear();
  if (selectableTableView()->selectedRow() == -1) {
    selectableTableView()->selectCellAtLocation(0, 1);
  } else {
    int selectedRow = selectableTableView()->selectedRow();
    selectedRow = selectedRow >= numberOfRows() ? numberOfRows()-1 : selectedRow;
    int selectedColumn = selectableTableView()->selectedColumn();
    selectedColumn = selectedColumn >= numberOfColumns() ? numberOfColumns() - 1 : selectedColumn;
    selectableTableView()->selectCellAtLocation(selectedColumn, selectedRow);
  }
}

TextFieldDelegateApp * EditableCellTableViewController::textFieldDelegateApp() {
  return (TextFieldDelegateApp *)app();
}

}
