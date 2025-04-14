#ifndef DYN_ARR_H
#define DYN_ARR_H

#include <assert.h>

#define ASSERT assert
#define REALLOC realloc
#define FREE free

// Initial capacity of a dynamic array
#ifndef DA_INIT_CAP
#define DA_INIT_CAP 256
#endif

// Append an item to a dynamic array
#define da_append(da, item)                                                    \
  do {                                                                         \
    if ((da)->count >= (da)->capacity) {                                       \
      (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity * 2; \
      (da)->items =                                                            \
          REALLOC((da)->items, (da)->capacity * sizeof(*(da)->items));         \
      ASSERT((da)->items != NULL && "No more memory");                         \
    }                                                                          \
    (da)->items[(da)->count++] = (item);                                       \
  } while (0)

#define da_free(da) FREE((da).items)

// Append several items to a dynamic array
#define da_append_many(da, new_items, new_items_count)                         \
  do {                                                                         \
    if ((da)->count + (new_items_count) > (da)->capacity) {                    \
      if ((da)->capacity == 0) {                                               \
        (da)->capacity = DA_INIT_CAP;                                          \
      }                                                                        \
      while ((da)->count + (new_items_count) > (da)->capacity) {               \
        (da)->capacity *= 2;                                                   \
      }                                                                        \
      (da)->items =                                                            \
          REALLOC((da)->items, (da)->capacity * sizeof(*(da)->items));         \
      ASSERT((da)->items != NULL && "No more  memory");                        \
    }                                                                          \
    memcpy((da)->items + (da)->count, (new_items),                             \
           (new_items_count) * sizeof(*(da)->items));                          \
    (da)->count += (new_items_count);                                          \
  } while (0)

#define da_remove(da, index)                                                   \
  do {                                                                         \
    ASSERT(((da)->count >= (size_t)index) &&                                             \
           "Cannot remove element out of scope in DA");                        \
                                                                               \
    (da)->count--;                                                             \
    for (size_t ___i = index; ___i < (da)->count; ++___i) {                             \
      (da)->items[___i] = (da)->items[___i + 1];                                     \
    }                                                                          \
  } while (0);

#define da_remove_range(da, index, len)                                        \
  do {                                                                         \
    if ((da)->count <= len + index) {                                          \
      printf("Cannot remove element out of scope in DA %ld / %ld",             \
             len + index, (da)->count);                                        \
      exit(1);                                                                 \
    }                                                                          \
                                                                               \
    (da)->count -= len;                                                        \
    for (size_t i = index; i < (da)->count; ++i) {                             \
      (da)->items[i] = (da)->items[i + len];                                   \
    }                                                                          \
  } while (0);

#endif // DYN_ARR_H
