#ifndef TEXT_WIDGET_HPP
#define TEXT_WIDGET_HPP

#include "widgets.hpp"
#include <string>

class TextWidget : public Widget {
    std::string _text;
    int _r, _g, _b;
public:
    TextWidget(int x, int y, std::string text, int r = 200, int g = 200, int b = 200);
    virtual void draw() const override;
    virtual void handle(genv::event ev) override;
    
    void set_text(const std::string& text) { _text = text; }
    void set_color(int r, int g, int b) { _r = r; _g = g; _b = b; }
};

#endif
