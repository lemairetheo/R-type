#include "RTypeClientGraphics.hpp"


int main(void)
{

    RTypeClientGraphics graphics(800, 600, "R-Type Client");
    std::cout << "Hello, World from client !" << std::endl;
    graphics.run();
    return 0;
}