//
//  BrainGrabber.h
//  BrainTest
//
//  Created by Charlie Whitney on 11/20/15.
//
//

#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/Filesystem.h"
#include "cinder/ip/Resize.h"

#include "CinderOpenCV.h"

#include "pretzel/PretzelGui.h"

#include "BrainSlider.h"

class BrainGrabber
{
    
public:
    BrainGrabber();
    
    void setup();
    void update();
    void draw();
    
    void openFileDialog();
    
    pretzel::PretzelGuiRef   mGui;

private:
    void findContours( ci::Surface tex );
    
    // ALL LOADED IMAGES
    std::vector<ci::Surface> mSurfList;
  
    int mCurImageNum;
    
    // COLOR CORRECTION
    ci::Color   mColToMatch;
    float       mColorTolerance;
    ci::Color   lB, uB;
    bool        bRegenContours;
    
    // CONTOUR FINDER
    typedef std::vector< std::vector<cv::Point> > ContourVector;
    ContourVector   mContours;
    std::vector< std::vector<ci::vec2> > mContourList;
    
    ci::gl::VertBatchRef   mVerts;
    
    // DEBUG
    ci::gl::TextureRef mDebugTex;
	
	BrainSlider *mSlider;
};