#include "graph_view.h"

using namespace Shared;

namespace Sequence {

GraphView::GraphView(SequenceStore * sequenceStore, InteractiveCurveViewRange * graphRange,
  CurveViewCursor * cursor, BannerView * bannerView, View * cursorView) :
  FunctionGraphView(graphRange, cursor, bannerView, cursorView),
  m_sequenceStore(sequenceStore),
  m_verticalCursor(false)
{
}

void GraphView::drawRect(KDContext * ctx, KDRect rect) const {
  FunctionGraphView::drawRect(ctx, rect);
  for (int i = 0; i < m_sequenceStore->numberOfActiveFunctions(); i++) {
    Sequence * s = m_sequenceStore->activeFunctionAtIndex(i);
    float rectMin = pixelToFloat(Axis::Horizontal, rect.left() - k_externRectMargin);
    float rectMax = pixelToFloat(Axis::Horizontal, rect.right() + k_externRectMargin);
    for (int x = rectMin; x < rectMax; x++) {
      float y = evaluateModelWithParameter(s, x);
      if (isnan(y)) {
        continue;
      }
      drawDot(ctx, rect, x, y, s->color());
    }
  }
}

void GraphView::setVerticalCursor(bool verticalCursor) {
  m_verticalCursor = verticalCursor;
}

float GraphView::evaluateModelWithParameter(Model * curve, float abscissa) const {
  Sequence * s = (Sequence *)curve;
  return s->evaluateAtAbscissa(abscissa, context());
}

KDSize GraphView::cursorSize() {
  if (m_verticalCursor) {
    return KDSize(1, 2*bounds().height());
  }
  return CurveView::cursorSize();
}

}
