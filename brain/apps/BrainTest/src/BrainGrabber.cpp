//
//  BrainGrabber.cpp
//  BrainTest
//
//  Created by Charlie Whitney on 11/20/15.
//
//

#include "BrainGrabber.h"

#include <fstream>

using namespace ci;
using namespace ci::app;
using namespace std;

BrainGrabber::BrainGrabber() :
    mColorTolerance(0.01),
    mColToMatch(1,1,1),
    mCurSliceNum(0),
    mCurrentViewMode( BRAIN_VIEW_MODE::MODE_2D ),
    mCameraZ(500)
{
    mGui = pretzel::PretzelGui::create("SETTINGS");
    mGui->addButton("OPEN FILES", [&](void*){ openFileDialog(); }, this );
    mGui->addColorPicker("Brain Color", &mColToMatch);
    mGui->addSlider("Tolerance", &mColorTolerance, 0.0, 0.5);
    
    mGui->addLabel("VIEW MODE");
    mGui->addButton("2D MODE", [&](void*){ mCurrentViewMode = MODE_2D; }, this );
    mGui->addButton("3D MODE", [&](void*){ mCurrentViewMode = MODE_3D; }, this );
    
    mGui->addLabel("3D STUFF");
    mGui->addSlider("Camera Z", &mCameraZ, 0, 700);
    
    mGui->addLabel("SAVE / LOAD SETTINGS");
    mGui->addSaveLoad();
    
    mGui->loadSettings();
    
    getWindow()->getSignalResize().connect([&](){
        mGui->setPos( vec2(0) );
        mGui->setSize( vec2(250, getWindowHeight()) );
    });
	
//	getWindow()->getSignalMouseUp().connect([&](MouseEvent event){
//		saveToFile("test", getAppPath().string() + "/export.xyz");
//	});
	mSlider = new BrainSlider(ci::Rectf(250 + 50, getWindowHeight() - 100, getWindowWidth() - 50, getWindowHeight() - 100 + 20));
}

void BrainGrabber::setup()
{
    mCamera.lookAt( vec3(0,0,mCameraZ), vec3(0), vec3(0,1,0) );
}

void BrainGrabber::openFileDialog()
{
    fs::path pth = ci::app::getOpenFilePath( getAssetPath("") );
    if( !pth.empty() ){
        
        mSliceDataList.clear();
        
        int i=0;
        fs::path rootPath = pth.parent_path();
        for( fs::directory_iterator it(rootPath); it != fs::directory_iterator(); ++it)
        {
            fs::path pp = *it;
			
//			console() << "about to load: " << pp << endl;
			
            if(pp.extension() == ".DS_Store"){
                continue;
            }
            
            vec2 maxSize(600.0, 600.0);
            Surface sf( loadImage(*it) );
            
            vec2 sizeDiff = maxSize / (vec2)sf.getSize();
            float ratio = min(sizeDiff.x, sizeDiff.y);
            vec2 newSize = (vec2)sf.getSize() * vec2(ratio);
            
            // save downsampled versions
            Surface rSurf( newSize.x, newSize.y, false );
            ip::resize(sf, &rSurf);
            
//            mSurfList.push_back( rSurf );
            
            SliceData sd;
            sd.uuid = i;
            sd.bNeedsRecalc = true;
            sd.mSurface = rSurf;
            
            mSliceDataList.push_back( sd );
            
            ++i;
//            ++it;
//            ++it;
            
//            if( mSliceDataList.size() > 100){
//                break;
//            }
        }
        
    }else{
        console() << "Cancel" << endl;
    }

}

void BrainGrabber::update()
{
    mCurSliceNum = floor( mSlider->getPosition() * (float)(mSliceDataList.size()-1) );
    
    // If any settings have changed, mark all slices to redo calculations
    if( mLastTolerance != mColorTolerance || mLastColor != mColToMatch ){
        for( int i=0; i<mSliceDataList.size(); i++ ){
            mSliceDataList[i].bNeedsRecalc = true;
        }
    }
    
    // If anything needs recalculating, do it now
    if( mSliceDataList.size() && mSliceDataList[mCurSliceNum].bNeedsRecalc ){
        findContours();
        mSliceDataList[mCurSliceNum].bNeedsRecalc = false;
    }
    
    // 3D ----------
    mCamera.lookAt( vec3(0,0,mCameraZ), vec3(0), vec3(0,1,0) );
}

