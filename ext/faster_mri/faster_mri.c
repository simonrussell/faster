#include "faster_mri.h"

VALUE Faster = Qnil;
VALUE Faster_Array = Qnil;

static VALUE Int_allocate(VALUE klass)
{
  IntArray *int_array;
  return Data_Make_Struct(klass, IntArray, NULL, IntArray_delete, int_array);
}

static VALUE Int_initialize(VALUE self, VALUE items)
{
  IntArray *int_array;
  long items_length;
  long i;

  Data_Get_Struct(self, IntArray, int_array);

  Check_Type(items, T_ARRAY);
  items_length = RARRAY_LEN(items);

  // resize _might_ fail, and leave it unmodified
  IntArray_resize(int_array, items_length);

  if (items_length < int_array->capacity)
  {
    int_array->length = items_length;
  }
  else
  {
    int_array->length = int_array->capacity;
  }

  // copy the items that'll fit
  for (i = 0; i < int_array->length; i++)
  {
    int_array->items[i] = NUM2LONG(RARRAY_PTR(items)[i]);
  }

  return self;
}

static VALUE Int_initialize_copy(VALUE rbSelf, VALUE rbOld)
{
  IntArray *self, *old;

  Data_Get_Struct(rbSelf, IntArray, self);
  Data_Get_Struct(rbOld, IntArray, old);

  if (IntArray_resize(self, old->length))
  {
    self->length = old->length;
    memcpy(self->items, old->items, self->length * sizeof(long));
  }
  else
  {
    // TODO error
  }
}

static VALUE Int_at(VALUE self, VALUE rbIndex)
{
  IntArray *int_array;
  long index;

  Data_Get_Struct(self, IntArray, int_array);

  Check_Type(rbIndex, T_FIXNUM);
  index = NUM2LONG(rbIndex);

  if (index >= 0)
  {
    if (index < int_array->length)
    {
      return LONG2NUM(int_array->items[index]);
    }
    else
    {
      return Qnil;
    }
  }
  else
  {
    return Qnil;
  }
}

static VALUE Int_length(VALUE self)
{
  IntArray *int_array;
  Data_Get_Struct(self, IntArray, int_array);

  return LONG2NUM(int_array->length);
}

static VALUE Int_empty(VALUE self)
{
  IntArray *int_array;
  Data_Get_Struct(self, IntArray, int_array);

  return int_array->length > 0 ? Qfalse : Qtrue;
}

static VALUE Int_last(VALUE self)
{
  IntArray *int_array;
  Data_Get_Struct(self, IntArray, int_array);

  if (int_array->length > 0)
  {
    return LONG2NUM(int_array->items[int_array->length - 1]);
  }
  else
  {
    return Qnil;
  }
}

static VALUE Int_append(VALUE self, VALUE rbItem)
{
  IntArray *int_array;

  Check_Type(rbItem, T_FIXNUM);
  Data_Get_Struct(self, IntArray, int_array);

  // resize _might_ fail, and leave it unmodified
  if (IntArray_resize(int_array, int_array->length + 1))
  {
    int_array->items[int_array->length++] = NUM2LONG(rbItem);
  }
  else
  {
    // error
  }

  return self;
}

static VALUE Int_insert(VALUE self, VALUE rbIndex, VALUE rbItem)
{
  IntArray *int_array;
  long index;

  Data_Get_Struct(self, IntArray, int_array);
  Check_Type(rbItem, T_FIXNUM);
  Check_Type(rbIndex, T_FIXNUM);

  index = NUM2LONG(rbIndex);

  if (index >= 0 && index <= int_array->length)
  {
    // resize _might_ fail, and leave it unmodified
    if (IntArray_resize(int_array, int_array->length + 1))
    {
      memmove(int_array->items + index + 1, int_array->items + index, (int_array->length - index) * sizeof(long));
      int_array->length++;
      int_array->items[index] = NUM2LONG(rbItem);
    }
    else
    {
      // error
    }
  }

  return self;
}

static VALUE Int_delete_at(VALUE self, VALUE rbIndex)
{
  IntArray *int_array;
  long index;
  VALUE result = Qnil;

  Data_Get_Struct(self, IntArray, int_array);
  Check_Type(rbIndex, T_FIXNUM);

  index = NUM2LONG(rbIndex);

  // resize _might_ fail, and leave it unmodified
  if (index >= 0 && index < int_array->length)
  {
    result = LONG2NUM(int_array->items[index]);
    memmove(int_array->items + index, int_array->items + index + 1, (int_array->length - index - 1) * sizeof(long));
    int_array->length--;
    IntArray_resize(int_array, int_array->length);
  }

  return result;
}

static VALUE Int_binary_search_ge(VALUE self, VALUE search_value)
{
  IntArray *array;

  Data_Get_Struct(self, IntArray, array);
  Check_Type(search_value, T_FIXNUM);

  return LONG2NUM(IntArray_binary_search_ge(array, NUM2LONG(search_value)));
}

static void define_Faster_Array_Int(void)
{
  VALUE klass = rb_define_class_under(Faster_Array, "Int", rb_cObject);
  rb_define_alloc_func(klass, Int_allocate);
  rb_define_method(klass, "initialize", Int_initialize, 1);
  rb_define_method(klass, "initialize_copy", Int_initialize_copy, 1);
  rb_define_method(klass, "at", Int_at, 1);
  rb_define_method(klass, "[]", Int_at, 1);
  rb_define_method(klass, "length", Int_length, 0);
  rb_define_method(klass, "size", Int_length, 0);
  rb_define_method(klass, "empty?", Int_empty, 0);
  rb_define_method(klass, "last", Int_last, 0);
  rb_define_method(klass, "<<", Int_append, 1);
  rb_define_method(klass, "insert", Int_insert, 2);
  rb_define_method(klass, "delete_at", Int_delete_at, 1);
  rb_define_method(klass, "binary_search_ge", Int_binary_search_ge, 1);
}

void Init_faster_mri(void)
{
  Faster = rb_define_module("Faster");
  Faster_Array = rb_define_module_under(Faster, "Array");

  define_Faster_Array_Int();
}