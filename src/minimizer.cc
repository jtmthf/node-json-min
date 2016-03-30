#include <nan.h>

using namespace v8;

// A worker class extending the NanAsyncWorker helper
// class, a simple encapsulation of worker-thread
// logic to make simple tasks easier

class MyWorker : public Nan::AsyncWorker {
 public:
  // Constructor
  MyWorker(Nan::Callback *callback, char* istr)
    : Nan::AsyncWorker(callback) {
      str = static_cast<char*>(malloc(strlen(istr)+1));
      strcpy(str, istr);
    }
  // Destructor
  ~MyWorker() {
    free(str);
  }

  // Executed inside the worker-thread.
  // It is not safe to access V8, or V8 data structures
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute () {
    // Asynchronous, non-V8 work goes here
    int len = strlen(str);
    int state = 0;
    char c;
    int j = 0;
    for (int i = 0; i < len; i++) {
      c = str[i];
      switch (state) {
        case 0:
        if (c == '\"') {
          state = 1;
          str[j++] = c;
        }
        else if (c == '/') {
          state = 3;
        }
        else if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
          str[j++] = c;
        }
        break;
        case 1:
        if (c == '\\') {
          state = 2;
          str[j++] = c;
        }
        else if (c == '\"') {
          state = 0;
          str[j++] = c;
        } else {
          str[j++] = c;
        }
        break;
        case 2:
        state = 1;
        str[j++] = c;
        break;
        case 3:
        if (c == '/') {
          state = 4;
        } else if (c == '*') {
          state = 5;
        } else {
          state = 0;
          str[j++] = '/';
          str[j++] = c;
        }
        break;
        case 4:
        if (c == '\n') {
          state = 0;
        }
        break;
        case 5:
        if (c == '*') {
          state = 6;
        }
        break;
        case 6:
        if (c == '/') {
          state = 0;
        }
        else if (c != '*') {
          state = 5;
        }
        break;
      }
    }
    str[j] = '\0';
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback () {
    Nan::EscapableHandleScope scope;
    Nan::MaybeLocal<String> minimized = Nan::New<String>(str);
    v8::Local<String> localStr;
    if (minimized.ToLocal(&localStr) == false) {
      Nan::ThrowError("Minimized result could not be converted to string");
    }
    v8::Local<v8::Value> argv[] = {localStr};
    // Nan::Callback#Call() does a Nan::MakeCallback() for us
    callback->Call(1, argv);
  }

 private:
  char* str;
};

NAN_METHOD(Minify) {
  // get string and callback
  // create NanCallback instance wrapping the callback
  // create a MyWorker instance, passing the callback and string
  // queue the worker instance onto the thread-pool
  Nan::MaybeLocal<String> maybeStr = Nan::To<String>(info[0]);
  v8::Local<String> str;

  if (maybeStr.ToLocal(&str) == false) {
    Nan::ThrowError("Error converting first argument to string");
  }


  if (info[1]->IsFunction() == false) {
    Nan::ThrowError("Error converting second argument to function");
  }

  Local<Function> callback = info[1].As<Function>();

  Nan::Callback* nanCallback = new Nan::Callback(callback);
  MyWorker* worker = new MyWorker(nanCallback, *String::Utf8Value(str));
  Nan::AsyncQueueWorker(worker);
}

NAN_MODULE_INIT(Init) {
  Nan::Set(target, Nan::New("minify").ToLocalChecked(),
      Nan::GetFunction(Nan::New<FunctionTemplate>(Minify)).ToLocalChecked());
}

NODE_MODULE(myaddon, Init)
