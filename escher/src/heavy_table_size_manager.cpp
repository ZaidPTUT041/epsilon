#include <assert.h>
#include <escher/heavy_table_size_manager.h>

#include <algorithm>

namespace Escher {

void AbstractHeavyTableSizeManager::rowDidChange(int row) {
  assert(row >= 0 && row < maxNumberOfRows());
  int nC = maxNumberOfColumns();
  KDSize maxCellSize = m_delegate->maxCellSize();
  for (int c = 0; c < nC; c++) {
    KDSize cellSize = m_delegate->cellSizeAtLocation(row, c);
    *memoizedRowHeight(row) = std::clamp(
        *memoizedRowHeight(row), cellSize.height(), maxCellSize.height());
  }
}

void AbstractHeavyTableSizeManager::columnDidChange(int column) {
  assert(column >= 0 && column < maxNumberOfColumns());
  int nR = maxNumberOfRows();
  KDSize maxCellSize = m_delegate->maxCellSize();
  for (int r = 0; r < nR; r++) {
    KDSize cellSize = m_delegate->cellSizeAtLocation(r, column);
    *memoizedColumnWidth(column) = std::clamp(
        *memoizedColumnWidth(column), cellSize.width(), maxCellSize.width());
  }
}

void AbstractHeavyTableSizeManager::deleteRowMemoization(int row) {
  int nR = maxNumberOfRows();
  assert(row >= 0 && row < nR);
  for (int r = row; r < nR - 1; r++) {
    *memoizedRowHeight(r) =
        std::max(*memoizedRowHeight(r), *memoizedRowHeight(r + 1));
  }
  *memoizedRowHeight(nR - 1) = TableSize1DManager::k_undefinedSize;
}

KDCoordinate AbstractHeavyTableSizeManager::computeSizeAtIndex(
    int i, Dimension dimension) {
  KDCoordinate returnValue = memoizedSizeAtIndex(i, dimension);
  if (returnValue == TableSize1DManager::k_undefinedSize) {
    computeAllSizes();
    returnValue = memoizedSizeAtIndex(i, dimension);
    assert(returnValue != TableSize1DManager::k_undefinedSize);
  }
  return returnValue;
}

void AbstractHeavyTableSizeManager::resetAllSizes() {
  int nR = maxNumberOfRows();
  for (int i = 0; i < nR; i++) {
    *memoizedRowHeight(i) = TableSize1DManager::k_undefinedSize;
  }
  int nC = maxNumberOfColumns();
  for (int i = 0; i < nC; i++) {
    *memoizedColumnWidth(i) = TableSize1DManager::k_undefinedSize;
  }
}

void AbstractHeavyTableSizeManager::computeAllSizes() {
  int nR = maxNumberOfRows();
  int nC = maxNumberOfColumns();
  KDSize maxCellSize = m_delegate->maxCellSize();
  for (int c = 0; c < nC; c++) {
    for (int r = 0; r < nR; r++) {
      KDSize cellSize = m_delegate->cellSizeAtLocation(r, c);
      *memoizedRowHeight(r) = std::clamp(
          *memoizedRowHeight(r), cellSize.height(), maxCellSize.height());
      *memoizedColumnWidth(c) = std::clamp(
          *memoizedColumnWidth(c), cellSize.width(), maxCellSize.width());
    }
  }
}

}  // namespace Escher
