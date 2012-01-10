#define EV_MULTIPLICITY 1
#define EV_EMBED_ENABLE 0
#define EV_FORK_ENABLE 0

#include "ofxNodejs.h"

#include <uv.h>
#include <ev.h>
#include <eio.h>

namespace node
{
extern v8::Persistent<v8::Context> context;
extern int Start(int argc, char *argv[]);
extern void Stop();
}

namespace ofxNodejs
{

static uv_prepare_t prepare_watcher;

static void node_init(uv_prepare_t *watcher, int revents)
{
	v8::HandleScope scope;
	assert(watcher == &prepare_watcher);

	uv_prepare_stop(watcher);
}

class NodeEventListener
{
public:

	void onUpdate(ofEventArgs&)
	{
		ev_run(uv_default_loop()->ev, EVRUN_NOWAIT);
	}

	void onExit(ofEventArgs&)
	{
		node::Stop();
	}

};

static NodeEventListener listener;
static vector<string> paths;

void addNodePath(string path)
{
	paths.push_back(path);
}

static bool inited = false;

static void initNode()
{
	if (inited) return;
	inited = true;

	//
	// setup node.js
	//

	const char *argv[] = { "node", "" };
	int argc = 1;

	string NODE_PATH = ofToDataPath("", true) + ";";
	const char *NODE_PATH_cstr = getenv("NODE_PATH");
	
	if (NODE_PATH_cstr)
	{
		NODE_PATH += string(NODE_PATH_cstr);
	}
	
	setenv("NODE_PATH", NODE_PATH.c_str(), 1);
	setenv("NODE_DISABLE_COLORS", "1", 1);
	setenv("NODE_NO_READLINE", "1", 1);
	
	cout << "NODE_PATH::" << getenv("NODE_PATH") << endl;

	uv_prepare_init(uv_default_loop(), &prepare_watcher);
	uv_prepare_start(&prepare_watcher, node_init);

	node::Start(argc, (char**)argv);

	//
	// setup of events
	//

	ofAddListener(ofEvents.update, &listener, &NodeEventListener::onUpdate);
	ofAddListener(ofEvents.exit, &listener, &NodeEventListener::onExit);
}

static const char*ToCString(const v8::String::Utf8Value& value)
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

Object $(string source, string source_name)
{
	initNode();

	v8::HandleScope scope;
	return ExecuteString(v8::String::New(source.c_str()),
						 v8::String::NewSymbol(source_name.c_str()));
}

Object $$(string path)
{
	initNode();
	
	ofBuffer buffer;

	if (path.substr(0, 7) == "http://")
		buffer = ofLoadURL(path).data;
	else
		buffer = ofBufferFromFile(path);

	return $(buffer.getText(), path);
}

Function $f(string funcname, v8::InvocationCallback function)
{
	initNode();
	
	v8::HandleScope scope;
	v8::Local<v8::Object> global = node::context->Global();
	
	v8::Local<v8::Function> func = v8::FunctionTemplate::New(function)->GetFunction();
	global->Set(v8::String::NewSymbol(funcname.c_str()), func);
	
	return Function(func);
}

}