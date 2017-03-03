#ifndef SEQUENCE_GRAPH_VIEW_H
#define SEQUENCE_GRAPH_VIEW_H

#include "../../shared/function_graph_view.h"
#include "../sequence_store.h"

namespace Sequence {

class GraphView : public Shared::FunctionGraphView {
public:
  GraphView(SequenceStore * sequenceStore, Shared::InteractiveCurveViewRange * graphRange,
    Shared::CurveViewCursor * cursor, Shared::BannerView * bannerView, View * cursorView);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setVerticalCursor(bool verticalCursor);
private:
  float evaluateModelWithParameter(Model * expression, float abscissa) const override;
  KDSize cursorSize() override;
  SequenceStore * m_sequenceStore;
  bool m_verticalCursor;
};

}

#endif
