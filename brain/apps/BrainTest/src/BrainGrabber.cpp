//
//  BrainGrabber.cpp
//  BrainTest
//
//  Created by Charlie Whitney on 11/20/15.
//
//

#include "BrainGrabber.h"

#include <fstream>

#define SCALE (1.0/600.0)

using namespace ci;
using namespace ci::app;
using namespace std;

BrainGrabber::BrainGrabber() :
mColorTolerance(0.01),
mColToMatch(1,1,1),
mCurSliceNum(0),
mCurrentViewMode( BRAIN_VIEW_MODE::MODE_2D ),
mCameraZ(3)
{
	mGui = pretzel::PretzelGui::create("SETTINGS");
	mGui->addButton("OPEN FILES", [&](void*){ openFileDialog(); }, this );
	mGui->addColorPicker("Brain Color", &mColToMatch);
	mGui->addSlider("Tolerance", &mColorTolerance, 0.0, 0.5);
	
	mGui->addLabel("VIEW MODE");
	mGui->addButton("2D MODE", [&](void*){ mCurrentViewMode = MODE_2D; }, this );
	mGui->addButton("3D MODE", [&](void*){ mCurrentViewMode = MODE_3D; recalcAll(); }, this );
	mGui->addButton("EXPORT XYZ", [&](void*){ exportXYZ(); }, this );
	
	mGui->addLabel("3D STUFF");
	mGui->addSlider("Camera Z", &mCameraZ, 0, 20);
	
	mGui->addLabel("SAVE / LOAD SETTINGS");
	mGui->addSaveLoad();
	
	mGui->loadSettings();
	
	getWindow()->getSignalResize().connect([&](){
		mGui->setPos( vec2(0) );
		mGui->setSize( vec2(250, getWindowHeight()) );
	});
	
	mSlider = new BrainSlider(ci::Rectf(250 + 50, getWindowHeight() - 100, getWindowWidth() - 50, getWindowHeight() - 100 + 20));
	
	reloadShader();
}

void BrainGrabber::setup()
{
	mCamera.lookAt( vec3(0,0,mCameraZ), vec3(0), vec3(0,1,0) );
	mContourFbo = gl::Fbo::create( getWindowWidth() - 250, getWindowHeight() );
	mArcball = Arcball( &mCamera, Sphere(vec3(0), 0.75) );
	
	getWindow()->getSignalMouseDown().connect([&](MouseEvent event){
		if (!mSlider->isDragging()) {
			mArcball.mouseDown(event);
		}
	});
	
	getWindow()->getSignalMouseDrag().connect([&](MouseEvent event){
		mArcball.mouseDrag(event);
	});
}

void BrainGrabber::reloadShader()
{
	console() << "Reload Shader" << endl;
	try {
		mSkullShader = gl::GlslProg::create( loadAsset("shaders/skull/skull.vert"), loadAsset("shaders/skull/skull.frag") );
		
		if( mParticleBatch ){
			mParticleBatch->replaceGlslProg( mSkullShader );
		}
		
		CI_LOG_I("Skull shader loaded ok");
	}catch( const std::exception &e ){
		CI_LOG_EXCEPTION( "Exception caught loading pixelation .vert / frag", e );
	}
}

