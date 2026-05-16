#include "jatekmester.hpp"
#include "graphics.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <map>

using namespace genv;

JatekMester::JatekMester(int width, int height) : _width(width), _height(height) {
    std::srand(std::time(nullptr));
    
    _state = STATE_MAIN_MENU;
    _pvp_mode = false;
    _status_msg = "";

    // ── Layout constants ───────────────────────────────────────────
    // Header bar: y 0..50  (title / status)
    // Sub-header: y 52..75 (labels, ships-left)
    // Score bar:  y 76..99
    // Board area: y 110..410  (300px boards)
    // Bottom bar: y 420..600  (shot list, buttons)

    const int BOARD_Y    = 110;
    const int BOARD_SIZE = 300;
    const int CELL       = BOARD_SIZE / 10;
    
    const int P1_X = 40;
    const int P2_X = _width - BOARD_SIZE - 40;
    
    _p1_board = new BoardWidget(P1_X, BOARD_Y, CELL, 10, 10, false,
        [this](int r, int c) { this->on_board_click(r, c, true); });
    _p2_board = new BoardWidget(P2_X, BOARD_Y, CELL, 10, 10, true,
        [this](int r, int c) { this->on_board_click(r, c, false); });

    // Gombok
    _action_btn = new Button(P1_X, _height - 55, 130, 38, "Forgat", [this]() {
        if (_state == STATE_P1_PLACEMENT || _state == STATE_P2_PLACEMENT)
            _horizontal_placement = !_horizontal_placement;
    });
    _start_bot_btn = new Button(_width/2 - 110, _height/2 - 55, 220, 48, "Jatek Bot Ellen", [this]() {
        this->start_game(false);
    });
    _start_pvp_btn = new Button(_width/2 - 110, _height/2 + 10, 220, 48, "Jatek Ember Ellen", [this]() {
        this->start_game(true);
    });
    _back_to_menu_btn = new Button(_width/2 - 110, _height - 75, 220, 45, "Vissza a Fomenube", [this]() {
        _state = STATE_MAIN_MENU;
        _status_msg = "";
        setup_state_widgets();
    });
    _pass_turn_btn = new Button(_width/2 - 110, _height/2 + 20, 220, 48, "Tovabb", [this]() {
        _state = _next_state;
        if (_state == STATE_P1_TURN) {
            _p1_board->set_hidden_ships(false);
            _p2_board->set_hidden_ships(true);
        } else if (_state == STATE_P2_TURN) {
            _p1_board->set_hidden_ships(true);
            _p2_board->set_hidden_ships(false);
        } else if (_state == STATE_P2_PLACEMENT) {
            _p1_board->set_hidden_ships(true);
            _p2_board->set_hidden_ships(false);
        }
        setup_state_widgets();
        update_shot_list();
    });

    // Szövegek – minden sornak külön Y, legalabb 22px közök
    _status_text     = new TextWidget(10,            8,  "",           255, 255,   0);
    _ships_left_text = new TextWidget(10,           35,  "",           160, 255, 160);
    _p1_label        = new TextWidget(P1_X,         88,  "P1 flotta", 130, 170, 220);
    _p2_label        = new TextWidget(P2_X,         88,  "P2 flotta", 130, 170, 220);
    _score_text      = new TextWidget(_width/2-90,  88,  "P1: 0 | P2: 0", 255, 220, 100);
    _dir_text        = new TextWidget(P1_X + 140,  _height - 50, "Irany: Vizszintes", 180, 180, 180);

    // Shot-type lista – alul középen, 150px magas -> 3 sor × 50px
    const int LIST_W = 160;
    const int LIST_H = 150;
    const int LIST_X = _width/2 - LIST_W/2;
    const int LIST_Y = _height - LIST_H - 10;
    _shot_type_list = new List(LIST_X, LIST_Y, LIST_W, LIST_H, {});
    _shot_label     = new TextWidget(LIST_X, LIST_Y - 22, "Loves tipusa:", 200, 200, 200);

    // Gyűjtsd össze az összes widget (az _all_widgets csak memóriakezelésre)
    _all_widgets.push_back(_p1_board);
    _all_widgets.push_back(_p2_board);
    _all_widgets.push_back(_action_btn);
    _all_widgets.push_back(_start_bot_btn);
    _all_widgets.push_back(_start_pvp_btn);
    _all_widgets.push_back(_back_to_menu_btn);
    _all_widgets.push_back(_pass_turn_btn);
    _all_widgets.push_back(_p1_label);
    _all_widgets.push_back(_p2_label);
    _all_widgets.push_back(_status_text);
    _all_widgets.push_back(_dir_text);
    _all_widgets.push_back(_score_text);
    _all_widgets.push_back(_ships_left_text);
    _all_widgets.push_back(_shot_type_list);
    _all_widgets.push_back(_shot_label);
    
    setup_state_widgets();
}

