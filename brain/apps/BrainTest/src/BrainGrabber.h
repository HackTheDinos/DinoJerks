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

#include "CinderOpenCV.h"

#include "pretzel/PretzelGui.h"

class BrainGrabber
{
    

    
public:
    BrainGrabber();
    
    void setup();
    void update();
    void draw();
    
    pretzel::PretzelGuiRef   mGui;

private:
    void findContour( ci::Surface tex );
    
    std::vector<ci::Surface> mSurfList;
    
    ci::Color   mColToMatch;
    float       mColorTolerance;
    
    typedef std::vector< std::vector<cv::Point> > ContourVector;
    ContourVector   mContours;
    std::vector< std::vector<ci::vec2> > mContourList;
    
    ci::gl::TextureRef mDebugTex;
};