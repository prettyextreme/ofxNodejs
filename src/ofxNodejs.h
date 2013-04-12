#pragma once

#include "ofMain.h"

#include "v8.h"

#include "node.h"
#include "node_object_wrap.h"

#include "ofxNodejsObject.h"
#include "ofxNodejsFunction.h"

namespace ofxNodejs
{

	void init(string path = "node_modules");
	
	Object eval(const string& source, const string& source_name = "<string>");
	inline Object $(const string& source, const string& source_name = "<string>") { return eval(source, source_name); }
	
	Object run(const string& path);
	inline Object $$(const string& path) { return run(path); }

	Function registerFunc(string funcname, v8::InvocationCallback function);

}