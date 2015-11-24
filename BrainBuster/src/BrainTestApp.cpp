#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BrainGrabber.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BrainTestApp : public App {
  public:
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;
    
    BrainGrabber    mBrain;
};

void BrainTestApp::setup()
{
    mBrain.setup();
}

void BrainTestApp::keyDown( KeyEvent event )
{
    if( event.getChar() == 'o' && event.isAccelDown() ){
        mBrain.openFileDialog();
    }
    else if( event.getChar() == 'r' && event.isAccelDown() ){
        mBrain.reloadShader();
    }
}

void BrainTestApp::update()
{
    mBrain.update();
}

void BrainTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    mBrain.draw();
}

CINDER_APP( BrainTestApp, RendererGl, [&](BrainTestApp::Settings *settings){
    settings->setWindowSize( 1024, 768 );
})
