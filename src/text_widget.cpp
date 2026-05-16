#include "text_widget.hpp"
#include "graphics.hpp"

using namespace genv;

TextWidget::TextWidget(int x, int y, std::string text, int r, int g, int b)
    : Widget(x, y, gout.twidth(text), gout.cascent() + gout.cdescent()), 
      _text(text), _r(r), _g(g), _b(b) {
}

void TextWidget::draw() const {
    gout << move_to(_x, _y) << color(_r, _g, _b) << text(_text);
}

void TextWidget::handle(genv::event ev) {
    // text widget does not handle events
}
