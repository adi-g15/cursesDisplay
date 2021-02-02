#include "display.hpp"
#include "node_adapter.hpp"
#include <array>
#include <string>
#include <algorithm>

class MyNodeAdapter: public node_adapter {
private:
        // these two are sample data
    std::string name;
    std::array<std::string, 3> data;

public:
    MyNodeAdapter(std::shared_ptr<Display> dispMngr, int h, int w, int y_corner, int x_corner): node_adapter(dispMngr, h, w, y_corner, x_corner){
        this->init_display();
    }

    void init_display() {  // this method is executed in the constructor itself after first render is complete, do whatever you may need at construction here
        this->window.box(ACS_VLINE, '-');

        this->window.moveCursor(1,1);
        this->window.add_str(std::to_string(this->node_id).data(), position::MIDDLE);
        this->window.h_line();

        this->window.newline();
        this->window.printf("Name - %", this->name.data());

        for (auto &&s : this->data)
        {
            this->window.newline();
            this->window.printf("E% - %, %", s, this->node_id);
        }
    }

    void update() override {
        std::random_shuffle( data.begin(), data.end() );

        this->render();
    }
};

int main() {
    Display display;

    display.newNodeAdapter<MyNodeAdapter>();

    std::this_thread::sleep_for(std::chrono::seconds(10));
}