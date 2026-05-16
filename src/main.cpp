#include "jatekmester.hpp"
#include "graphics.hpp"

using namespace genv;

int main() {
    gout.open(800, 600);
    gout.load_font("assets/LiberationSans-Regular.ttf", 18);
    gin.timer(40);
    
    JatekMester jm(800, 600);
    jm.run();
    return 0;
}
