#include <iostream>
#include "SmartList.h"
#include "gtest/gtest.h"

TEST(add, head)
{
	int Value = 2;

	SmartList<int> List;
	List.Add(Value);

	std::shared_ptr<SmartNode<int>> Head = List.GetHead();
	EXPECT_TRUE(Head != nullptr);
	EXPECT_EQ(Head->Data, Value);
}

TEST(add, multiple)
{
	std::initializer_list<int> Initializer = {2, 4, 5, 2, 4};

	SmartList<int> List;
	for (int Val : Initializer)
	{
		List.Add(Val);
	}

	std::shared_ptr<SmartNode<int>> Current = List.GetHead();
	for (int Val : Initializer)
	{
		EXPECT_TRUE(Current != nullptr);
		EXPECT_EQ(Current->Data, Val);
		Current = Current->Next;
	}
}

TEST(add, initilizer)
{
	std::initializer_list<int> Initializer = {2, 4, 5, 2, 4};

	SmartList<int> List;
	List.Add(Initializer);

	std::shared_ptr<SmartNode<int>> Current = List.GetHead();
	for (int Val : Initializer)
	{
		EXPECT_TRUE(Current != nullptr);
		EXPECT_EQ(Current->Data, Val);
		Current = Current->Next;
	}
}

TEST(remove_first, first)
{
	std::initializer_list<int> Initializer = {2, 4, 5, 2, 4};
	std::vector<int> Values = Initializer;
	int RemovedIndex = 0;

	SmartList<int> List;
	List.Add(Initializer);
	List.RemoveFirst(Values[RemovedIndex], [](const int& E1, const int& E2) { return E1 == E2; });

	std::shared_ptr<SmartNode<int>> Current = List.GetHead();
	for (auto i = 0; i < Values.size(); ++i)
	{
		if (i != RemovedIndex)
		{
			EXPECT_TRUE(Current != nullptr);
			EXPECT_EQ(Current->Data, Values[i]);
			Current = Current->Next;
		}
	}
}

TEST(remove_first, middle)
{
	std::initializer_list<int> Initializer = {2, 4, 5, 2, 4};
	std::vector<int> Values = Initializer;
	int RemovedIndex = 2;

	SmartList<int> List;
	List.Add(Initializer);
	List.RemoveFirst(Values[RemovedIndex], [](const int& E1, const int& E2) { return E1 == E2; });

	std::shared_ptr<SmartNode<int>> Current = List.GetHead();
	for (auto i = 0; i < Values.size(); ++i)
	{
		if (i != RemovedIndex)
		{
			EXPECT_TRUE(Current != nullptr);
			EXPECT_EQ(Current->Data, Values[i]);
			Current = Current->Next;
		}
	}
}

TEST(remove_all, first)
{
	std::initializer_list<int> Initializer = {2, 4, 5, 2, 4};
	std::vector<int> Values = Initializer;
	int RemovedIndex = 2;

	SmartList<int> List;
	List.Add(Initializer);
	List.RemoveAll(Values[RemovedIndex], [](const int& E1, const int& E2) { return E1 == E2; });

	std::shared_ptr<SmartNode<int>> Current = List.GetHead();
	for (auto i = 0; i < Values.size(); ++i)
	{
		if (Values[i] != Values[RemovedIndex])
		{
			EXPECT_TRUE(Current != nullptr);
			EXPECT_EQ(Current->Data, Values[i]);
			Current = Current->Next;
		}
	}
}

TEST(call_on_all, square_array)
{
	std::initializer_list<int> Initializer = {0, 1, 2, 3, 4, 5};
	std::vector<int> Results;
	Results.resize(Initializer.size());

	SmartList<int> List;
	List.Add(Initializer);
	List.CallOnAll([&Results](const int& E)
	{
		Results[E] = E * E;
	});

	for (int i = 0; i < Results.size(); ++i)
	{
		EXPECT_EQ(Results[i], i * i);
	}
}
