#pragma once

#include "ofMain.h"

#include "v8.h"

namespace ofxNodejs
{

class Function
{
	string funcname;

public:

	Function() {}
	Function(string name) { bind(name); }
	virtual ~Function() { unbind(); }

	void bind(string funcname_);
	void unbind();

	virtual v8::Handle<v8::Value> exec(const v8::Arguments& args);
	

	static map<string, Function*> callback_map;
	static v8::Handle<v8::Value> callback(const v8::Arguments& args);

};

}