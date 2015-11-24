//
//  BrainSlider.hpp
//  BrainTest
//
//  Created by Benjamin Bojko on 11/21/15.
//
//

#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

class BrainSlider
{
	
	
	
public:
	BrainSlider(ci::Rectf trackBounds);
	
	void update();
	void draw();
	
	/**
	 *  Normalized from 0...1
	 */
	float getPosition();
	
	bool isDragging();
	
	ci::Rectf mTrackBounds;
	ci::Rectf mKnobBounds;
	
private:
	
	void moveTo(float x);
	
	ci::vec2 mMousePos = ci::vec2(0,0);
	bool mIsDragging = false;
	
	float mPosition = 0.0f;
};