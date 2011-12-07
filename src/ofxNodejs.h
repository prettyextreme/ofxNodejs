#pragma once

#include "ofMain.h"

#include "v8.h"

#include "ofxNodejsObject.h"
#include "ofxNodejsFunction.h"

namespace ofxNodejs
{

Object $(string source, string source_name = "<string>");
Object $$(string path);
	
}