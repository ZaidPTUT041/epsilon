#ifndef ESCHER_NESTED_MENU_CONTROLLER_H
#define ESCHER_NESTED_MENU_CONTROLLER_H

#include <escher/highlight_cell.h>
#include <escher/list_view_data_source.h>
#include <escher/selectable_table_view.h>
#include <escher/stack_view_controller.h>

class NestedMenuController : public StackViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  NestedMenuController(Responder * parentResponder, const char * title = 0);
  void setSender(Responder * sender) { m_sender = sender; }

  // StackViewController
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;

  //ListViewDataSource
  virtual KDCoordinate rowHeight(int j) override;
  HighlightCell * reusableCell(int index, int type) override;
protected:
  class Stack {
  public:
    class State {
    public:
      State(int selectedRow = -1, KDCoordinate verticalScroll = 0);
      bool isNull();
      int selectedRow() { return m_selectedRow; }
      KDCoordinate verticalScroll() { return m_verticalScroll; }
    private:
      int m_selectedRow;
      KDCoordinate m_verticalScroll;
    };
    void push(int selectedRow, KDCoordinate verticalScroll);
    State * stateAtIndex(int index);
    State pop();
    int depth();
    void resetStack();
  private:
    constexpr static int k_maxModelTreeDepth = 3;
    State m_statesStack[k_maxModelTreeDepth];
  };

  class ListController : public ViewController {
  public:
    ListController(Responder * parentResponder, SelectableTableView * tableView, const char * title);
    const char * title() override;
    View * view() override;
    void didBecomeFirstResponder() override;
    void setFirstSelectedRow(int firstSelectedRow);
  private:
    SelectableTableView * m_selectableTableView;
    int m_firstSelectedRow;
    const char * m_title;
  };

  static constexpr int LeafCellType = 0;
  static constexpr int NodeCellType = 1;
  int stackDepth();
  bool handleEventForRow(Ion::Events::Event event, int selectedRow);
  virtual bool selectSubMenu(int selectedRow);
  virtual bool returnToPreviousMenu();
  virtual bool selectLeaf(int selectedRow) = 0;
  Responder * sender() { return m_sender; }
  virtual HighlightCell * leafCellAtIndex(int index) = 0;
  virtual HighlightCell * nodeCellAtIndex(int index) = 0;
  SelectableTableView m_selectableTableView;
  Stack m_stack;
  ListController m_listController;
private:
  Responder * m_sender;
};

#endif
