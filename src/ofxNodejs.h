#pragma once

#include "ofMain.h"

#include "v8.h"

#include "node.h"
#include "node_object_wrap.h"

#include "ofxNodejsObject.h"
#include "ofxNodejsFunction.h"

namespace ofxNodejs
{

	void appendNodePath(string path);
	vector<string> getNodePath();
	
	Object eval(const string& source, const string& source_name = "<string>");
	inline Object $(const string& source, const string& source_name = "<string>") { return eval(source, source_name); }
	
	Object execFile(const string& path);
	inline Object $$(const string& path) { return execFile(path); }

	Function registerFunc(string funcname, v8::InvocationCallback function);
	inline Function $f(string funcname, v8::InvocationCallback function) { return registerFunc(funcname, function); }

}