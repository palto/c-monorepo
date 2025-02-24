#include <stdio.h>
#include "fibonacci.h"
#include "prompt.h"

typedef struct {
    unsigned int index;
    long *numbers;
} FibonacciArrayContext;

void fibonacci_arr_collect(long number, void* context) {
    FibonacciArrayContext* ctx = (FibonacciArrayContext*) context;
    ctx->numbers[ctx->index] = number;
    ctx->index++;
}

void fibonacci(long *arr, int count)
{
    FibonacciArrayContext collection = {
        .index = 0,
        .numbers = arr
    };

    fibonacci_callback(count, fibonacci_arr_collect, &collection);
}

void fibonacci_callback(int count, FibonacciCallback cb, void* context)
{
    long prev = 0;
    long acc = 1;
    for (int i = 0; i < count; i++)
    {
        if(i == 0) {
            cb(0, context);
            continue;
        }
        if(i == 1) {
            cb(1, context);
            continue;
        }

        long temp = acc;
        acc = acc + prev;
        prev = temp;
        cb(acc, context);
    }
}

void print_number(long number) {
    fprintf(stdout, "%ld\n", number);
}

void fibonacci_app() {
    int count = prompt_for_number();
    if (count == -1)
    {
        fprintf(stderr, "User gave up\n");
        return;
    }

    long numbers[count];

    fprintf(stderr, "Generating %d fibonacci\n", count);
    
    fibonacci(numbers, count);

    for (int i; i < count; i++) {
        fprintf(stdout, "%d: %ld\n", i + 1, numbers[i]);
    }
}



