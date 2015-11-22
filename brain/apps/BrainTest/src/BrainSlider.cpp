//
//  BrainSlider.cpp
//  BrainTest
//
//  Created by Benjamin Bojko on 11/21/15.
//
//

#include "BrainSlider.h"

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

BrainSlider::BrainSlider(ci::Rectf trackBounds) {
	const vec2 knobSize = vec2(20, 20);
	mTrackBounds = trackBounds;
	mKnobBounds = ci::Rectf(trackBounds.x1, trackBounds.getCenter().y - knobSize.y * 0.5, trackBounds.x1 + knobSize.x, trackBounds.getCenter().y + knobSize.y * 0.5);
	
	ci::app::getWindow()->getSignalMouseDown().connect([&](MouseEvent event) {
		mIsDragging = mKnobBounds.contains(event.getPos());
		
		// tap on track
		if (!mIsDragging && mTrackBounds.contains(event.getPos())) {
			moveTo(event.getX());
		}
	});
	
	ci::app::getWindow()->getSignalMouseDrag().connect([&](MouseEvent event) {
		if (!mIsDragging) {
			return;
		}
		
		moveTo(event.getX());
	});
	
	ci::app::getWindow()->getSignalMouseUp().connect([&](MouseEvent event) {
		mIsDragging = false;
	});
}

void BrainSlider::update() {
}

void BrainSlider::draw() {
	gl::color(1, 1, 1, 0.5);
	gl::drawSolidRect(mTrackBounds);
	
	gl::color(1, 0, 0, 0.5);
	gl::drawSolidRect(mKnobBounds);
}

void BrainSlider::moveTo(float x) {
	float knobWidthHalf = mKnobBounds.getWidth() * 0.5f;
	
	x = glm::clamp(x, mTrackBounds.x1 + knobWidthHalf, mTrackBounds.x2 - knobWidthHalf);
	
	mKnobBounds.x1 = x - knobWidthHalf;
	mKnobBounds.x2 = x + knobWidthHalf;
}

float BrainSlider::getPosition() {
	float knobWidth = mKnobBounds.getWidth();
	float knobCenterX = mKnobBounds.getCenter().x;
	float left = mTrackBounds.x1 + knobWidth * 0.5;
	float width = mTrackBounds.getWidth() - knobWidth;
	return (knobCenterX - left) / width;
}
