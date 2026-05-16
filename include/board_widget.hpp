#ifndef BOARD_WIDGET_HPP
#define BOARD_WIDGET_HPP

#include "widgets.hpp"
#include <vector>
#include <functional>

enum CellState { EMPTY = 0, SHIP = 1, MISS = 2, HIT = 3, SUNK = 4 };

class BoardWidget : public Widget {
    int _rows, _cols;
    int _cell_size;
    std::vector<std::vector<CellState>> _grid;
    bool _hidden_ships; // If true, SHIP state is drawn as EMPTY (for enemy board)
    std::function<void(int, int)> _on_click;

public:
    BoardWidget(int x, int y, int cell_size, int rows, int cols, bool hidden_ships, std::function<void(int, int)> on_click);
    
    virtual void draw() const override;
    virtual void handle(genv::event ev) override;
    
    void set_cell(int r, int c, CellState state);
    CellState get_cell(int r, int c) const;
    void reset();
    void set_hidden_ships(bool hidden) { _hidden_ships = hidden; }
};

#endif
