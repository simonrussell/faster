#include "faster_mri.h"

// NOTE: this isn't actually used by the ruby bridge, so deal with stuff being set to zero.
IntArray *IntArray_new(void)
{
  IntArray *int_array = ALLOC(IntArray);
  memset(int_array, 0, sizeof(IntArray));
  return int_array;
}

long IntArray_calc_capacity(long desired)
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

int IntArray_resize(IntArray *int_array, long new_capacity)
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
    new_items = int_array->items;
    REALLOC_N(new_items, int, new_capacity);

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
    xfree(int_array->items);
    int_array->items = NULL;
    int_array->capacity = int_array->length = 0;
    return 1;
  }
}

void IntArray_delete(IntArray *int_array)
{
  IntArray_resize(int_array, 0);
  xfree(int_array);
}