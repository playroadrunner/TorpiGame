#include "list.hpp"
#include "graphics.hpp"
#include <algorithm>

using namespace genv;

List::List(int x, int y, int sx, int sy, std::vector<std::string> items)
    : Widget(x, y, sx, sy), _items(items), _selected_index(-1), _scroll_offset(0) {
    _item_height = 40; // Slightly more height
}

void List::draw() const {
    gout << move_to(_x, _y) << color(30, 30, 30) << box(_size_x, _size_y);
    gout << move_to(_x+1, _y+1) << color(60, 60, 60) << box(_size_x-2, _size_y-2);
    gout << move_to(_x+2, _y+2) << color(45, 45, 45) << box(_size_x-4, _size_y-4);
    
    int visible_count = _size_y / _item_height;
    for (int i = 0; i < visible_count && (i + _scroll_offset) < (int)_items.size(); ++i) {
        int idx = i + _scroll_offset;
        int iy = _y + i * _item_height;
        
        // Sor teljes hátterét mindig töröljük (megakadályozza a régi számjegyek maradását)
        gout << move_to(_x + 4, iy + 2) << color(45, 45, 45) << box(_size_x - 8, _item_height - 4);

        if (idx == _selected_index) {
            gout << move_to(_x + 4, iy + 4) << color(80, 80, 180) << box(_size_x - 8, _item_height - 8);
        }

        int text_h = gout.cascent() + gout.cdescent();
        int text_y = iy + (_item_height - text_h) / 2;
        gout << move_to(_x + 15, text_y) << color(220, 220, 220) << text(_items[idx]);
    }
    
    // Scrollbar indicator
    if (_items.size() > (size_t)visible_count) {
        int bar_h = _size_y * visible_count / _items.size();
        int bar_y = _y + (_scroll_offset * _size_y / _items.size());
        gout << move_to(_x + _size_x - 10, bar_y) << color(100, 100, 100) << box(6, bar_h);
    }
}

void List::handle(genv::event ev) {
    if (ev.type == ev_mouse) {
        if (ev.button == btn_left) {
            int relative_y = ev.pos_y - _y;
            int clicked_idx = relative_y / _item_height + _scroll_offset;
            if (clicked_idx >= 0 && clicked_idx < (int)_items.size()) {
                _selected_index = clicked_idx;
            }
        } else if (ev.button == btn_wheelup) {
            _scroll_offset = std::max(0, _scroll_offset - 1);
        } else if (ev.button == btn_wheeldown) {
            int visible_count = _size_y / _item_height;
            if ((int)_items.size() > visible_count) {
                _scroll_offset = std::min((int)_items.size() - visible_count, _scroll_offset + 1);
            }
        }
    }
}

std::string List::get_selected_item() const {
    if (_selected_index >= 0 && _selected_index < (int)_items.size()) {
        return _items[_selected_index];
    }
    return "";
}

void List::add_item(std::string item) {
    _items.push_back(item);
}

void List::remove_item(int index) {
    if (index >= 0 && index < (int)_items.size()) {
        _items.erase(_items.begin() + index);
        if (_selected_index == index) _selected_index = -1;
        else if (_selected_index > index) _selected_index--;
    }
}

void List::set_items(std::vector<std::string> items) {
    _items = items;
    _selected_index = _items.empty() ? -1 : 0;  // always select first
    _scroll_offset = 0;
}
