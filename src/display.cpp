// @note @future -> Later try to get this into a single display.hpp for easier use with modifications later on

#include "verse.hpp"
#include "display/display.hpp"
#include "display/node_adapter.hpp"

void Display::pauseRendering(){
	this->paused = true; 
}

void Display::resumeRendering(){
	this->paused = false;
}

std::shared_ptr<node_adapter> Display::newNodeAdapter(World_Node* node){
	// @note - Not using the_occupy_currently
	this->main_area->updateDimen();

	int y_corner;
	int x_corner;
	int index_num = queue.size();
	if(index_num == 8){
		queue.pop();

		index_num = 7;
	}

	int bigBox_coord_x;
	int bigBox_coord_y;	// the big box's corner coord

	bigBox_coord_y = ((main_area->getmax_y()+1)/2) * (index_num / 4);	// first row or second row
	bigBox_coord_x = ((main_area->getmax_x()+1)/4) * (index_num % 4);

	if( index_num %2 == 0 ){
		y_corner = bigBox_coord_y;
	}else{
		y_corner = bigBox_coord_y + 2;	// 2 lines below
	}

		// here what we want is to `horizonatlly center` the node_adapter inside this big_box
	x_corner = bigBox_coord_x + ( (main_area->getmax_x()+1)/4 - adapters_width ) / 2;

	std::shared_ptr<node_adapter> adapter{ new node_adapter(this->shared_from_this(), node, this->adapters_height, this->adapters_width, y_corner, x_corner) };
	queue.push(adapter->node);
	queue.adapters.push_back(adapter);

	return std::move(adapter);
}

void Display::helpScreen(){
	using namespace std::chrono_literals;

	this->curses_init();	// @note - single_term itself manages if curses is already initialised

	this->resumeRendering();
	this->main_area->disable();
	this->legend_area->disable();

	SubWindow helpArea{ 0, 0, 3, 0 };	// don't make it a raw pointer, since for it's destructor to be called, it's scope should end, which won't happen if we use new SubWindow, with raw pointer

	helpArea.box();
	auto async_input = this->get_async_input();
	
	helpArea.addstr(1, 1, "Help Guide");
	helpArea.nladdstr("====================");

	helpArea.newline();
	helpArea.nladdstr("*All worlds continue on diff. threads,w/o blocking the display, or the verse");

	helpArea.newline();

	helpArea.nladdstr("- Type the id to chose a particular world");

	helpArea.nladdstr("- Commands");
	helpArea.nladdstr("-   N - Namaste World(New)");
	helpArea.nladdstr("-   P - Pause");
	helpArea.nladdstr("-   R - Resume");
	helpArea.nladdstr("-   T - Time Travel !!");
	helpArea.nladdstr("-   L - Logs (of World)");
	helpArea.nladdstr("-   V - Logs (of Verse)");

	helpArea.addstr(-3, 1, "If you find a problem...");
	helpArea.nladdstr("Please solve it yourselves");
	helpArea.nladdstr(":copy: AdityaG15 :D");

	top_area->addstr(title, position::MIDDLE);

	int cur_dimen_y, cur_dimen_x;
	getmaxyx(stdscr, this->_terminal_y, this->_terminal_x);
	while( true ){
		main_area->updateDimen();
		legend_area->updateDimen();

		getmaxyx(stdscr, cur_dimen_y, cur_dimen_x);
		if( cur_dimen_y != _terminal_y || cur_dimen_x != _terminal_x ){
			this->clearAll(); // to clean out the previous borders
		}

		top_area->box();
		main_area->box();
		legend_area->box();

		this->refreshAll();

		if( async_input.wait_for(100ms) == std::future_status::ready ){
			char c;
			try{
				if(c == Display::QUIT_KEY){
					this->reset_curses();	// free up memory

					return;
				}
				helpArea.refresh();
			}catch(std::future_error & e){
				raise(SIGTERM);

				this->parent_verse->kaal_day("Display");
				return;
			}

			if( c == Display::QUIT_KEY )	return;
			helpArea.refresh();
			async_input = this->get_async_input();	// restart that task
		}

		std::this_thread::sleep_for(200ms);
	}

	this->optionScreen();

}

