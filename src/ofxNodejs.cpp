#include "ofxNodejs.h"

#include "uv.h"
#include "v8_typed_array.h"

using namespace v8;

namespace node
{
	extern char** Init(int argc, char *argv[]);
	extern Handle<Object> SetupProcessObject(int argc, char *argv[]);
	extern void Load(Handle<Object> process_l);
	extern void EmitExit(v8::Handle<v8::Object> process_l);
	extern void RunAtExit();
}

namespace ofxNodejs
{
static v8::Context::Scope *context_scope;
static v8::Persistent<v8::Context> context;
static v8::Handle<v8::Object> process_l;

class NodeEventListener
{
public:

	void onUpdate(ofEventArgs&)
	{
		uv_run(uv_default_loop(), UV_RUN_NOWAIT);
	}

	void onExit(ofEventArgs&)
	{
		{
			v8::HandleScope handle_scope;
			
			node::EmitExit(process_l);
			node::RunAtExit();
		}
		context.Dispose();
		
		delete context_scope;
		
		v8::V8::Dispose();
	}
};

static NodeEventListener listener;
static vector<string> paths;

void appendNodePath(string path)
{
	paths.push_back(path);
}
	
vector<string> getNodePath()
{
	return paths;
}
	
static void registerBindings();

static void initNode()
{
	static bool inited = false;
	if (inited) return;
	inited = true;

	//
	// setup node.js
	//

	char *argv[] = { "node", "" };
	int argc = 1;

	paths.push_back(ofToDataPath("", true));
	
	const char *NODE_PATH_cstr = getenv("NODE_PATH");
	if (NODE_PATH_cstr)
	{
		paths.push_back(NODE_PATH_cstr);
	}
	
	string NODE_PATH;
	for (int i = 0; i < paths.size(); i++)
	{
#ifdef TARGET_WIN32
		NODE_PATH += paths[i] + ";";
#else
		NODE_PATH += paths[i] + ":";
#endif
	}
	
	setenv("NODE_PATH", NODE_PATH.c_str(), 1);
	setenv("NODE_DISABLE_COLORS", "1", 1);
	setenv("NODE_NO_READLINE", "1", 1);

	{
		using namespace node;
		
		Init(argc, argv);
		
		V8::Initialize();
		
		v8::HandleScope handle_scope;
		
		context = Context::New();
		context_scope = new Context::Scope(context);

		process_l = node::SetupProcessObject(argc, argv);
		v8_typed_array::AttachBindings(context->Global());
		
		Load(process_l);
		
		uv_run(uv_default_loop(), UV_RUN_NOWAIT);
	}

	//
	// setup of events
	//

	ofAddListener(ofEvents().update, &listener, &NodeEventListener::onUpdate);
	ofAddListener(ofEvents().exit, &listener, &NodeEventListener::onExit);
	
	registerBindings();
}

static const char* ToCString(const v8::String::Utf8Value& value)
{
	return *value ? *value : "<string conversion failed>";
}

void ReportException(v8::TryCatch* try_catch)
{
	v8::HandleScope handle_scope;
	v8::String::Utf8Value exception(try_catch->Exception());
	const char* exception_string = ToCString(exception);
	v8::Handle<v8::Message> message = try_catch->Message();
	if (message.IsEmpty())
	{
		// V8 didn't provide any extra information about this error; just
		// print the exception.
		printf("%s\n", exception_string);
	}
	else
	{
		// Print (filename):(line number): (message).
		v8::String::Utf8Value filename(message->GetScriptResourceName());
		const char* filename_string = ToCString(filename);
		int linenum = message->GetLineNumber();
		printf("%s:%i: %s\n", filename_string, linenum, exception_string);
		// Print line of source code.
		v8::String::Utf8Value sourceline(message->GetSourceLine());
		const char* sourceline_string = ToCString(sourceline);
		printf("%s\n", sourceline_string);
		// Print wavy underline (GetUnderline is deprecated).
		int start = message->GetStartColumn();
		for (int i = 0; i < start; i++)
		{
			printf(" ");
		}
		int end = message->GetEndColumn();
		for (int i = start; i < end; i++)
		{
			printf("^");
		}
		printf("\n");
		v8::String::Utf8Value stack_trace(try_catch->StackTrace());
		if (stack_trace.length() > 0)
		{
			const char* stack_trace_string = ToCString(stack_trace);
			printf("%s\n", stack_trace_string);
		}
	}
}

static v8::Handle<v8::Value> ExecuteString(v8::Handle<v8::String> source, v8::Handle<v8::Value> name)
{
	v8::HandleScope handle_scope;
	v8::TryCatch try_catch;
	v8::Handle<v8::Script> script = v8::Script::Compile(source, name);

	if (script.IsEmpty())
	{
		// Print errors that happened during compilation.
		ReportException(&try_catch);
		return v8::Undefined();
	}
	else
	{
		v8::Handle<v8::Value> result = script->Run();
		if (result.IsEmpty())
		{
			assert(try_catch.HasCaught());
			// Print errors that happened during execution.
			ReportException(&try_catch);
			return v8::Undefined();
		}
		else
		{
			assert(!try_catch.HasCaught());
			if (!result->IsUndefined())
			{
				return result;
			}
			return v8::Undefined();
		}
	}
}

//
	
Object eval(const string& source, const string& source_name)
{
	initNode();

	v8::HandleScope scope;
	return ExecuteString(v8::String::New(source.c_str()),
						 v8::String::NewSymbol(source_name.c_str()));
}

Object execFile(const string& path)
{
	initNode();
	
	ofBuffer buffer;

	if (path.substr(0, 7) == "http://")
		buffer = ofLoadURL(path).data;
	else
		buffer = ofBufferFromFile(path);

	return $(buffer.getText(), path);
}

Function registerFunc(string funcname, v8::InvocationCallback function)
{
	initNode();
	
	v8::HandleScope scope;
	v8::Local<v8::Object> global = context->Global();
	
	v8::Local<v8::Function> func = v8::FunctionTemplate::New(function)->GetFunction();
	global->Set(v8::String::NewSymbol(funcname.c_str()), func);
	
	return Function(func);
}

// bindings

static v8::Handle<v8::Value> _ofToDataPath(const v8::Arguments& args)
{
	string path = ofToDataPath(Object(args[0]).as<string>(), true);
	return Object(path);
}

static v8::Handle<v8::Value> _ofGetElapsedTimef(const v8::Arguments& args)
{
	return Object(ofGetElapsedTimef());
}

static v8::Handle<v8::Value> _ofGetFrameNum(const v8::Arguments& args)
{
	return Object(ofGetFrameNum());
}

static v8::Handle<v8::Value> _ofNoise(const v8::Arguments& args)
{
	int n = args.Length();
	float result = 0;
	
	if (n == 1)
	{
		result = ofNoise(Object(args[0]).as<float>());
	}
	else if (n == 2)
	{
		result = ofNoise(Object(args[0]).as<float>(),
						 Object(args[1]).as<float>());
	}
	else if (n == 3)
	{
		result = ofNoise(Object(args[0]).as<float>(),
						 Object(args[1]).as<float>(),
						 Object(args[2]).as<float>());
	}
	else if (n == 4)
	{
		result = ofNoise(Object(args[0]).as<float>(),
						 Object(args[1]).as<float>(),
						 Object(args[2]).as<float>(),
						 Object(args[3]).as<float>());
	}
	else
	{
		return v8::Null();
	}
	
	return Object(result);
}

static v8::Handle<v8::Value> _ofSignedNoise(const v8::Arguments& args)
{
	int n = args.Length();
	float result = 0;
	
	if (n == 1)
	{
		result = ofSignedNoise(Object(args[0]).as<float>());
	}
	else if (n == 2)
	{
		result = ofSignedNoise(Object(args[0]).as<float>(),
							   Object(args[1]).as<float>());
	}
	else if (n == 3)
	{
		result = ofSignedNoise(Object(args[0]).as<float>(),
							   Object(args[1]).as<float>(),
							   Object(args[2]).as<float>());
	}
	else if (n == 4)
	{
		result = ofSignedNoise(Object(args[0]).as<float>(),
							   Object(args[1]).as<float>(),
							   Object(args[2]).as<float>(),
							   Object(args[3]).as<float>());
	}
	else
	{
		return v8::Null();
	}
	
	return Object(result);
}

static v8::Handle<v8::Value> _ofRandom(const v8::Arguments& args)
{
	int n = args.Length();
	float result = 0;

	if (n == 1)
	{
		result = ofRandom(Object(args[0]).as<float>());
	}
	else if (n == 2)
	{
		result = ofRandom(Object(args[0]).as<float>(),
						  Object(args[1]).as<float>());
	}
	else
	{
		return v8::Null();
	}

	return Object(result);
}

static v8::Handle<v8::Value> _ofGetMouseX(const v8::Arguments& args)
{
	int n = args.Length();
	float result = 0;
	
	if (n == 0)
	{
		result = ofGetMouseX();
	}
	else
	{
		return v8::Null();
	}
	
	return Object(result);
}

static v8::Handle<v8::Value> _ofGetMouseY(const v8::Arguments& args)
{
	int n = args.Length();
	float result = 0;
	
	if (n == 0)
	{
		result = ofGetMouseY();
	}
	else
	{
		return v8::Null();
	}
	
	return Object(result);
}


static void registerBindings()
{
	registerFunc("ofToDataPath", _ofToDataPath);
	registerFunc("ofGetElapsedTimef", _ofGetElapsedTimef);
	registerFunc("ofGetFrameNum", _ofGetFrameNum);
	registerFunc("ofNoise", _ofNoise);
	registerFunc("ofSignedNoise", _ofSignedNoise);
	registerFunc("ofRandom", _ofRandom);
	registerFunc("ofGetMouseX", _ofGetMouseX);
	registerFunc("ofGetMouseY", _ofGetMouseY);
}

}