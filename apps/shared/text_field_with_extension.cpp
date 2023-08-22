#include "text_field_with_extension.h"

namespace Shared {

void TextFieldWithExtension::willSetCursorLocation(const char **location) {
  size_t textLength = strlen(text());
  assert(textLength >= m_extensionLength);
  const char *maxLocation =
      m_contentView.text() + (textLength - m_extensionLength);
  if (*location > maxLocation) {
    *location = maxLocation;
  }
}

bool TextFieldWithExtension::privateRemoveEndOfLine() {
  return removeTextBeforeExtension(false);
}

void TextFieldWithExtension::removeWholeText() {
  removeTextBeforeExtension(true);
  scrollToCursor();
}

bool TextFieldWithExtension::removeTextBeforeExtension(bool whole) {
  assert(isEditing());
  const char *extension =
      m_contentView.text() + (strlen(text()) - m_extensionLength);
  assert(extension >= m_contentView.text() &&
         extension < m_contentView.text() +
                         (ContentView::k_maxBufferSize - m_extensionLength));
  char *destination =
      const_cast<char *>(whole ? m_contentView.text() : cursorLocation());
  if (destination == extension) {
    return false;
  }
  assert(destination >= m_contentView.text());
  assert(destination < extension);
  m_contentView.willModifyTextBuffer();
  strlcpy(destination, extension,
          ContentView::k_maxBufferSize - (destination - m_contentView.text()));
  m_contentView.setCursorLocation(destination);
  m_contentView.didModifyTextBuffer();
  layoutSubviews();
  return true;
}

}  // namespace Shared
