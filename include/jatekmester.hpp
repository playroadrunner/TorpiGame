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
    STATE_PLACEMENT,
    STATE_PLAYER_TURN,
    STATE_ENEMY_TURN,
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
    std::vector<Widget*> _widgets;
    
    BoardWidget* _player_board;
    BoardWidget* _enemy_board;
    Button* _action_btn;
    TextWidget* _status_text;
    TextWidget* _player_label;
    TextWidget* _enemy_label;
    TextWidget* _dir_text;
    TextWidget* _score_text;
    List* _shot_type_list;
    TextWidget* _shot_label;
    
    GameState _state;
    std::string _status_msg;
    
    std::vector<int> _ships_to_place; // lengths of ships left to place
    bool _horizontal_placement;
    
    std::vector<Ship> _player_ships;
    std::vector<Ship> _enemy_ships;
    
    void on_player_board_click(int r, int c);
    void on_enemy_board_click(int r, int c);
    
    void place_enemy_ships();
    bool can_place_ship(const std::vector<Ship>& ships, int r, int c, int length, bool horizontal) const;
    void add_ship(std::vector<Ship>& ships, BoardWidget* board, int r, int c, int length, bool horizontal);
    
    void enemy_shoot();
    void update_sunk_ships(std::vector<Ship>& ships, BoardWidget* board);
    bool check_win(const std::vector<Ship>& ships) const;
    void update_score();

public:
    JatekMester(int width, int height);
    ~JatekMester();
    void run();
};

#endif
