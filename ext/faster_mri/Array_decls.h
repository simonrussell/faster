typedef struct
{
  long capacity;
  long length;
  Item *items;
} Array;

Array *Array_(new)(void);
long Array_(calc_capacity)(long desired);
int Array_(resize)(Array *int_array, long new_capacity);
void Array_(delete)(Array *int_array);
long Array_(binary_search_ge)(Array *array, Item search_value);