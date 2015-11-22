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

class BrainGrabber
{
    
public:
    BrainGrabber();
    
    void setup();
    void update();
    void draw();
    
    void openFileDialog();

private:
    enum BRAIN_VIEW_MODE {
        MODE_2D = 0,
        MODE_3D
    };
    
    struct SliceData {
        int             uuid;
        bool            bNeedsRecalc;
        ci::Surface     mSurface;
        std::vector<ci::gl::VertBatchRef> mContourList;
    };
    
    void draw2D();
    void draw3D();
    
    void findContours();
    
    pretzel::PretzelGuiRef   mGui;
    
    std::vector<SliceData>  mSliceDataList;
    
    // ALL LOADED IMAGES
    int mCurSliceNum;
    
    // COLOR CORRECTION
    ci::Color   mColToMatch;
    float       mColorTolerance;
    ci::Color   lB, uB;
    
    // hack settings
    ci::Color   mLastColor;
    float       mLastTolerance;
    
    // CONTOUR FINDER
    typedef std::vector< std::vector<cv::Point> > ContourVector;
    ContourVector   mContours;
    
    // VIEW
    BRAIN_VIEW_MODE mCurrentViewMode;
    
    // DEBUG
    ci::gl::TextureRef mDebugTex;
};