#ifndef STATISTICS_MULTIPLE_BOXES_VIEW_H
#define STATISTICS_MULTIPLE_BOXES_VIEW_H

#include <escher.h>
#include "store.h"
#include "box_view.h"
#include "box_banner_view.h"
#include "multiple_data_view.h"

namespace Statistics {

class MultipleBoxesView : public MultipleDataView {
public:
  MultipleBoxesView(Store * store, BoxView::Quantile * selectedQuantile);
  // MultipleDataView
  int seriesOfSubviewAtIndex(int index) override;
  const BoxBannerView * bannerView() const override { return &m_bannerView; }
  BoxView * dataViewAtIndex(int index) override;
  void layoutDataSubviews() override;
private:
  BoxView m_boxView1;
  BoxView m_boxView2;
  BoxView m_boxView3;
  BoxBannerView m_bannerView;
};

}

#endif
