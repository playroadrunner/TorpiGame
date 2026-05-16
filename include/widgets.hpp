#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include "graphics.hpp"

class Widget {
protected:
    int _x, _y, _size_x, _size_y;
    bool _focused;
public:
    Widget(int x, int y, int sx, int sy);
    virtual ~Widget() {}
    virtual bool is_selected(int mouse_x, int mouse_y);
    virtual void draw() const = 0;
    virtual void handle(genv::event ev) = 0;
    virtual void set_focus(bool focused) { _focused = focused; }
    virtual bool is_focused() const { return _focused; }
    
    // Getters for responsive layout positioning
    int x() const { return _x; }
    int y() const { return _y; }
    int size_x() const { return _size_x; }
    int size_y() const { return _size_y; }
};

#endif