JatekMester::~JatekMester() {
    for (Widget * w : _all_widgets) delete w;
}

void JatekMester::start_game(bool pvp) {
    _pvp_mode = pvp;
    _p1_cross_ammo = 3;
    _p1_carpet_ammo = 2;
    _p2_cross_ammo = 3;
    _p2_carpet_ammo = 2;
    _p1_ships.clear();
    _p2_ships.clear();
    _p1_board->reset();
    _p2_board->reset();
    
    _p1_board->set_hidden_ships(false);
    _p2_board->set_hidden_ships(true);
    
    _ships_to_place = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    _horizontal_placement = true;
    _state = STATE_P1_PLACEMENT;
    _status_msg = "P1: Helyezd el a hajoikat! Kattints a bal oldali tabladra.";
    
    if (!_pvp_mode) {
        place_enemy_ships();
    }
    
    update_ships_left_text();
    update_shot_list();
    setup_state_widgets();
}

void JatekMester::setup_state_widgets() {
    _menu_widgets.clear();
    _game_widgets.clear();
    _placement_widgets.clear();
    
    if (_state == STATE_MAIN_MENU) {
        _menu_widgets.push_back(_start_bot_btn);
        _menu_widgets.push_back(_start_pvp_btn);
    } else if (_state == STATE_P1_PLACEMENT) {
        _placement_widgets.push_back(_p1_board);
        _placement_widgets.push_back(_action_btn);
        _placement_widgets.push_back(_dir_text);
        _placement_widgets.push_back(_ships_left_text);
        _placement_widgets.push_back(_p1_label);
    } else if (_state == STATE_P2_PLACEMENT) {
        _placement_widgets.push_back(_p2_board); // P2 places on right board
        _placement_widgets.push_back(_action_btn);
        _placement_widgets.push_back(_dir_text);
        _placement_widgets.push_back(_ships_left_text);
        _placement_widgets.push_back(_p2_label);
    } else if (_state == STATE_PASS_TURN) {
        _game_widgets.push_back(_pass_turn_btn);
    } else {
        // Game active or over
        _game_widgets.push_back(_p1_board);
        _game_widgets.push_back(_p2_board);
        _game_widgets.push_back(_p1_label);
        _game_widgets.push_back(_p2_label);
        _game_widgets.push_back(_score_text);
        
        if (_state == STATE_P1_TURN || _state == STATE_P2_TURN) {
            _game_widgets.push_back(_shot_type_list);
            _game_widgets.push_back(_shot_label);
        }
        if (_state == STATE_GAME_OVER) {
            _game_widgets.push_back(_back_to_menu_btn);
        }
    }
}

void JatekMester::update_shot_list() {
    std::vector<std::string> shots;
    int cross = (_state == STATE_P2_TURN) ? _p2_cross_ammo : _p1_cross_ammo;
    int carpet = (_state == STATE_P2_TURN) ? _p2_carpet_ammo : _p1_carpet_ammo;
    
    shots.push_back("Normal loves (vegtelen)");
    shots.push_back("Kereszt loves (" + std::to_string(cross) + " db)");
    shots.push_back("Szonyegbomba (" + std::to_string(carpet) + " db)");
    _shot_type_list->set_items(shots);
}

