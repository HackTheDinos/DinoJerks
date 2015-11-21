//
//  BrainGrabber.cpp
//  BrainTest
//
//  Created by Charlie Whitney on 11/20/15.
//
//

#include "BrainGrabber.h"

using namespace ci;
using namespace ci::app;
using namespace std;

BrainGrabber::BrainGrabber() :
    mColorTolerance(0.01),
    mColToMatch(1,1,1)
{
    mGui = pretzel::PretzelGui::create();
    mGui->addColorPicker("Brain Color", &mColToMatch);
    mGui->addSlider("Tolerance", &mColorTolerance, 0.0, 1.0);
    mGui->addSaveLoad();
    
    mGui->loadSettings();
}

void BrainGrabber::setup()
{
    fs::path pth = ci::app::getOpenFilePath( getAssetPath("") );
    
    if( !pth.empty() ){
        fs::path rootPath = pth.parent_path();
        
        for( fs::directory_iterator it(rootPath); it != fs::directory_iterator(); ++it)
        {
            mSurfList.push_back( Surface( loadImage(*it) ) );
        }
        
    }else{
//        console() << "cancel" << endl;
    }
}

void BrainGrabber::update()
{
    if( mSurfList.size() ){
        findContour( mSurfList[0] );
    }
}

void BrainGrabber::findContour( Surface surf )
{
    cv::Mat input = toOcv( Channel(surf) );
    
    // transform the material to find the specified color
    ci::Color hsvCol = Color( CM_HSV, mColToMatch.r, mColToMatch.g, mColToMatch.b );
    
    ci::Color lB( CM_RGB, hsvCol.r, hsvCol.g, min(1.0f, hsvCol.b - mColorTolerance) );
    ci::Color uB( CM_RGB, hsvCol.r, hsvCol.g, min(1.0f, hsvCol.b - mColorTolerance) );
    
    // these are in bgr?
    cv::Scalar lowerBounds(lB.r * 255.0, lB.g * 255.0, lB.b * 255.0);
    cv::Scalar upperBounds(uB.r * 255.0, uB.g * 255.0, uB.b * 255.0);
    cv::inRange(input, lowerBounds, upperBounds, input);
    
    
    mDebugTex = gl::Texture::create( fromOcv(input) );
    
    // find outlines
//    cv::findContours(input, mContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    
    
    return;
    
    mContourList.clear();
    for (ContourVector::const_iterator iter = mContours.begin(); iter != mContours.end(); ++iter)
    {
        vector<vec2> mPoints;
        PolyLine2f pl;
        pl.setClosed();
        
        // FORM POLYLINE AND FIND CENTROID
        for (vector<cv::Point>::const_iterator pt = iter->begin(); pt != iter->end(); ++pt) {
            vec2 p( fromOcv(*pt) );
            mPoints.push_back(p);
            pl.push_back(p);
        }
        
        mContourList.push_back( mPoints );
    }
}

void BrainGrabber::draw()
{
    gl::pushMatrices();{
        gl::color(1,1,1,1);
        gl::translate( vec2(250, 10) );
        if( mSurfList.size() ){
            gl::draw( gl::Texture::create( mSurfList[0] ) );
            
            if( mContourList.size() ){
                gl::ScopedColor scCol( 1,0,0,1 );
                gl::draw( mContourList[0] );
            }
        }
        
        if(mDebugTex){
            gl::draw(mDebugTex);
        }
    }gl::popMatrices();
    
    pretzel::PretzelGui::drawAll();
}