void BrainGrabber::openFileDialog()
{
	fs::path pth = ci::app::getOpenFilePath( getAssetPath("") );
	if( !pth.empty() ){
		
		mSliceDataList.clear();
		mContourPoints.clear();
		
		int i=0;
		fs::path rootPath = pth.parent_path();
		for( fs::directory_iterator it(rootPath); it != fs::directory_iterator(); ++it)
		{
			fs::path pp = *it;
			
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
			
			SliceData sd;
			sd.uuid = i;
			sd.bNeedsRecalc = true;
			sd.mSurface = rSurf;
			
			mSliceDataList.push_back( sd );
			mContourPoints.push_back(vector<vec3>());
			
			findContours(i);
			
			++i;
			
			//            if( mSliceDataList.size() > 550){
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
		findContours(mCurSliceNum);
		mSliceDataList[mCurSliceNum].bNeedsRecalc = false;
	}
	
	// 3D ----------
	//    mCamera.lookAt( vec3(0,0,mCameraZ), vec3(0), vec3(0,1,0) );
	mCamera.setAspectRatio( (getWindowWidth() - 250) / getWindowHeight() );
}

void BrainGrabber::findContours( int slice )
{
	SliceData *curSlice = &mSliceDataList[slice];
	
	Surface curSurf = curSlice->mSurface;
	cv::Mat input = toOcv( Channel(curSurf) );
	
	// transform the material to find the specified color
	vec3 hsvCol = rgbToHsv( mColToMatch );
	
	// TO HSL, ADJUST BRIGHTNESS, BACK TO RGB
	lB = hsvToRgb( vec3( hsvCol.r, hsvCol.g, max(0.0f, hsvCol.b - mColorTolerance) ) );
	uB = hsvToRgb( vec3( hsvCol.r, hsvCol.g, min(1.0f, hsvCol.b + mColorTolerance) ) );
	
	cv::Scalar lowerBounds(lB.r * 255.0, lB.g * 255.0, lB.b * 255.0);
	cv::Scalar upperBounds(uB.r * 255.0, uB.g * 255.0, uB.b * 255.0);
	cv::inRange(input, lowerBounds, upperBounds, input);
	
	mDebugTex = gl::Texture::create( fromOcv(input) );
	
	// FIND OUTLINES ----------------------------------------------------------------
	mSliceDataList[slice].mVertBatchList.clear();
	mSliceDataList[slice].mVertList.clear();
	mContourPoints[slice].clear();
	
	cv::findContours(input, mContours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	
	float scale = fminf(1.0f / mSliceDataList.size(), fminf(1.0f / curSurf.getWidth(), 1.0 / curSurf.getHeight()));
	float z = (float)slice * scale;
	
	// iterate through each contour and save the points
	for (ContourVector::const_iterator iter = mContours.begin(); iter != mContours.end(); ++iter)
	{
		gl::VertBatchRef tVertBatch = gl::VertBatch::create(GL_LINE_LOOP);
		
		for (vector<cv::Point>::const_iterator pt = iter->begin(); pt != iter->end(); ++pt) {
			
			//            vec2 hImg = (vec2)curSurf.getSize() * vec2(0.5);
			vec2 p( (vec2)fromOcv(*pt) /*-hImg*/);
			p *= scale;
			
			vec3 vert = vec3(p, z);
			vert.x -= 0.5;
			
			tVertBatch->color(vert.x + 0.5, vert.y + 0.5, vert.z + 0.5);
			tVertBatch->vertex( vert );
			
			mContourPoints[slice].push_back(vert);
			mSliceDataList[slice].mVertList.push_back(vert);
		}
		tVertBatch->end();
		
		mSliceDataList[slice].mVertBatchList.push_back( tVertBatch );
	}
}

void BrainGrabber::recalcAll()
{
	mAllPoints.clear();
	
	for( int i=0; i<mSliceDataList.size(); i++){
		SliceData *sd = &mSliceDataList[i];
		
		if( sd->bNeedsRecalc ){
			findContours(i);
			sd->bNeedsRecalc = false;
		}
		
		for(int k=0; k<sd->mVertList.size(); k++){
			//            mAllPoints.color( 1,1,1 );
			//            mAllPoints.vertex( sd->mVertList[k] );
			mAllPoints.push_back( sd->mVertList[k] );
		}
	}
	
	pushVboPoints();
}

void BrainGrabber::pushVboPoints()
{
	mParticleVbo = gl::Vbo::create( GL_ARRAY_BUFFER, mAllPoints, GL_STATIC_DRAW );
	
	geom::BufferLayout particleLayout;
	particleLayout.append( geom::Attrib::POSITION, 3, sizeof( vec3 ), 0 );
	//    particleLayout.append( geom::Attrib::COLOR, 4, sizeof( Particle ), offsetof( Particle, color ) );
	
	// Create mesh by pairing our particle layout with our particle Vbo.
	// A VboMesh is an array of layout + vbo pairs
	auto mesh = gl::VboMesh::create( mAllPoints.size(), GL_POINTS, { { particleLayout, mParticleVbo } } );
	
	mParticleBatch = gl::Batch::create( mesh, mSkullShader );
	gl::pointSize( 1.0f );
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
		SliceData *sd = &mSliceDataList[mCurSliceNum];
		vec2 surfSize = sd->mSurface.getSize();
		
		gl::pushMatrices();{
			gl::translate( vec2(250, 0) );
			gl::translate( surfSize * vec2(0.5) );
			gl::scale( surfSize );
			
			if( sd->mVertBatchList.size() ){
				for( int i=0; i<sd->mVertBatchList.size(); ++i){
					if( !sd->mVertBatchList[i]->empty() ){
						sd->mVertBatchList[i]->draw();
					}
				}
			}
		}gl::popMatrices();
	}
	
	pretzel::PretzelGui::drawAll();
	mSlider->draw();
	
	gl::drawString("Current Slice: " + to_string(mCurSliceNum) + " / " + to_string(mSliceDataList.size()), vec2(300, getWindowHeight() - 70));
}

void BrainGrabber::draw3D()
{
	gl::pushMatrices();{
	gl::setMatrices( mCamera );
		
		gl::ScopedDepth scD(true);
		gl::rotate( mArcball.getQuat() );
		
		if( mParticleBatch ){
			mParticleBatch->draw();
		}
		
	}gl::popMatrices();
}

void BrainGrabber::exportXYZ()
{
	std::string str = "";
	
	if (mContourPoints.size() > 0) {
		
		for (int i = 0; i < mContourPoints.size(); ++i) {
			auto points = mContourPoints[i];
			
			if (points.size() <= 0) {
				continue;
			}
			
			for (int j = 0; j < points.size(); ++j) {
				vec3 pt = points[j];
				str += to_string(pt.x) + " " + to_string(pt.y) + " " + to_string(pt.z) + "\n";
			}
		}
		
	}
	
	saveToFile(str, getAppPath().string() + "/export.xyz");
}

void BrainGrabber::saveToFile(const string text, const string location) {
	std::ofstream oStream(location);
	oStream << text;
	oStream.close();
}



