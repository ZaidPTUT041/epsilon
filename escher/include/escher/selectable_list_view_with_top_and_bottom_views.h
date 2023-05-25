#ifndef ESCHER_LIST_VIEW_WITH_TOP_AND_BOTTOM_VIEW_H
#define ESCHER_LIST_VIEW_WITH_TOP_AND_BOTTOM_VIEW_H

#include <escher/selectable_list_view_controller.h>

namespace Escher {

class ListViewWithTopAndBottomViewsDataSource
    : public StandardMemoizedListViewDataSource {
 public:
  ListViewWithTopAndBottomViewsDataSource(ListViewDataSource* dataSource,
                                          View* topView, View* bottomView)
      : m_innerDataSource(dataSource),
        m_topCell(topView),
        m_bottomCell(bottomView) {}

  int numberOfRows() const override;
  KDCoordinate separatorBeforeRow(int index) override;
  HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;

  bool hasTopView() const { return m_topCell.hasView(); }
  bool hasBottomView() const { return m_bottomCell.hasView(); }
  void setTopView(View* view) { m_topCell.setView(view); }
  void setBottomView(View* view) { m_bottomCell.setView(view); }

 protected:
  KDCoordinate nonMemoizedRowHeight(int j) override;
  void willDisplayCellForIndex(HighlightCell* cell, int index) override;
  int typeAtIndex(int index) const override;

 private:
  constexpr static int k_topCellType = 0;
  constexpr static int k_bottomCellType = 1;
  constexpr static int k_cellTypeOffset = 2;

  class ViewCell : public HighlightCell {
   public:
    ViewCell(View* view) : HighlightCell(), m_view(view) {}
    KDSize minimalSizeForOptimalDisplay() const override {
      assert(m_view);
      return m_view->minimalSizeForOptimalDisplay();
    }
    bool hasView() const { return m_view; }
    void setView(View* view) { m_view = view; }

   protected:
    int numberOfSubviews() const override { return 1; }
    View* subviewAtIndex(int index) override {
      assert(m_view);
      return m_view;
    }
    void layoutSubviews(bool force) override {
      assert(m_view);
      setChildFrame(m_view, bounds(), force);
    }
    bool protectedIsSelectable() override { return false; }

   private:
    View* m_view;
  };

  ListViewDataSource* m_innerDataSource;
  ViewCell m_topCell;
  ViewCell m_bottomCell;
};

class SelectableListViewWithTopAndBottomViews
    : public SelectableViewController,
      public ListViewDataSource,
      public SelectableListViewDelegate {
 public:
  SelectableListViewWithTopAndBottomViews(Responder* parentResponder,
                                          View* topView = nullptr,
                                          View* bottomView = nullptr);

  View* view() override { return &m_selectableListView; }
  void resetMemoization(bool force = true) override {
    return m_outerDataSource.resetMemoization(force);
  }
  void listViewDidChangeSelectionAndDidScroll(
      SelectableListView* l, int previousRow, KDPoint previousOffset,
      bool withinTemporarySelection = false) override;

 protected:
  constexpr static KDGlyph::Format k_messageFormat = {
      .style = {.glyphColor = Escher::Palette::GrayDark,
                .backgroundColor = Escher::Palette::WallScreen,
                .font = KDFont::Size::Small},
      .horizontalAlignment = KDGlyph::k_alignCenter};

  void didBecomeFirstResponder() override;
  int innerRowFromRow(int row) const {
    assert(row >= m_outerDataSource.hasTopView());
    return row - m_outerDataSource.hasTopView();
  }
  int innerSelectedRow() const { return innerRowFromRow(selectedRow()); }
  void selectFirstCell() { selectCell(m_outerDataSource.hasTopView()); }
  void selectLastCell() {
    selectCell(m_outerDataSource.numberOfRows() - 1 -
               m_outerDataSource.hasBottomView());
  }
  void setTopView(View* view) { m_outerDataSource.setTopView(view); }
  void setBottomView(View* view) { m_outerDataSource.setBottomView(view); }

  SelectableListView m_selectableListView;

 private:
  ListViewWithTopAndBottomViewsDataSource m_outerDataSource;
};

}  // namespace Escher

#endif