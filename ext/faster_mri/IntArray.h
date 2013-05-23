typedef struct
{
  long capacity;
  long length;
  int *items;
} IntArray;

IntArray *IntArray_new(void);
long IntArray_calc_capacity(long desired);
int IntArray_resize(IntArray *int_array, long new_capacity);
void IntArray_delete(IntArray *int_array);