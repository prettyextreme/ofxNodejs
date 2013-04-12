#pragma once

#include "ofMain.h"

#include "v8.h"
#include "javascript-external-runtime.h"

namespace ofxNodejs
{
	
	template <typename T>
	swig_type_info* type2swig_type_info()
	{
		ofLogError("ofxNodejs") << "unknown type";
		return NULL;
	}

#define register_swig_type_info(type) \
template <> \
inline swig_type_info* type2swig_type_info<type>() { \
	static swig_type_info *t = SWIG_TypeQuery(#type"*"); \
	return t; \
}
	
	register_swig_type_info(char);
	register_swig_type_info(short);
	register_swig_type_info(int);
	register_swig_type_info(long);
	register_swig_type_info(unsigned char);
	register_swig_type_info(unsigned short);
	register_swig_type_info(unsigned int);
	register_swig_type_info(unsigned long);
	
	register_swig_type_info(float);
	register_swig_type_info(double);
	
	register_swig_type_info(ofColor);
	register_swig_type_info(ofFloatColor);
	register_swig_type_info(ofShortColor);
	
	register_swig_type_info(ofPixels);
	register_swig_type_info(ofFloatPixels);
	register_swig_type_info(ofShortPixels);
	
	register_swig_type_info(ofImage);
	register_swig_type_info(ofFloatImage);
	register_swig_type_info(ofShortImage);
	
	register_swig_type_info(ofBuffer);
	register_swig_type_info(ofDirectory);
	register_swig_type_info(ofFile);
	
	register_swig_type_info(ofVec2f);
	register_swig_type_info(ofVec3f);
	register_swig_type_info(ofVec4f);
	register_swig_type_info(ofQuaternion);
	register_swig_type_info(ofMatrix3x3);
	register_swig_type_info(ofMatrix4x4);
	register_swig_type_info(ofRectangle);
	
	register_swig_type_info(ofLight);
	register_swig_type_info(ofMaterial);
	
	register_swig_type_info(ofCamera);
	register_swig_type_info(ofEasyCam);
	register_swig_type_info(ofMesh);
	register_swig_type_info(ofNode);
	register_swig_type_info(ofPath);
	
	register_swig_type_info(ofFbo);
	register_swig_type_info(ofVbo);
	register_swig_type_info(ofVboMesh);
	register_swig_type_info(ofShader);
	
	register_swig_type_info(ofTexture);
	register_swig_type_info(ofTextureData);
	
	register_swig_type_info(ofPolyline);
	
	register_swig_type_info(ofStyle);
	register_swig_type_info(ofTrueTypeFont);
	register_swig_type_info(ofURLFileLoader);
	
	register_swig_type_info(ofVideoGrabber);
	register_swig_type_info(ofVideoPlayer);
	register_swig_type_info(ofSoundPlayer);
}