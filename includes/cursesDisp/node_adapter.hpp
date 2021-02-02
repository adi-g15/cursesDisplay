#pragma once

#include <vector>
#include <utility>
#include <stdexcept>

#include "util/coord.hpp"
#include "curses_subwin.hpp"

class Display;	// forward-declaration

class node_adapter{
	typedef util::_coord2D<int> coordinate;	// on a screen so won't need anything bigger than an int
	typedef std::shared_ptr<Display> DispMngr;
public:
	SubWindow window;	// @note @caution -> The node_adapter's SubWindow isn't meant to be passed around ! (If tried to create a shared_ptr from this, will give double free)
	DispMngr dispMngr;	// @todo - Make this a non-modifieable by this class (should always point to same dispMngr)

	// the further data can be replaced by having a pointer t the wrld_node instead of all this individually
	const size_t node_id;
	static inline size_t Last_ID{1};

	node_adapter(DispMngr dispMngr, int height, int width,int y_corner, int x_corner);

public:
	virtual void update() = 0;	// you should call render() inside this
	void disable();	// once disabled, can NOT be enabled again (as of NOW )
	void render();

	~node_adapter(){}	// this should be a virtual destructor, though not making it since not needed

	friend class Display;
};