void BrainGrabber::findContours()
{
    Surface curSurf = mSliceDataList[mCurSliceNum].mSurface;
    cv::Mat input = toOcv( Channel(curSurf) );
    
    // transform the material to find the specified color
    vec3 hsvCol = rgbToHsv( mColToMatch );
    
    // TO HSL
    vec3 lBv = vec3( hsvCol.r, hsvCol.g, max(0.0f, hsvCol.b - mColorTolerance) );
    vec3 uBv = vec3( hsvCol.r, hsvCol.g, min(1.0f, hsvCol.b + mColorTolerance) );
    
    // BACK TO RGB
    lB = hsvToRgb( lBv );
    uB = hsvToRgb( uBv );
    
    cv::Scalar lowerBounds(lB.r * 255.0, lB.g * 255.0, lB.b * 255.0);
    cv::Scalar upperBounds(uB.r * 255.0, uB.g * 255.0, uB.b * 255.0);
    cv::inRange(input, lowerBounds, upperBounds, input);
    
    mDebugTex = gl::Texture::create( fromOcv(input) );
    
    // find outlines

    mSliceDataList[mCurSliceNum].mContourList.clear();
    
    cv::findContours(input, mContours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
//    cv::findContours(input, mContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    for (ContourVector::const_iterator iter = mContours.begin(); iter != mContours.end(); ++iter)
    {
        gl::VertBatchRef tVertBatch = gl::VertBatch::create(GL_LINE_LOOP);
        for (vector<cv::Point>::const_iterator pt = iter->begin(); pt != iter->end(); ++pt) {
            vec2 p( fromOcv(*pt) );
            
            tVertBatch->color(1,0,0);
            tVertBatch->vertex(p - ((vec2)curSurf.getSize() * vec2(0.5)) );
        }
        tVertBatch->end();
        
        mSliceDataList[mCurSliceNum].mContourList.push_back( tVertBatch );
    }
}

void BrainGrabber::draw()
{
    gl::color(1,1,1,1);
    
    if( mCurrentViewMode == BRAIN_VIEW_MODE::MODE_2D ){
        draw2D();
    }else if( mCurrentViewMode == BRAIN_VIEW_MODE::MODE_3D ){
        draw3D();
    }
    
    pretzel::PretzelGui::drawAll();
}

void BrainGrabber::draw2D()
{
    // DEBUG IMAGE
    if(mDebugTex){
        gl::color( Color::white() );
        gl::draw(mDebugTex, vec2(250, 0) );
    }
    
    // DEBUG COLOR RECTS
    gl::pushMatrices();{
        gl::translate( vec2(250, 0) );
        gl::translate(0, 600);
        gl::color( mColToMatch );
        gl::drawSolidRect( Rectf(0,0,50,50) );
        gl::color( lB );
        gl::drawSolidRect( Rectf(50,0,100,50) );
        gl::color( uB );
        gl::drawSolidRect( Rectf(100,0,150,50) );
        gl::color( Color::white() );
    }gl::popMatrices();
    
    // CONTOURS
    if( mSliceDataList.size() ){
        SliceData sd = mSliceDataList[mCurSliceNum];
        
        gl::pushMatrices();{
            gl::translate( vec2(250, 0) );
            gl::translate( (vec2)sd.mSurface.getSize() * (vec2)0.5 );
            
            if( sd.mContourList.size() ){
                gl::ScopedColor scCol( 1,0,1,1 );
//                gl::draw( mContourList[mCurImageNum] );
                
                for( int i=0; i<sd.mContourList.size(); ++i){
                    if( !sd.mContourList[i]->empty() ){
                        sd.mContourList[i]->draw();
                    }
                }
            }
        }gl::popMatrices();
    }
	
    pretzel::PretzelGui::drawAll();
	mSlider->draw();
	

	gl::drawString("Current Slice: " + to_string(mCurSliceNum) + " / " + to_string(mSliceDataList.size()), vec2(300, getWindowHeight() - 70));

}

void BrainGrabber::saveToFile(const string text, const string location) {
	std::ofstream oStream(location);
	oStream << text;
	oStream.close();
}

void BrainGrabber::draw3D()
{
    gl::pushMatrices();{
    
        gl::setMatrices( mCamera );
        
        for( int i=0; i<mSliceDataList.size(); i++){
            for( int k=0; k<mSliceDataList[i].mContourList.size(); k++){
                mSliceDataList[i].mContourList[k]->draw();
            }
        }
    
    }gl::popMatrices();
}