void Display::optionScreen(){
	// @future @todo -> read comment in the declaration, and implement it
}

void Display::runInitTasks(){
	using namespace std::chrono_literals;

	// if( std::this_thread::get_id() == this->parent_verse->thread_id ){
	//     ERROR: DISPLAY SHOULD BE ON A DIFFERENT THREAD, THAN THE VERSE
	// }

	this->curses_init();

	this->resumeRendering();
	this->clearAll();

	if( !top_area ) this->top_area.reset( new SubWindow( 3, 0, 0, 0 ) );
	if( !main_area ) this->main_area.reset( new SubWindow( 0, 0.8f * _terminal_x, 3, 0 ) );
	if( !legend_area ) this->legend_area.reset( new SubWindow( 0, 0, 3, 0.8f*_terminal_x ) );

	if( top_area->enabled ) this->top_area->enable();
	if( main_area->enabled ) this->top_area->enable();
	if( legend_area->enabled ) this->top_area->enable();

	auto async_input = this->get_async_input();

	legend_area->addstr(1, 1, "Legend");

	legend_area->nladdstr("*All worlds continue on diff. threads,w/o blocking the display, or the verse");

	legend_area->newline();

	legend_area->nladdstr("- Type the id to chose a particular world");

	legend_area->nladdstr("- Commands");
	legend_area->nladdstr("-   N - Namaste World(New)");
	legend_area->nladdstr("-   P - Pause");
	legend_area->nladdstr("-   R - Resume");
	legend_area->nladdstr("-   T - Time Travel !!");
	legend_area->nladdstr("-   L - Logs (of World)");
	legend_area->nladdstr("-   V - Logs (of Verse)");

	legend_area->addstr(-3, 1, "If you find a problem...");
	legend_area->nladdstr("Please solve it yourselves");
	legend_area->nladdstr(":copy: AdityaG15 :D");

	int cur_dimen_y, cur_dimen_x;
	getmaxyx(stdscr, this->_terminal_y, this->_terminal_x);
	while( true ){
		main_area->updateDimen();
		legend_area->updateDimen();

		getmaxyx(stdscr, cur_dimen_y, cur_dimen_x);
		if( cur_dimen_y != _terminal_y || cur_dimen_x != _terminal_x ){
			wclear(stdscr); // to clean out the previous borders
		}

		top_area->box();
		main_area->box();
		legend_area->box();

		top_area->addstr(title, position::MIDDLE);

		wrefresh(stdscr);

			// time to take input
		if( async_input.wait_for(100ms) == std::future_status::ready ){
			legend_area->addstr(static_cast<int>(0.75f*legend_area->getmax_y()), 1, "You entered -> ");
			char c;
			try{
				c = async_input.get();
				legend_area->addch(c);

				if(c == Display::QUIT_KEY){
					this->reset_curses();	// free up memory

					// @note - Or maybe use inter thread communication here, using condition_variable, so to wait till verse tells all done, then this will return, and the destructor again sends signal to verse after all things done in destructor, then verse can be sure that all destructed
					// @note - UNCOMMENT next line, later
					// return this->parent_verse->kaal_day("Display");    // also passing the source so that it doesn't actually try to end this
					return;
				}
				legend_area->refresh();
			}catch(std::future_error & e){
				raise(SIGTERM);

				// verse::LOGGER << std::boolalpha << async_input.valid() << std::endl;
				// verse::ERR_LOGGER << e.code()<< " " << e.what() << std::endl;
				this->parent_verse->kaal_day("Display");
				return;
			}

			if( c == Display::QUIT_KEY )	return;
			legend_area->refresh();
			async_input = this->get_async_input();	// restart that task
		}

		std::this_thread::sleep_for(200ms);
	}

}

void Display::showExit(){
	clear();
	clearAll();

	mvwaddstr(stdscr, this->_terminal_y/2, (this->_terminal_x - 15)/2, "About to Exit !");
}

Display::Display(Verse* parent) :
	single_term("WorldLine Simulator v0.271", "Created by Aditya Gupta and Steins; Gate"),
	parent_verse(parent){}

Display::~Display(){

}
