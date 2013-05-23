// NOTE: this isn't actually used by the ruby bridge, so deal with stuff being set to zero.
Array *Array_(new)(void)
{
  Array *array = ALLOC(Array);
  memset(array, 0, sizeof(Array));
  return array;
}

long Array_(calc_capacity)(long desired)
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

int Array_(resize)(Array *array, long new_capacity)
{
  Item *new_items;

  new_capacity = Array_(calc_capacity)(new_capacity);

  if (new_capacity == array->capacity)
  {
    /* nothing */
    return 1;
  }
  else if (new_capacity > 0)
  {
    new_items = array->items;
    REALLOC_N(new_items, Item, new_capacity);

    if (new_items)
    {
      array->items = new_items;
      array->capacity = new_capacity;

      if (new_capacity < array->length)
      {
        array->length = new_capacity;
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
    xfree(array->items);
    array->items = NULL;
    array->capacity = array->length = 0;
    return 1;
  }
}

void Array_(delete)(Array *array)
{
  Array_(resize)(array, 0);
  xfree(array);
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

long Array_(binary_search_ge)(Array *array, Item search_value)
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
      Item index_value = array->items[index];

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

void Array_(insert)(Array *array, long index, Item item)
{
  if (index >= 0 && index <= array->length)
  {
    // resize _might_ fail, and leave it unmodified
    if (Array_(resize)(array, array->length + 1))
    {
      memmove(array->items + index + 1, array->items + index, (array->length - index) * sizeof(Item));
      array->length++;
      array->items[index] = item;
    }
    else
    {
      // error
    }
  }
}
