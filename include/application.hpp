#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "widgets.hpp"
#include "list.hpp"
#include "button.hpp"
#include <vector>
#include <string>

class Application {
    int _width, _height;
    std::vector<Widget*> _widgets;
    
    List * _customer_list;
    List * _car_list;
    List * _rental_list;
    Button * _rent_btn;
    Button * _return_btn;

    std::vector<std::string> load_cars_from_csv(std::string filename);
    void rent_car();
    void return_car();

public:
    Application(int width, int height);
    ~Application();
    void run();
};

#endif
