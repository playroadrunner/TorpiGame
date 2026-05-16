#include "jatekmester.hpp"
#include "graphics.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace genv;

JatekMester::JatekMester(int width, int height) : _width(width), _height(height) {
    std::srand(std::time(nullptr));
    
    int board_size = 300;
    int cell_size = board_size / 10;
    
    int p_x = 50;
    int p_y = 100;
    
    int e_x = _width - board_size - 50;
    int e_y = 100;
    
    _player_board = new BoardWidget(p_x, p_y, cell_size, 10, 10, false, [this](int r, int c) { this->on_player_board_click(r, c); });
    _enemy_board = new BoardWidget(e_x, e_y, cell_size, 10, 10, true, [this](int r, int c) { this->on_enemy_board_click(r, c); });
    
    _action_btn = new Button(_width / 2 - 75, _height - 60, 150, 40, "Forgat (Irany)", [this]() {
        if (_state == STATE_PLACEMENT) {
            _horizontal_placement = !_horizontal_placement;
            _action_btn = new Button(_width / 2 - 75, _height - 60, 150, 40, 
                                     _horizontal_placement ? "Forgat (Vízszintes)" : "Forgat (Függőleges)", 
                                     [this]() { 
                                         _horizontal_placement = !_horizontal_placement; 
                                     }); // Small hack, simpler to just redraw text, but Button doesn't have set_label.
        }
    }); // We'll handle button label changes differently
    
    _widgets.push_back(_player_board);
    _widgets.push_back(_enemy_board);
    _widgets.push_back(_action_btn);
    
    _ships_to_place = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    _horizontal_placement = true;
    _state = STATE_PLACEMENT;
    _status_msg = "Helyezd el a hajoikat! Kattints a tabladra.";
    
    place_enemy_ships();
}

JatekMester::~JatekMester() {
    for (Widget * w : _widgets) delete w;
}

bool JatekMester::can_place_ship(const std::vector<Ship>& ships, int r, int c, int length, bool horizontal) const {
    if (horizontal) {
        if (c + length > 10) return false;
    } else {
        if (r + length > 10) return false;
    }
    
    // Check overlap and adjacency (1 cell margin)
    for (int i = -1; i <= length; ++i) {
        for (int j = -1; j <= 1; ++j) {
            int check_r = horizontal ? r + j : r + i;
            int check_c = horizontal ? c + i : c + j;
            
            if (check_r >= 0 && check_r < 10 && check_c >= 0 && check_c < 10) {
                // if any existing ship occupies this
                for (const auto& s : ships) {
                    for (int k = 0; k < s.length; ++k) {
                        int sr = s.horizontal ? s.r : s.r + k;
                        int sc = s.horizontal ? s.c + k : s.c;
                        if (check_r == sr && check_c == sc) return false;
                    }
                }
            }
        }
    }
    return true;
}

void JatekMester::add_ship(std::vector<Ship>& ships, BoardWidget* board, int r, int c, int length, bool horizontal) {
    ships.push_back({r, c, length, horizontal, 0, false});
    for (int i = 0; i < length; ++i) {
        if (horizontal) board->set_cell(r, c + i, SHIP);
        else board->set_cell(r + i, c, SHIP);
    }
}

void JatekMester::place_enemy_ships() {
    std::vector<int> ships_to_place = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    for (int len : ships_to_place) {
        bool placed = false;
        while (!placed) {
            int r = std::rand() % 10;
            int c = std::rand() % 10;
            bool horiz = (std::rand() % 2) == 0;
            
            if (can_place_ship(_enemy_ships, r, c, len, horiz)) {
                add_ship(_enemy_ships, _enemy_board, r, c, len, horiz);
                placed = true;
            }
        }
    }
}

void JatekMester::on_player_board_click(int r, int c) {
    if (_state == STATE_PLACEMENT) {
        if (_ships_to_place.empty()) return;
        
        int len = _ships_to_place.front();
        if (can_place_ship(_player_ships, r, c, len, _horizontal_placement)) {
            add_ship(_player_ships, _player_board, r, c, len, _horizontal_placement);
            _ships_to_place.erase(_ships_to_place.begin());
            
            if (_ships_to_place.empty()) {
                _state = STATE_PLAYER_TURN;
                _status_msg = "Te jossz! Kattints az ellenseg tablajara.";
            } else {
                _status_msg = "Kovetkezo hajo hossza: " + std::to_string(_ships_to_place.front());
            }
        }
    }
}

