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
  long *new_items;

  new_capacity = IntArray_calc_capacity(new_capacity);

  if (new_capacity == int_array->capacity)
  {
    /* nothing */
    return 1;
  }
  else if (new_capacity > 0)
  {
    new_items = int_array->items;
    REALLOC_N(new_items, long, new_capacity);

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

/*
    return 0 if empty?
    return size if last < search_value

    bottom_index = 0
    top_index = size

    while top_index > bottom_index
      index = (bottom_index + top_index) >> 1   # integer
      index_value = at(index)
      comparison = (index_value <=> search_value)

      if comparison == 0      # index_value == search_value
        return index
      elsif comparison > 0    # index_value >  search_value
        top_index = index
      else                    # index_value <  search_value
        bottom_index = index + 1
      end
    end

    top_index
*/

long IntArray_binary_search_ge(IntArray *array, long search_value)
{
  if (array->length == 0)
  {
    return 0;
  }
  else if (array->items[array->length - 1] < search_value)
  {
    return array->length;
  }
  else
  {
    long bottom_index = 0;
    long top_index = array->length;

    while (top_index > bottom_index)
    {
      long index = (bottom_index + top_index) >> 1;
      long index_value = array->items[index];

      if (index_value == search_value)
      {
        return index;
      }
      else if (index_value > search_value)
      {
        top_index = index;
      }
      else
      {
        bottom_index = index + 1;
      }
    }

    return top_index;
  }
}