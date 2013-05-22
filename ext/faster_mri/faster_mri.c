#include <ruby.h>

VALUE Faster = Qnil;
VALUE Faster_Array = Qnil;

typedef struct
{
  long capacity;
  long length;
  int *items;
} IntArray;

static IntArray *IntArray_new(void)
{
  IntArray *int_array = malloc(sizeof(IntArray));

  int_array->items = NULL;
  int_array->capacity = int_array->length = 0;

  return int_array;
}

static long IntArray_calc_capacity(long desired)
{
  if (desired <= 0)
  {
    return 0;
  }
  else if (desired < 512)
  {
    return 512;
  }
  else if (desired < 4096)
  {
    return 4096;
  }
  else if (desired < 32768)
  {
    return 32768;
  }
  else if (desired < 65536)
  {
    return 65536;
  }
  else
  {
    return (desired | 65535) + 1;
  }
}

static int IntArray_resize(IntArray *int_array, long new_capacity)
{
  int *new_items;

  new_capacity = IntArray_calc_capacity(new_capacity);

  if (new_capacity == int_array->capacity)
  {
    /* nothing */
    return 1;
  }
  else if (new_capacity > 0)
  {
    new_items = realloc(int_array->items, sizeof(int) * new_capacity);

    if (new_items)
    {
      int_array->items = new_items;
      int_array->capacity = new_capacity;

      if (new_capacity < int_array->length)
      {
        int_array->length = new_capacity;
      }

      return 1;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    free(int_array->items);
    int_array->items = NULL;
    int_array->capacity = int_array->length = 0;
    return 1;
  }
}

static void IntArray_delete(IntArray *int_array)
{
  IntArray_resize(int_array, 0);
  free(int_array);
}

static VALUE Int_allocate(VALUE klass)
{
  IntArray *int_array = IntArray_new();

  return Data_Wrap_Struct(klass, NULL, IntArray_delete, int_array);
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
    int_array->items[i] = NUM2INT(RARRAY_PTR(items)[i]);
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
    memcpy(self->items, old->items, self->length * sizeof(int));
  }
  else
  {
    // TODO error
  }
}

static VALUE Int_at(VALUE self, VALUE rbIndex)
{
  IntArray *int_array;
  int index;

  Data_Get_Struct(self, IntArray, int_array);

  Check_Type(rbIndex, T_FIXNUM);
  index = NUM2INT(rbIndex);

  if (index >= 0)
  {
    if (index < int_array->length)
    {
      return INT2NUM(int_array->items[index]);
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

  return int_array->length > 0 ? Qtrue : Qfalse;
}

static VALUE Int_last(VALUE self)
{
  IntArray *int_array;
  Data_Get_Struct(self, IntArray, int_array);

  if (int_array->length > 0)
  {
    return INT2NUM(int_array->items[int_array->length - 1]);
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
    int_array->items[int_array->length++] = NUM2INT(rbItem);
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

  // resize _might_ fail, and leave it unmodified
  if (IntArray_resize(int_array, int_array->length + 1))
  {
    memmove(int_array->items + index + 1, int_array->items + index, (int_array->length - index) * sizeof(int));
    int_array->length++;
    int_array->items[index] = NUM2INT(rbItem);
  }
  else
  {
    // error
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
    result = INT2NUM(int_array->items[index]);
    memmove(int_array->items + index, int_array->items + index + 1, (int_array->length - index - 1) * sizeof(int));
    int_array->length--;
    IntArray_resize(int_array, int_array->length);
  }
  else
  {
    // error
  }

  return result;
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
}

void Init_faster_mri(void)
{
  Faster = rb_define_module("Faster");
  Faster_Array = rb_define_module_under(Faster, "Array");

  define_Faster_Array_Int();
}