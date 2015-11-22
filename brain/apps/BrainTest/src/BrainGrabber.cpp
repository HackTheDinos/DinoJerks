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
	
	mSlider = new BrainSlider(ci::Rectf(100, getWindowHeight() - 100, getWindowWidth() - 200, getWindowHeight() - 100 + 2));
}

void BrainGrabber::setup()
{
    fs::path pth = ci::app::getOpenFilePath( getAssetPath("") );
    
    if( !pth.empty() ){
        fs::path rootPath = pth.parent_path();
        
        for( fs::directory_iterator it(rootPath); it != fs::directory_iterator(); ++it)
        {
            vec2 maxSize(600.0, 600.0);
            Surface sf( loadImage(*it) );
            
            vec2 sizeDiff = maxSize / (vec2)sf.getSize();
            float ratio = min(sizeDiff.x, sizeDiff.y);
            
            vec2 newSize = (vec2)sf.getSize() * vec2(ratio);
            Surface rSurf( newSize.x, newSize.y, false );
            rSurf.copyFrom(sf, ci::Area(0,0,sf.getWidth(), sf.getHeight()) );
            
            console() << "New size :: " << newSize << " / " << sizeDiff << endl;
            
            mSurfList.push_back( sf );
        }
        
    }else{
        console() << "Cancel" << endl;
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
    vec3 hsvCol = rgbToHsv( mColToMatch );
    
    // TO HSL
    vec3 lBv = vec3( hsvCol.r, hsvCol.g, max(0.0f, hsvCol.b - mColorTolerance) );
    vec3 uBv = vec3( hsvCol.r, hsvCol.g, min(1.0f, hsvCol.b + mColorTolerance) );
    
    // BACK TO RGB
    lB = hsvToRgb( lBv );
    uB = hsvToRgb( uBv );
    
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
//            gl::draw( gl::Texture::create( mSurfList[0] ) );
            
            if( mContourList.size() ){
                gl::ScopedColor scCol( 1,0,0,1 );
                gl::draw( mContourList[0] );
            }
        }
        
        if(mDebugTex){
//            ci::color( Color(1,0,0) );
//            gl::drawSolidRect(
            gl::draw(mDebugTex);
        }
        
        {
            gl::translate(0, 600);
            gl::color( mColToMatch );
            gl::drawSolidRect( Rectf(0,0,50,50) );
            gl::color( lB );
            gl::drawSolidRect( Rectf(50,0,100,50) );
            gl::color( uB );
            gl::drawSolidRect( Rectf(100,0,150,50) );
            gl::color( Color::white() );
        }
		
    }gl::popMatrices();
	
    pretzel::PretzelGui::drawAll();
	mSlider->draw();
	
	gl::drawString("Slider pos: " + to_string(mSlider->getPosition()), vec2(100, getWindowHeight() - 80));
}
