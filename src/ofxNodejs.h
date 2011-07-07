#pragma once

#include "ofMain.h"

#include "v8.h"

namespace ofxNodejs
{

class Value {
public:

	Value(v8::Local<v8::Value> value) : _value(value) {}
	
	bool isUndefined() { return _value->IsUndefined(); }
	bool isNull() { return _value->IsNull(); }
	bool isTrue() { return _value->IsTrue(); }
	bool isFalse() { return _value->IsFalse(); }
	bool isString() { return _value->IsString(); }
	bool isFunction() { return _value->IsFunction(); }
	bool isArray() { return _value->IsArray(); }
	bool isObject() { return _value->IsObject(); }
	bool isNumber() { return _value->IsNumber(); }
	bool isInt32() { return _value->IsInt32(); }
	bool isUint32() { return _value->IsUint32(); }
	bool isData() { return _value->IsDate(); }
	
	bool toBoolean() { return _value->BooleanValue(); }
	int32_t toInt32() { return _value->Int32Value(); }
	uint32_t toUint32() { return _value->Uint32Value(); }
	double toNumber() { return _value->NumberValue(); }
	
	string toString();
	void print();

private:
	v8::Local<v8::Value> _value;
};

void init(string startup_script);

Value $(string source, string source_name = "<string>");
Value $$(string path);

} // namespace ofxNodejs