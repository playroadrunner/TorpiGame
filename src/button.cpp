#include "button.hpp"
#include "graphics.hpp"

using namespace genv;

Button::Button(int x, int y, int sx, int sy, std::string label, std::function<void()> action)
    : Widget(x, y, sx, sy), _label(label), _action(action), _pressed(false) {
}

void Button::draw() const {
    // Shadow
    gout << move_to(_x + 3, _y + 3) << color(10, 10, 10) << box(_size_x, _size_y);
    
    if (_pressed) {
        gout << move_to(_x + 1, _y + 1) << color(90, 90, 190) << box(_size_x, _size_y);
    } else {
        gout << move_to(_x, _y) << color(70, 70, 170) << box(_size_x, _size_y);
    }
    
    // Glossy top highlight
    gout << move_to(_x, _y) << color(120, 120, 255) << box(_size_x, 2);
    
    // Text centering (top-based drawing)
    int text_h = gout.cascent() + gout.cdescent();
    int tx = _x + (_size_x - gout.twidth(_label)) / 2;
    int ty = _y + (_size_y - text_h) / 2;
    
    gout << move_to(tx, ty) << color(255, 255, 255) << text(_label);
}

void Button::handle(genv::event ev) {
    if (ev.type == ev_mouse) {
        if (ev.button == btn_left) {
            if (is_selected(ev.pos_x, ev.pos_y)) {
                _pressed = true;
            }
        } else if (ev.button == -btn_left) {
            if (_pressed && is_selected(ev.pos_x, ev.pos_y)) {
                _action();
            }
            _pressed = false;
        }
    }
}