void JatekMester::update_ships_left_text() {
    if (_ships_to_place.empty()) {
        _ships_left_text->set_text("Minden hajo lehelyezve.");
        return;
    }
    
    std::string text = "Hajok: ";
    std::map<int, int> counts;
    for (int l : _ships_to_place) counts[l]++;
    
    bool first = true;
    for (auto it = counts.rbegin(); it != counts.rend(); ++it) {
        if (!first) text += ", ";
        text += std::to_string(it->first) + "-as: " + std::to_string(it->second) + "db";
        first = false;
    }
    _ships_left_text->set_text(text);
}

bool JatekMester::can_place_ship(const std::vector<Ship> &ships, int r, int c, int length, bool horizontal) const {
    if (horizontal) {
        if (c + length > 10) return false;
    } else {
        if (r + length > 10) return false;
    }
    for (int i = -1; i <= length; ++i) {
        for (int j = -1; j <= 1; ++j) {
            int check_r = horizontal ? r + j : r + i;
            int check_c = horizontal ? c + i : c + j;
            if (check_r >= 0 && check_r < 10 && check_c >= 0 && check_c < 10) {
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

void JatekMester::add_ship(std::vector<Ship> &ships, BoardWidget *board, int r, int c, int length, bool horizontal) {
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
            if (can_place_ship(_p2_ships, r, c, len, horiz)) {
                add_ship(_p2_ships, _p2_board, r, c, len, horiz);
                placed = true;
            }
        }
    }
}

void JatekMester::on_board_click(int r, int c, bool is_p1_board) {
    if (_state == STATE_P1_PLACEMENT && is_p1_board) {
        if (_ships_to_place.empty()) return;
        int len = _ships_to_place.front();
        if (can_place_ship(_p1_ships, r, c, len, _horizontal_placement)) {
            add_ship(_p1_ships, _p1_board, r, c, len, _horizontal_placement);
            _ships_to_place.erase(_ships_to_place.begin());
            update_ships_left_text();
            if (_ships_to_place.empty()) {
                if (_pvp_mode) {
                    _state = STATE_PASS_TURN;
                    _next_state = STATE_P2_PLACEMENT;
                    _status_msg = "A gepet at kell adni P2-nek. Kattints a tovabb gombra.";
                    _ships_to_place = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
                    update_ships_left_text();
                } else {
                    _state = STATE_P1_TURN;
                    _status_msg = "P1 jon! Kattints az ellenseg tablajara.";
                }
                setup_state_widgets();
            }
        }
    } else if (_state == STATE_P2_PLACEMENT && !is_p1_board) {
        if (_ships_to_place.empty()) return;
        int len = _ships_to_place.front();
        if (can_place_ship(_p2_ships, r, c, len, _horizontal_placement)) {
            add_ship(_p2_ships, _p2_board, r, c, len, _horizontal_placement);
            _ships_to_place.erase(_ships_to_place.begin());
            update_ships_left_text();
            if (_ships_to_place.empty()) {
                _state = STATE_PASS_TURN;
                _next_state = STATE_P1_TURN;
                _status_msg = "P2 kesz. Add at P1-nek.";
                setup_state_widgets();
            }
        }
    } else if (_state == STATE_P1_TURN && !is_p1_board) {
        int shot_type = _shot_type_list->get_selected_index();
        if (shot_type == -1) shot_type = 0; 
        if (shot_type == 1 && _p1_cross_ammo <= 0) { _status_msg = "Nincs tobb Kereszt loves!"; return; }
        if (shot_type == 2 && _p1_carpet_ammo <= 0) { _status_msg = "Nincs tobb Szonyegbomba!"; return; }
        
        std::vector<std::pair<int, int>> targets;
        if (shot_type == 0) { targets.push_back({r, c}); }
        else if (shot_type == 1) { _p1_cross_ammo--; for (int i=0; i<10; ++i) { targets.push_back({r, i}); targets.push_back({i, c}); } }
        else if (shot_type == 2) { _p1_carpet_ammo--; for (int i=-1; i<=1; ++i) for (int j=-1; j<=1; ++j) targets.push_back({r+i, c+j}); }
        
        update_shot_list();
        bool any_hit = false;
        for (auto p : targets) {
            int tr = p.first; int tc = p.second;
            CellState st = _p2_board->get_cell(tr, tc);
            if (st == EMPTY || st == SHIP) {
                if (st == SHIP) { _p2_board->set_cell(tr, tc, HIT); any_hit = true; }
                else { _p2_board->set_cell(tr, tc, MISS); }
            }
        }
        
        update_sunk_ships(_p2_ships, _p2_board);
        if (check_win(_p2_ships)) {
            _state = STATE_GAME_OVER; _status_msg = "P1 GYOZOTT!"; setup_state_widgets();
        } else {
            if (any_hit) {
                _status_msg = "Talalat! P1 jossz ujra.";
            } else {
                if (_pvp_mode) {
                    _state = STATE_PASS_TURN; _next_state = STATE_P2_TURN; _status_msg = "Melle. Add at P2-nek."; setup_state_widgets();
                } else {
                    _state = STATE_P2_TURN; _status_msg = "Melle. A bot jon."; setup_state_widgets();
                }
            }
        }
    } else if (_state == STATE_P2_TURN && is_p1_board) {
        int shot_type = _shot_type_list->get_selected_index();
        if (shot_type == -1) shot_type = 0; 
        if (shot_type == 1 && _p2_cross_ammo <= 0) { _status_msg = "Nincs tobb Kereszt loves!"; return; }
        if (shot_type == 2 && _p2_carpet_ammo <= 0) { _status_msg = "Nincs tobb Szonyegbomba!"; return; }
        
        std::vector<std::pair<int, int>> targets;
        if (shot_type == 0) { targets.push_back({r, c}); }
        else if (shot_type == 1) { _p2_cross_ammo--; for (int i=0; i<10; ++i) { targets.push_back({r, i}); targets.push_back({i, c}); } }
        else if (shot_type == 2) { _p2_carpet_ammo--; for (int i=-1; i<=1; ++i) for (int j=-1; j<=1; ++j) targets.push_back({r+i, c+j}); }
        
        update_shot_list();
        bool any_hit = false;
        for (auto p : targets) {
            int tr = p.first; int tc = p.second;
            CellState st = _p1_board->get_cell(tr, tc);
            if (st == EMPTY || st == SHIP) {
                if (st == SHIP) { _p1_board->set_cell(tr, tc, HIT); any_hit = true; }
                else { _p1_board->set_cell(tr, tc, MISS); }
            }
        }
        
        update_sunk_ships(_p1_ships, _p1_board);
        if (check_win(_p1_ships)) {
            _state = STATE_GAME_OVER; _status_msg = "P2 GYOZOTT!"; setup_state_widgets();
        } else {
            if (any_hit) {
                _status_msg = "Talalat! P2 jossz ujra.";
            } else {
                _state = STATE_PASS_TURN; _next_state = STATE_P1_TURN; _status_msg = "Melle. Add at P1-nek."; setup_state_widgets();
            }
        }
    }
}

void JatekMester::update_sunk_ships(std::vector<Ship> &ships, BoardWidget *board) {
    for (auto &s : ships) {
        if (!s.sunk) {
            bool all_hit = true;
            for (int i = 0; i < s.length; ++i) {
                int sr = s.horizontal ? s.r : s.r + i;
                int sc = s.horizontal ? s.c + i : s.c;
                if (board->get_cell(sr, sc) != HIT) { all_hit = false; break; }
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

bool JatekMester::check_win(const std::vector<Ship> &ships) const {
    for (const auto &s : ships) {
        if (!s.sunk) return false;
    }
    return true;
}

void JatekMester::bot_shoot() {
    bool shot = false;
    while (!shot) {
        int r = std::rand() % 10;
        int c = std::rand() % 10;
        CellState state = _p1_board->get_cell(r, c);
        
        if (state == EMPTY || state == SHIP) {
            if (state == SHIP) {
                _p1_board->set_cell(r, c, HIT);
                update_sunk_ships(_p1_ships, _p1_board);
                if (check_win(_p1_ships)) {
                    _state = STATE_GAME_OVER;
                    _status_msg = "BOT GYOZOTT!";
                    setup_state_widgets();
                }
            } else {
                _p1_board->set_cell(r, c, MISS);
                if (_state != STATE_GAME_OVER) {
                    _state = STATE_P1_TURN;
                    _status_msg = "Te jossz! Kattints az ellenseg tablajara.";
                    setup_state_widgets();
                }
            }
            shot = true;
        }
    }
}

void JatekMester::update_score() {
    int p1_sunk = 0;
    for (const auto &s : _p2_ships) if (s.sunk) p1_sunk++;
    int p2_sunk = 0;
    for (const auto &s : _p1_ships) if (s.sunk) p2_sunk++;
    _score_text->set_text("P1 pont: " + std::to_string(p1_sunk) + " - P2 pont: " + std::to_string(p2_sunk));
}

void JatekMester::run() {
    event ev;
    int focus = -1;
    int enemy_delay = 0;

    while (gin >> ev && ev.keycode != key_escape) {
        if (ev.type == ev_timer) {
            if (_state == STATE_P2_TURN && !_pvp_mode) {
                enemy_delay++;
                if (enemy_delay > 20) { 
                    bot_shoot();
                    enemy_delay = 0;
                }
            }
        }
        
        std::vector<Widget*>* active_widgets = &_all_widgets;
        if (_state == STATE_MAIN_MENU) active_widgets = &_menu_widgets;
        else if (_state == STATE_P1_PLACEMENT || _state == STATE_P2_PLACEMENT) active_widgets = &_placement_widgets;
        else if (_state == STATE_PASS_TURN) active_widgets = &_game_widgets; 
        else active_widgets = &_game_widgets;
    
        if (ev.type == ev_mouse && ev.button == btn_left) {
            focus = -1;
            for (size_t i = 0; i < active_widgets->size(); i++) {
                if ((*active_widgets)[i]->is_selected(ev.pos_x, ev.pos_y)) {
                    focus = i;
                }
            }
        }
        
        if (ev.type == ev_mouse && ev.button == btn_right && (_state == STATE_P1_PLACEMENT || _state == STATE_P2_PLACEMENT)) {
            _horizontal_placement = !_horizontal_placement;
        }
        
        if (focus != -1 && focus < (int)active_widgets->size()) {
            (*active_widgets)[focus]->handle(ev);
        }

        gout << color(20, 20, 25) << move_to(0, 0) << box(_width, _height);
        
        _status_text->set_text(_status_msg);
        _status_text->draw();
        
        if (_state == STATE_MAIN_MENU) {
            gout << move_to(_width / 2 - 130, _height / 3 - 30) << color(255, 100, 100) << text("TORPEDO JATEK");
        } else if (_state == STATE_PASS_TURN) {
            gout << move_to(_width / 2 - 150, _height / 3) << color(255, 200, 100) << text("A gepet add at a masik jatekosnak!");
        }
        
        if (_state == STATE_P1_PLACEMENT || _state == STATE_P2_PLACEMENT) {
            std::string dir = _horizontal_placement ? "Vizszintes" : "Fuggoleges";
            _dir_text->set_text("Irany (Jobb klikk): " + dir);
        }

        if (_state == STATE_P1_TURN || _state == STATE_P2_TURN || _state == STATE_GAME_OVER || _state == STATE_P2_TURN) {
            update_score();
        }

        for (Widget * w : *active_widgets) {
            w->draw();
        }
        gout << refresh;
    }
}
