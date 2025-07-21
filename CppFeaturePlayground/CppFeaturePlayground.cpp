// CppFeaturePlayground.cpp : Defines the entry point for the application.
//

#include "CppFeaturePlayground.h"

#include <random>

#include "SmartList.h"

int main()
{
	std::cout << "Creating a list and adding elements\n";
	SmartList<float> List;
	List.Log = true;
	List.Add(1.1f);
	List.Add(2.5f);
	List.Add(3.5f);
	List.Add(5.5f);
	List.Add(2.5f);
	List.Add(3.3f);
	List.Add(3.5f);
	List.Add(5.9f);
	List.Add(2.5f);
	List.Add(3.9f);
	List.Print();

	std::cout << "Removing the first element of value 2.5\n";
	List.RemoveFirst(2.5, [](const float& E1, const float& E2)
	{
		return abs(E1 - E2) < 1.0E-10;
	});
	List.Print();

	std::cout << "Removing all elements of value 3.5\n";
	List.RemoveAll(3.5, [](const float& E1, const float& E2)
	{
		return abs(E1 - E2) < 1.0E-10;
	});
	List.Print();

	std::cout << "Testing multithreading with a new random list\n";
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distribution(0.0, 5.0);
	int NumRandomElements = 99;
	List.Clear();
	for (int i = 0; i < NumRandomElements; ++i)
	{
		List.Add(distribution(gen));
	}
	List.CallOnAll([](const float& E)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(int(1000 * E)));
	});

	std::cout << "All tests done\n";
	return 0;
}
