#ifndef JATEKMESTER_HPP
#define JATEKMESTER_HPP

#include "widgets.hpp"
#include "board_widget.hpp"
#include "button.hpp"
#include "text_widget.hpp"
#include "list.hpp"
#include <vector>
#include <string>

enum GameState {
    STATE_MAIN_MENU,
    STATE_P1_PLACEMENT,
    STATE_P2_PLACEMENT,
    STATE_PASS_TURN,
    STATE_P1_TURN,
    STATE_P2_TURN,
    STATE_GAME_OVER
};

struct Ship {
    int r, c;
    int length;
    bool horizontal;
    int hits;
    bool sunk;
};

class JatekMester {
    int _width, _height;
    
    std::vector<Widget*> _menu_widgets;
    std::vector<Widget*> _game_widgets;
    std::vector<Widget*> _placement_widgets;
    std::vector<Widget*> _all_widgets;
    
    Button* _start_bot_btn;
    Button* _start_pvp_btn;
    Button* _quit_btn;
    Button* _back_to_menu_btn;
    Button* _pass_turn_btn;
    
    BoardWidget* _p1_board;
    BoardWidget* _p2_board;
    Button* _action_btn;
    TextWidget* _status_text;
    TextWidget* _p1_label;
    TextWidget* _p2_label;
    TextWidget* _dir_text;
    TextWidget* _score_text;
    TextWidget* _ships_left_text;
    List* _shot_type_list;
    TextWidget* _shot_label;
    
    GameState _state;
    GameState _next_state; // For pass turn
    std::string _status_msg;
    bool _pvp_mode;
    bool _running;
    
    std::vector<int> _ships_to_place; // lengths of ships left to place
    bool _horizontal_placement;
    
    std::vector<Ship> _p1_ships;
    std::vector<Ship> _p2_ships;
    
    int _p1_cross_ammo;
    int _p1_carpet_ammo;
    int _p2_cross_ammo;
    int _p2_carpet_ammo;
    
    void update_shot_list();
    void update_ships_left_text();
    
    void on_board_click(int r, int c, bool is_p1_board);
    
    void place_enemy_ships();
    bool can_place_ship(const std::vector<Ship>& ships, int r, int c, int length, bool horizontal) const;
    void add_ship(std::vector<Ship>& ships, BoardWidget* board, int r, int c, int length, bool horizontal);
    
    void bot_shoot();
    void update_sunk_ships(std::vector<Ship>& ships, BoardWidget* board);
    bool check_win(const std::vector<Ship>& ships) const;
    void update_score();
    void start_game(bool pvp);
    void setup_state_widgets();

public:
    JatekMester(int width, int height);
    ~JatekMester();
    void run();
};

#endif
