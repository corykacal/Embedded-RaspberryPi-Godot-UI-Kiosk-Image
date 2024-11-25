#include "display.hpp"

int main() {
    Display display;
    display.clear(0xF800);  // Red screen
    while(1) sleep(1);
    return 0;
}
