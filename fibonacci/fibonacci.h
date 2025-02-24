#ifndef FIBONACCI_H
#define FIBONACCI_H

typedef void (*FibonacciCallback)(long, void *);

void fibonacci(long* numbers, int count);

void fibonacci_callback(int count, FibonacciCallback cb, void *context);

void fibonacci_app();

#endif // FIBONACCI_H