#include "application.hpp"
#include "graphics.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace genv;

Application::Application(int width, int height) : _width(width), _height(height) {
    gout.open(_width, _height);
    if (!gout.load_font("assets/LiberationSans-Regular.ttf", 18)) {
        std::cerr << "Nem sikerült betölteni a fontot!" << std::endl;
    }
    
    std::vector<std::string> customers = {"Kovács János", "Nagy Béla", "Szabó Erzsébet", "Tóth Gergő", "Kiss Anna"};
    std::vector<std::string> cars = load_cars_from_csv("assets/cars.csv");
    
    // Proportional layout calculations
    int margin = _width * 0.025; 
    int btn_w = 120;
    int btn_h = 45;
    
    int available_w = _width - (margin * 4) - btn_w;
    int list_w = available_w / 2;
    
    int title_space = 30;
    int available_h = _height - title_space - (margin * 4);
    int list_h = available_h / 2;

    _customer_list = new List(margin, title_space + 15, list_w, list_h, customers);
    _car_list = new List(list_w + margin * 2, title_space + 15, list_w, list_h, cars);
    
    _rent_btn = new Button(_width - btn_w - margin, title_space + 15, btn_w, btn_h, "Bérel", [this](){ this->rent_car(); });
    
    _rental_list = new List(margin, title_space + list_h + margin + 35, list_w * 2 + margin, list_h, {});
    _return_btn = new Button(_width - btn_w - margin, title_space + list_h + margin + 35, btn_w, btn_h, "Visszahoz", [this](){ this->return_car(); });
    
    _widgets.push_back(_customer_list);
    _widgets.push_back(_car_list);
    _widgets.push_back(_rental_list);
    _widgets.push_back(_rent_btn);
    _widgets.push_back(_return_btn);
}

Application::~Application() {
    for (Widget * w : _widgets) delete w;
}

std::vector<std::string> Application::load_cars_from_csv(std::string filename) {
    std::vector<std::string> res;
    std::ifstream f(filename);
    if (!f.is_open()) return {"Hiba: " + filename + " nem található"};
    
    std::string line;
    bool first = true;
    while (std::getline(f, line)) {
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == ' ')) {
            line.pop_back();
        }
        if (line.empty()) continue;
        if (first && (line.find("vehicle") != std::string::npos || line.find("make") != std::string::npos)) {
            first = false;
            continue;
        }
        first = false;
        std::replace(line.begin(), line.end(), ',', ' ');
        res.push_back(line);
    }
    return res;
}

void Application::rent_car() {
    int c_idx = _customer_list->get_selected_index();
    int v_idx = _car_list->get_selected_index();
    if (c_idx != -1 && v_idx != -1) {
        std::string customer = _customer_list->get_selected_item();
        std::string car = _car_list->get_selected_item();
        _rental_list->add_item(customer + " -> " + car);
        _car_list->remove_item(v_idx);
    }
}

void Application::return_car() {
    int r_idx = _rental_list->get_selected_index();
    if (r_idx != -1) {
        std::string rental = _rental_list->get_selected_item();
        size_t pos = rental.find(" -> ");
        if (pos != std::string::npos) {
            std::string car = rental.substr(pos + 4);
            _car_list->add_item(car);
            _rental_list->remove_item(r_idx);
        }
    }
}

void Application::run() {
    event ev;
    int focus = -1;
    while (gin >> ev && ev.keycode != key_escape) {
        if (ev.type == ev_mouse && ev.button == btn_left) {
            focus = -1;
            for (size_t i = 0; i < _widgets.size(); i++) {
                if (_widgets[i]->is_selected(ev.pos_x, ev.pos_y)) {
                    focus = i;
                }
            }
        }
        
        if (focus != -1) {
            _widgets[focus]->handle(ev);
        }
        
        if (ev.type == ev_mouse && (ev.button == btn_wheelup || ev.button == btn_wheeldown)) {
            for (Widget * w : _widgets) {
                if (w->is_selected(ev.pos_x, ev.pos_y)) {
                    w->handle(ev);
                }
            }
        }

        gout << color(20, 20, 25) << move_to(0, 0) << box(_width, _height);
        
        gout << move_to(_customer_list->x(), _customer_list->y() - 25) << color(180, 180, 200) << text("Ügyfelek");
        gout << move_to(_car_list->x(), _car_list->y() - 25) << color(180, 180, 200) << text("Elérhető autók");
        gout << move_to(_rental_list->x(), _rental_list->y() - 25) << color(180, 180, 200) << text("Aktív bérlések");

        for (Widget * w : _widgets) {
            w->draw();
        }
        gout << refresh;
    }
}
