#include "faster_mri.h"

VALUE Faster = Qnil;
VALUE Faster_Array = Qnil;

static VALUE Int_allocate(VALUE klass)
{
  LongArray *array;
  return Data_Make_Struct(klass, LongArray, NULL, LongArray_delete, array);
}

static VALUE Int_initialize(VALUE self, VALUE items)
{
  LongArray *array;
  long items_length;
  long i;

  Data_Get_Struct(self, LongArray, array);

  Check_Type(items, T_ARRAY);
  items_length = RARRAY_LEN(items);

  // resize _might_ fail, and leave it unmodified
  LongArray_resize(array, items_length);

  if (items_length < array->capacity)
  {
    array->length = items_length;
  }
  else
  {
    array->length = array->capacity;
  }

  // copy the items that'll fit
  for (i = 0; i < array->length; i++)
  {
    array->items[i] = NUM2LONG(RARRAY_PTR(items)[i]);
  }

  return self;
}

static VALUE Int_initialize_copy(VALUE rbSelf, VALUE rbOld)
{
  LongArray *self, *old;

  Data_Get_Struct(rbSelf, LongArray, self);
  Data_Get_Struct(rbOld, LongArray, old);

  if (LongArray_resize(self, old->length))
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
  LongArray *array;
  long index;

  Data_Get_Struct(self, LongArray, array);

  Check_Type(rbIndex, T_FIXNUM);
  index = NUM2LONG(rbIndex);

  if (index >= 0)
  {
    if (index < array->length)
    {
      return LONG2NUM(array->items[index]);
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
  LongArray *array;
  Data_Get_Struct(self, LongArray, array);

  return LONG2NUM(array->length);
}

static VALUE Int_empty(VALUE self)
{
  LongArray *array;
  Data_Get_Struct(self, LongArray, array);

  return array->length > 0 ? Qfalse : Qtrue;
}

static VALUE Int_last(VALUE self)
{
  LongArray *array;
  Data_Get_Struct(self, LongArray, array);

  if (array->length > 0)
  {
    return LONG2NUM(array->items[array->length - 1]);
  }
  else
  {
    return Qnil;
  }
}

static VALUE Int_append(VALUE self, VALUE rbItem)
{
  LongArray *array;

  Check_Type(rbItem, T_FIXNUM);
  Data_Get_Struct(self, LongArray, array);

  // resize _might_ fail, and leave it unmodified
  if (LongArray_resize(array, array->length + 1))
  {
    array->items[array->length++] = NUM2LONG(rbItem);
  }
  else
  {
    // error
  }

  return self;
}

static VALUE Int_insert(VALUE self, VALUE rbIndex, VALUE rbItem)
{
  LongArray *array;
  long index;

  Data_Get_Struct(self, LongArray, array);
  Check_Type(rbItem, T_FIXNUM);
  Check_Type(rbIndex, T_FIXNUM);

  index = NUM2LONG(rbIndex);

  if (index >= 0 && index <= array->length)
  {
    // resize _might_ fail, and leave it unmodified
    if (LongArray_resize(array, array->length + 1))
    {
      memmove(array->items + index + 1, array->items + index, (array->length - index) * sizeof(long));
      array->length++;
      array->items[index] = NUM2LONG(rbItem);
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
  LongArray *array;
  long index;
  VALUE result = Qnil;

  Data_Get_Struct(self, LongArray, array);
  Check_Type(rbIndex, T_FIXNUM);

  index = NUM2LONG(rbIndex);

  // resize _might_ fail, and leave it unmodified
  if (index >= 0 && index < array->length)
  {
    result = LONG2NUM(array->items[index]);
    memmove(array->items + index, array->items + index + 1, (array->length - index - 1) * sizeof(long));
    array->length--;
    LongArray_resize(array, array->length);
  }

  return result;
}

static VALUE Int_binary_search_ge(VALUE self, VALUE search_value)
{
  LongArray *array;

  Data_Get_Struct(self, LongArray, array);
  Check_Type(search_value, T_FIXNUM);

  return LONG2NUM(LongArray_binary_search_ge(array, NUM2LONG(search_value)));
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