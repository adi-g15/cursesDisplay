#include <mutex>    // for std::call_once
#include <cassert>

#include "node_adapter.hpp"
#include "display.hpp"
#include "curses_subwin.hpp"

node_adapter::node_adapter(DispMngr dispMngr, int height, int width,int y_corner, int x_corner) :
    node_id(node_adapter::Last_ID++),
    dispMngr(dispMngr),
    window(dispMngr->main_area, height, width, y_corner, x_corner)
{
    assert(dispMngr != nullptr);

    window.refresh();
    wrefresh(stdscr);
}

void node_adapter::render(){	// updates the content on the window, with updated content from the world_naode that is linked
    if( this->dispMngr->paused )	return;

    // @todo - Handle the display here

    dispMngr->updateScreen();
    //std::call_once( this->dispMngr->disp_once_flag, &Display::updateScreen, this->dispMngr );
}
