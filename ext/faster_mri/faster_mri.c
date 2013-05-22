#include <ruby.h>

VALUE Faster = Qnil;
VALUE Faster_Array = Qnil;

void Init_faster_mri(void)
{
  Faster = rb_define_module("Faster");
  Faster_Array = rb_define_module_under(Faster, "Array");
}