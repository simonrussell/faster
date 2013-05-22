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

static void IntArray_resize(IntArray *int_array, long new_capacity)
{
  int *new_items;

  if (new_capacity > 0)
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
    }
  }
  else
  {
    if (int_array->items)
    {
      free(int_array->items);
    }

    int_array->items = NULL;
    int_array->capacity = int_array->length = 0;
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

static VALUE Int_at(VALUE self, VALUE rbIndex)
{
  IntArray *int_array;
  Data_Get_Struct(self, IntArray, int_array);

  Check_Type(rbIndex, T_FIXNUM);
  int index = NUM2INT(rbIndex);

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

static void define_Faster_Array_Int(void)
{
  VALUE klass = rb_define_class_under(Faster_Array, "Int", rb_cObject);
  rb_define_alloc_func(klass, Int_allocate);
  rb_define_method(klass, "initialize", Int_initialize, 1);
  rb_define_method(klass, "at", Int_at, 1);
  rb_define_method(klass, "length", Int_length, 0);
}

void Init_faster_mri(void)
{
  Faster = rb_define_module("Faster");
  Faster_Array = rb_define_module_under(Faster, "Array");

  define_Faster_Array_Int();
}