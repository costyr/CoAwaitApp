// Co_awaitSample.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <future>
#include <iostream>
#include <experimental/resumable>
#include <experimental/generator>
#include <iterator>

std::experimental::generator<unsigned int> iota(unsigned int n = 0)
{
  n++;
  while (true)
  {
    co_yield n++;
    n++;
  }
}

std::future<int> async_add(int a, int b)
{
  auto fut = std::async([=]() {
    int c = a + b;
    return c;
    });

  return fut;
}

std::future<int> async_fib(int n)
{
  if (n <= 2)
    co_return 1;

  int a = 1;
  int b = 1;

  // iterate computing fib(n)
  for (int i = 0; i < n - 2; ++i)
  {
    int c = co_await async_add(a, b);
    a = b;
    b = c;
  }

  co_return b;
}

std::future<void> test_async_fib()
{
  int bb[] = { 100000, 7, 20, 2, 1 };
  for (auto i : bb)
  {
    int ret = co_await async_fib(i);
    std::cout << "async_fib(" << i << ") returns " << ret << std::endl;
  }
}

int main()
{
  std::copy_n(iota(42).begin(), 9, std::ostream_iterator<int>(std::cout, ","));
  std::cout << std::endl;

  auto fut = test_async_fib();

  for (int i = 0; i < 1000000000; i++)
    std::cout << i << std::endl;

  fut.wait();
}
