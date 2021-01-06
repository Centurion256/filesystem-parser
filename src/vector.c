
#include <vector.h>
// #include <stdio.h>

// Basic vararray/vector implementation (nothing fancy here)

vector vector_new(size_t msize)
{
    // printf("Create: size=%d ", msize);

    vector self = (vector)malloc(sizeof(struct vector_t));
    self->size = 0;
    // self->data = (void **)malloc(sizeof(*(self->data)) * msize);
    // self->data
    self->head=0;
    self->bufsize=sizeof(self->data);
    return self;
}

void *vector_get(vector self, size_t i)
{
    if (i < self->size)
    {
        return (self->data)[(self->head + i)%self->bufsize];
    }
    return 0;
}

int vector_push(vector self, void *data)
{

    self->data[(self->head + self->size) % self->bufsize] = data;
    self->size += 1;
    return 0;
}

void* vector_qpop(vector self)
{
    void* res = self->data[self->head];
    self->head = (self->head+1)%self->bufsize;
    self->size -= 1;
    return res;
}

const void **vector_data(vector self)
{
    return (const void **)self->data;
}

size_t vector_size(vector self)
{
    return self->size;
}

int vector_free(vector self)
{
    // free(self->data);
    free(self);
    return 0;
}