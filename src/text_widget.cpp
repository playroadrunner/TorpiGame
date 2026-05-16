#include "text_widget.hpp"
#include "graphics.hpp"

using namespace genv;

TextWidget::TextWidget(int x, int y, std::string text, int r, int g, int b)
    : Widget(x, y, 500, gout.cascent() + gout.cdescent() + 2), 
      _text(text), _r(r), _g(g), _b(b) {
}

void TextWidget::draw() const {
    // clear bg strip to prevent text bleed
    gout << move_to(_x, _y) << color(20, 20, 25) << box(600, gout.cascent() + gout.cdescent() + 4);
    gout << move_to(_x, _y) << color(_r, _g, _b) << text(_text);
}

void TextWidget::handle(genv::event ev) {
    // text widget does not handle events
}
