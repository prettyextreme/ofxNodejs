#include "ofxNodejsObject.h"

namespace ofxNodejs
{

Object::Object(Array o)
{
	v8::HandleScope scope;
	v8::Local<v8::Array> a = v8::Local<v8::Array>(v8::Array::New(o.size()));
	
	for (int i = 0; i < o.size(); i++)
	{
		v8::Handle<v8::Value> p = o[i];
		a->Set(i, p);
	}
	
	v = a;
}

}