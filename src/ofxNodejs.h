#pragma once

#include "ofMain.h"

#include "v8.h"

#include "node.h"
#include "node_object_wrap.h"

#include "ofxNodejsObject.h"
#include "ofxNodejsFunction.h"

namespace ofxNodejs
{

Object $(string source, string source_name = "<string>");
Object $$(string path);

Function $f(string funcname, v8::InvocationCallback function);

}