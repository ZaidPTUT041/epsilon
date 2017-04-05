#ifndef HOME_APP_CELL_H
#define HOME_APP_CELL_H

#include <escher.h>

namespace Home {

class AppCell : public HighlightCell {
public:
  AppCell();
  void drawRect(KDContext * ctx, KDRect rect) const override;

  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;

  void setVisible(bool visible);
  void reloadCell() override;
  void setApp(::App * app);
private:
  static constexpr KDCoordinate k_iconMargin = 18;
  static constexpr KDCoordinate k_iconWidth = 55;
  static constexpr KDCoordinate k_iconHeight = 56;
  static constexpr KDCoordinate k_nameWidthMargin = 4;
  static constexpr KDCoordinate k_nameHeightMargin = 2;
  ImageView m_iconView;
  MessageTextView m_nameView;
  bool m_visible;
};

}

#endif
