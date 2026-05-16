#include "board_widget.hpp"
#include "graphics.hpp"

using namespace genv;

BoardWidget::BoardWidget(int x, int y, int cell_size, int rows, int cols, bool hidden_ships, std::function<void(int, int)> on_click)
    : Widget(x, y, cols * cell_size, rows * cell_size), 
      _rows(rows), _cols(cols), _cell_size(cell_size), 
      _grid(rows, std::vector<CellState>(cols, EMPTY)),
      _hidden_ships(hidden_ships), _on_click(on_click) {
}

void BoardWidget::draw() const {
    for (int r = 0; r < _rows; ++r) {
        for (int c = 0; c < _cols; ++c) {
            int cx = _x + c * _cell_size;
            int cy = _y + r * _cell_size;
            
            // Draw cell border
            gout << move_to(cx, cy) << color(200, 200, 200) << box(_cell_size, _cell_size);
            gout << move_to(cx + 1, cy + 1) << color(30, 30, 40) << box(_cell_size - 2, _cell_size - 2); // Inner bg
            
            CellState state = _grid[r][c];
            if (state == SHIP && _hidden_ships) {
                state = EMPTY; // Hide unhit ships
            }
            
            // Draw state
            if (state == SHIP) {
                gout << move_to(cx + 2, cy + 2) << color(100, 100, 100) << box(_cell_size - 4, _cell_size - 4);
            } else if (state == MISS) {
                gout << move_to(cx + _cell_size / 2 - 2, cy + _cell_size / 2 - 2) << color(200, 200, 200) << box(4, 4);
            } else if (state == HIT) {
                gout << move_to(cx + 2, cy + 2) << color(200, 50, 50) << box(_cell_size - 4, _cell_size - 4);
                // Draw X
                gout << move_to(cx + 4, cy + 4) << color(0, 0, 0) << line_to(cx + _cell_size - 4, cy + _cell_size - 4);
                gout << move_to(cx + _cell_size - 4, cy + 4) << color(0, 0, 0) << line_to(cx + 4, cy + _cell_size - 4);
            } else if (state == SUNK) {
                gout << move_to(cx + 2, cy + 2) << color(100, 0, 0) << box(_cell_size - 4, _cell_size - 4);
            }
        }
    }
}

void BoardWidget::handle(genv::event ev) {
    if (ev.type == ev_mouse && ev.button == btn_left) {
        if (is_selected(ev.pos_x, ev.pos_y)) {
            int c = (ev.pos_x - _x) / _cell_size;
            int r = (ev.pos_y - _y) / _cell_size;
            if (r >= 0 && r < _rows && c >= 0 && c < _cols) {
                _on_click(r, c);
            }
        }
    }
}

void BoardWidget::set_cell(int r, int c, CellState state) {
    if (r >= 0 && r < _rows && c >= 0 && c < _cols) {
        _grid[r][c] = state;
    }
}

CellState BoardWidget::get_cell(int r, int c) const {
    if (r >= 0 && r < _rows && c >= 0 && c < _cols) {
        return _grid[r][c];
    }
    return EMPTY;
}

void BoardWidget::reset() {
    for (int r = 0; r < _rows; ++r) {
        for (int c = 0; c < _cols; ++c) {
            _grid[r][c] = EMPTY;
        }
    }
}
