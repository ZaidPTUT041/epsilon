#include "store_controller.h"
#include "app.h"
#include "../apps_container.h"
#include "../constant.h"
#include <assert.h>

using namespace Shared;

namespace Statistics {

StoreController::StoreController(Responder * parentResponder, Store * store, ButtonRowController * header) :
  Shared::StoreController(parentResponder, store, header)
{
}

void StoreController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  ::StoreController::willDisplayCellAtLocation(cell, i, j);
  if (cellAtLocationIsEditable(i, j)) {
    return;
  }
  EvenOddMessageTextCell * mytitleCell = (EvenOddMessageTextCell *)cell;
  if (i == 0) {
    mytitleCell->setMessage(I18n::Message::Values);
    return;
  }
  mytitleCell->setMessage(I18n::Message::Sizes);
}

void StoreController::unloadView() {
  for (int i = 0; i < k_numberOfTitleCells; i++) {
    assert(m_titleCells[i] != nullptr);
    delete m_titleCells[i];
    m_titleCells[i] = nullptr;
  }
  Shared::StoreController::unloadView();
}

HighlightCell * StoreController::titleCells(int index) {
  assert(index >= 0 && index < k_numberOfTitleCells);
  return m_titleCells[index];
}

bool StoreController::setDataAtLocation(float floatBody, int columnIndex, int rowIndex) {
  if (columnIndex == 1) {
    if (floatBody < 0) {
      return false;
    }
    m_store->set(roundf(floatBody), columnIndex, rowIndex-1);
    return true;
  }
  return Shared::StoreController::setDataAtLocation(floatBody, columnIndex, rowIndex);
}

View * StoreController::createView() {
  for (int i = 0; i < k_numberOfTitleCells; i++) {
    assert(m_titleCells[i] == nullptr);
    m_titleCells[i] = new EvenOddMessageTextCell(KDText::FontSize::Small);
  }
  return Shared::StoreController::createView();
}

}
