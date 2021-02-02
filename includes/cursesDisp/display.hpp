#pragma once

#include <memory>
#include <queue>
#include <list>
#include <chrono>
#include <thread>
#include <future>
#include <mutex>	// for std::call_once, and std::once_flag

#include "multiTerm/single_term.hpp"
#include "curses_subwin.hpp"
#include "node_adapter.hpp"

/* This class will just `hold` everything, with all handling of `its members` being controlled by the parent verse object */
class Display : protected single_term, public std::enable_shared_from_this<Display>{
	typedef std::shared_ptr<SubWindow> SubWindow_Ptr;
	typedef std::shared_ptr<Display> Display_Ptr;

public:
	bool paused{ true };
//	std::once_flag disp_once_flag;	// @deprecated - Was in use when std::call_once was being used to call updateDisplay()

	std::queue<std::shared_ptr<node_adapter>> queue;	// stores pointers to node_adapters

	template<typename DerivedAdapter>
	std::shared_ptr<node_adapter> newNodeAdapter();
	void runInitTasks() override;
	void printScreen();
	void updateScreen();

	std::mutex event_mutex;	// used to lock mutex so that ONLY ONE thread accesses it during each time interval the display is active
	void startEventLoop();

	/**
	* @note - Display::render() shouldn't be used anymore
	* @why - Since curses is not gauranteed to be thread safe
	* 
	* What it was doing is, in a while loop, every 200ms it will update the screen
	* Consider the scenario, when a node_adapter WRITES to it's own subwindow, (WHICH IS PART OF the main_area), and simultaneously, Display::render writes something at same position through main_area, this is the problem, this shouldn't be a problem but i am not sure if modifying a window and its subwindow is safe or not
	* 
	* @implications -> Since we wont use it, we will need 3 functions -> printScreen(), updateScreen() AND a clickHandler()
	* Also, i first decided to use std::call_once for calls to Display::updateScreen()
	* But, now i have found std::timed_mutex, it seems more appropriate, ie. the function will be called once for the interval, and also it will be waiting for that much interval, GREAT
	*/
	void render();
	void showExit();
	void helpScreen();	// @bug - Doesn't work as of 21st Nov 20
	void optionScreen();	// shows options -> 1. View verse state; 2. View help
	void runPreEndTasks();
	void resumeRendering();
	void pauseRendering();

	Display();
	~Display();

private:
	static constexpr char QUIT_KEY{ 'q' };
	static constexpr char HELP_KEY{ 'h' };

	int adapters_height{ 10 };
	int adapters_width{ 17 };	// these should be same for all, as of the current situation
		// @future - Try to make these adapt to, for eg. more snakes, so ONLY the next code for determining the position should need change, after the y_length, and x_length aren't same for all node_adapters anymore

	SubWindow_Ptr top_area, main_area, legend_area;

	std::vector<std::vector<bool>> occupy_table;	/* the occupy table keeps record of what regions are occupied (also called, occupancy_table) */
	std::timed_mutex dispMutex;

	struct
	{
	private:
		std::function<void(char, Display_Ptr)> kbhit = [](char key, Display_Ptr source) {
			if (source == nullptr) {
				throw std::invalid_argument("Source to kbhit handler is null");
			}

			if (key == Display::QUIT_KEY) {
				source->reset_curses();	// free up memory
				source->pauseRendering();
			}
		};

		friend void Display::startEventLoop();	// @note @important -> MUST be on a different thread than main thread
	} handlers;

	friend class node_adapter;
};

template<typename DerivedAdapter>
inline std::shared_ptr<node_adapter> Display::newNodeAdapter(){
	static_assert(std::is_base_of_v<node_adapter, DerivedAdapter>,
		"Your custom node adapter MUST inherit from node_adapter");

	// @note - Not using the_occupy_currently
	this->main_area->updateDimen();
	this->pauseRendering();

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

	std::shared_ptr<node_adapter> adapter( 
		new DerivedAdapter(
			this->shared_from_this(),
			this->adapters_height,
			this->adapters_width,
			y_corner,
			x_corner
		)
	);

	queue.push(adapter);
	// adapter->update();	// get it on screen

	this->main_area->refresh();

	return adapter;
}
