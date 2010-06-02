#include "speed.h"

/*
 * create ruby bindings for
 *    pagespeed::PagespeedInput
 *    pagespeed::Resource
 *    pagespeed::Engine
 */

VALUE mSpeed;
VALUE cSpeedInput;
VALUE cSpeedResource;
VALUE cSpeedEngine;

/* it's necessary to wrap the resource object so we can know whether or not to delete it */
struct SpeedResourceWrap {
  SpeedResourceWrap () : res(new pagespeed::Resource()), owned(false) {
  }
  ~SpeedResourceWrap () {
    if (!owned) { delete res; }
  }
  pagespeed::Resource *res;
  bool owned;
};

static void ruby_speed_input_mark(pagespeed::PagespeedInput *input) {
}

static void ruby_speed_input_free(pagespeed::PagespeedInput *input) {
  delete input;
}

static VALUE ruby_speed_input_new(int argc, VALUE *argv, VALUE klass) {
  VALUE rb_input;
  pagespeed::PagespeedInput *input = new pagespeed::PagespeedInput();

  rb_input = Data_Wrap_Struct(klass, ruby_speed_input_mark, ruby_speed_input_free, input);

  return rb_input;
}

static VALUE ruby_speed_input_add(VALUE self, VALUE resource) {
  pagespeed::PagespeedInput *input;
  SpeedResourceWrap *res_wrap;
  Data_Get_Struct(self, pagespeed::PagespeedInput, input);
  Data_Get_Struct(resource, SpeedResourceWrap, res_wrap);

  input->AddResource(res_wrap->res);
  res_wrap->owned = true;

  return self;
}

static void ruby_speed_resource_mark(SpeedResourceWrap *res) {
}

static void ruby_speed_resource_free(SpeedResourceWrap *res) {
  delete res;
}

static VALUE ruby_speed_resource_new(int argc, VALUE *argv, VALUE klass) {
  VALUE rb_res, url, method;
  SpeedResourceWrap *res_wrap = new SpeedResourceWrap();

  rb_scan_args(argc, argv, "02", &url, &method);

  res_wrap->res->SetRequestUrl(std::string(RSTRING_PTR(url),RSTRING_LEN(url)));
  res_wrap->res->SetRequestMethod(std::string(RSTRING_PTR(method),RSTRING_LEN(method)));
        
  rb_res = Data_Wrap_Struct(klass, ruby_speed_resource_mark, ruby_speed_resource_free, res_wrap);

  return rb_res;
}

static void add_request_header(VALUE key, VALUE value, pagespeed::Resource *r) {
  r->AddRequestHeader(std::string(RSTRING_PTR(key),RSTRING_LEN(key)),
                      std::string(RSTRING_PTR(value),RSTRING_LEN(value)));
}

static VALUE ruby_speed_resource_add_request(VALUE self, VALUE headers) {
  SpeedResourceWrap *rw;
  Data_Get_Struct(self, SpeedResourceWrap, rw);
  pagespeed::Resource *r = rw->res;;
  rb_hash_foreach(headers, (int (*)(...))add_request_header, (VALUE)r);
  return self;
}

static VALUE ruby_speed_resource_add_response(VALUE self, VALUE status, VALUE body) {
  SpeedResourceWrap *rw;
  Data_Get_Struct(self, SpeedResourceWrap, rw);
  pagespeed::Resource *r = rw->res;;
  r->SetResponseStatusCode(FIX2LONG(status));
  r->SetResponseBody(std::string(RSTRING_PTR(body),RSTRING_LEN(body)));
  return self;
}

static void ruby_speed_engine_mark(pagespeed::Engine *engine) {
}

static void ruby_speed_engine_free(pagespeed::Engine *engine) {
  delete engine;
}


static VALUE ruby_speed_engine_new(VALUE klass, VALUE rb_rules) {
  VALUE rb_engine;
  std::vector<pagespeed::Rule*> rules;
  bool save_optimized_content = false;

  pagespeed::rule_provider::AppendAllRules(save_optimized_content, &rules);

  pagespeed::Engine *engine = new pagespeed::Engine(rules);
  engine->Init();

  rb_engine = Data_Wrap_Struct(klass, ruby_speed_engine_mark, ruby_speed_engine_free, engine);

  return rb_engine;
}

static VALUE ruby_speed_engine_compute(VALUE self, VALUE rb_input) {
  pagespeed::Engine *engine;
  pagespeed::PagespeedInput *input;
  Data_Get_Struct(self, pagespeed::Engine, engine);
  Data_Get_Struct(rb_input, pagespeed::PagespeedInput, input);

  pagespeed::formatters::TextFormatter formatter(&std::cout);
  engine->ComputeAndFormatResults(*input, &formatter);
  return Qnil;
}

exportable 
void Init_speed_core() {
  mSpeed         = rb_define_module("Speed");
  cSpeedInput    = rb_define_class_under(mSpeed, "Input", rb_cObject);
  cSpeedResource = rb_define_class_under(mSpeed, "Resource", rb_cObject);
  cSpeedEngine   = rb_define_class_under(mSpeed, "Engine", rb_cObject);

  rb_define_const(mSpeed, "ALL", rb_str_new2("ALL"));

  rb_define_singleton_method(cSpeedInput, "new", RBCALL(ruby_speed_input_new), -1);
  rb_define_method(cSpeedInput, "add", RBCALL(ruby_speed_input_add), 1);

  rb_define_singleton_method(cSpeedResource, "new", RBCALL(ruby_speed_resource_new), -1);
  rb_define_method(cSpeedResource, "add_request", RBCALL(ruby_speed_resource_add_request), 1);
  rb_define_method(cSpeedResource, "add_response", RBCALL(ruby_speed_resource_add_response), 2);

  rb_define_singleton_method(cSpeedEngine, "new", RBCALL(ruby_speed_engine_new), 1);
  rb_define_method(cSpeedEngine, "compute", RBCALL(ruby_speed_engine_compute), 1);
}
