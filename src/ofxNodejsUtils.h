#pragma once

#include "ofMain.h"

#include "v8.h"

namespace ofxNodejs {
	
	const char* ToCString(const v8::String::Utf8Value& value);
	
	void ReportException(v8::TryCatch* try_catch);
	
	v8::Handle<v8::Value> ExecuteString(v8::Handle<v8::String> source, v8::Handle<v8::Value> name);
	
}
