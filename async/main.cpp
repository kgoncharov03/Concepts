#include "async.h"
#include <iostream>
#include <cassert>
#include <string>

template<>
ThreadPool<int> Async<int>::pool{};

int foo() {
	return 1;
}

void testAsync() {
	std::future<int> f = Async<int>::async(foo, false);
	int result = f.get();
	std::string output = (result == foo()) ? "async test passed" : "async test didn't passed";
	std::cout << output << std::endl;
}

void testSync() {
	std::future<int> f = Async<int>::async(foo, true);
	int result = f.get();
	std::string output = (result == foo()) ? "sync test passed" : "sync test didn't passed";
	std::cout << output << std::endl;
}

int main() {
	testAsync();
	testSync();
	system("pause");
	return 0;
}