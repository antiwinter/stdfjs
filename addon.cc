#include <libstdf.h>
#include <nan.h>

void Parse(const Nan::FunctionCallbackInfo<v8::Value> &info)
{

  if (info.Length() != 2)
  {
    Nan::ThrowTypeError("Invoke w/ stdf.parse(file, callback(rec))");
    return;
  }

  Nan::Utf8String fp(info[0]->ToString());
  v8::Local<v8::Function> cb = info[1].As<v8::Function>();

  stdf_file *f;
  char *recname;
  rec_unknown *rec;
  int i;
  dtc_U4 stdf_ver;

  printf("%s::%d\n", __func__, __LINE__);

  f = stdf_open(*fp);
  if (!f)
  {
    printf("fp is %s\n", *fp);
    Nan::ThrowTypeError("Could not open file");
    printf("%s::%d\n", __func__, __LINE__);
  }
  printf("%s::%d\n", __func__, __LINE__);

  stdf_get_setting(f, STDF_SETTING_VERSION, &stdf_ver);
  printf("%s::%d\n", __func__, __LINE__);
  while ((rec = stdf_read_record(f)) != NULL)
  {
    printf("rec is %p\n", rec);

    recname = stdf_get_rec_name(rec->header.REC_TYP, rec->header.REC_SUB);
    printf("Record %s (%3i,%3i) %i bytes:\n", recname, rec->header.REC_TYP,
           rec->header.REC_SUB, rec->header.REC_LEN);

    const unsigned argc = 1;
    v8::Local<v8::Value> argv[argc] = {Nan::New(recname).ToLocalChecked()};

    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, argc, argv);
  }
}

void Init(v8::Local<v8::Object> exports)
{
  exports->Set(Nan::New("parse").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(Parse)->GetFunction());
}

NODE_MODULE(addon, Init)