void JatekMester::on_enemy_board_click(int r, int c) {
    if (_state == STATE_PLAYER_TURN) {
        CellState state = _enemy_board->get_cell(r, c);
        if (state == EMPTY || state == SHIP) {
            if (state == SHIP) {
                _enemy_board->set_cell(r, c, HIT);
                update_sunk_ships(_enemy_ships, _enemy_board);
                if (check_win(_enemy_ships)) {
                    _state = STATE_GAME_OVER;
                    _status_msg = "GYOZTEM!";
                } else {
                    _status_msg = "Talalat! Te jossz ujra.";
                }
            } else {
                _enemy_board->set_cell(r, c, MISS);
                _state = STATE_ENEMY_TURN;
                _status_msg = "Melle. Az ellenseg jon.";
            }
        }
    }
}

void JatekMester::update_sunk_ships(std::vector<Ship>& ships, BoardWidget* board) {
    for (auto& s : ships) {
        if (!s.sunk) {
            bool all_hit = true;
            for (int i = 0; i < s.length; ++i) {
                int sr = s.horizontal ? s.r : s.r + i;
                int sc = s.horizontal ? s.c + i : s.c;
                if (board->get_cell(sr, sc) != HIT) {
                    all_hit = false;
                    break;
                }
            }
            if (all_hit) {
                s.sunk = true;
                for (int i = 0; i < s.length; ++i) {
                    int sr = s.horizontal ? s.r : s.r + i;
                    int sc = s.horizontal ? s.c + i : s.c;
                    board->set_cell(sr, sc, SUNK);
                }
            }
        }
    }
}

bool JatekMester::check_win(const std::vector<Ship>& ships) const {
    for (const auto& s : ships) {
        if (!s.sunk) return false;
    }
    return true;
}

void JatekMester::enemy_shoot() {
    // Simple random shooting
    bool shot = false;
    while (!shot) {
        int r = std::rand() % 10;
        int c = std::rand() % 10;
        CellState state = _player_board->get_cell(r, c);
        
        if (state == EMPTY || state == SHIP) {
            if (state == SHIP) {
                _player_board->set_cell(r, c, HIT);
                update_sunk_ships(_player_ships, _player_board);
                if (check_win(_player_ships)) {
                    _state = STATE_GAME_OVER;
                    _status_msg = "VESZTETTEL!";
                }
                // Enemy gets another turn
            } else {
                _player_board->set_cell(r, c, MISS);
                if (_state != STATE_GAME_OVER) {
                    _state = STATE_PLAYER_TURN;
                    _status_msg = "Te jossz! Kattints az ellenseg tablajara.";
                }
            }
            shot = true;
        }
    }
}

void JatekMester::run() {
    event ev;
    int focus = -1;
    
    // Very simple timer for enemy turn
    int enemy_delay = 0;

    while (gin >> ev && ev.keycode != key_escape) {
        if (ev.type == ev_timer) {
            if (_state == STATE_ENEMY_TURN) {
                enemy_delay++;
                if (enemy_delay > 20) { // arbitrary delay
                    enemy_shoot();
                    enemy_delay = 0;
                }
            }
        }
    
        if (ev.type == ev_mouse && ev.button == btn_left) {
            focus = -1;
            for (size_t i = 0; i < _widgets.size(); i++) {
                if (_widgets[i]->is_selected(ev.pos_x, ev.pos_y)) {
                    focus = i;
                }
            }
        }
        
        if (ev.type == ev_mouse && ev.button == btn_right && _state == STATE_PLACEMENT) {
            _horizontal_placement = !_horizontal_placement; // right click to rotate
        }
        
        if (focus != -1) {
            _widgets[focus]->handle(ev);
        }

        gout << color(20, 20, 25) << move_to(0, 0) << box(_width, _height);
        
        gout << move_to(50, 70) << color(180, 180, 200) << text("Sajat flotta");
        gout << move_to(_width - 350, 70) << color(180, 180, 200) << text("Ellenseges flotta");
        
        gout << move_to(50, 20) << color(255, 255, 0) << text(_status_msg);
        
        if (_state == STATE_PLACEMENT) {
            std::string dir = _horizontal_placement ? "Vizszintes" : "Fuggoleges";
            gout << move_to(50, 40) << color(200, 200, 200) << text("Irany (Jobb klikk): " + dir);
        }

        for (Widget * w : _widgets) {
            w->draw();
        }
        gout << refresh;
    }
}
