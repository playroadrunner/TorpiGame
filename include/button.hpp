#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "widgets.hpp"
#include <string>
#include <functional>

class Button : public Widget {
    std::string _label;
    std::function<void()> _action;
    bool _pressed;
public:
    Button(int x, int y, int sx, int sy, std::string label, std::function<void()> action);
    virtual void draw() const override;
    virtual void handle(genv::event ev) override;
};

#endif
