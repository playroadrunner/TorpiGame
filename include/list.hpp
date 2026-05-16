#ifndef LIST_HPP
#define LIST_HPP

#include "widgets.hpp"
#include <vector>
#include <string>

class List : public Widget {
    std::vector<std::string> _items;
    int _selected_index;
    int _scroll_offset;
    int _item_height;
public:
    List(int x, int y, int sx, int sy, std::vector<std::string> items);
    virtual void draw() const override;
    virtual void handle(genv::event ev) override;
    
    int get_selected_index() const { return _selected_index; }
    std::string get_selected_item() const;
    void add_item(std::string item);
    void remove_item(int index);
    void set_items(std::vector<std::string> items);
};

#endif
