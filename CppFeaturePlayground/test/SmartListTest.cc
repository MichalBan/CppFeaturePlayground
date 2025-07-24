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
	int Values[] = {2, 4, 5};
	SmartList<int> List;
	for (int Val : Values)
	{
		List.Add(Val);
	}
	std::shared_ptr<SmartNode<int>> Current = List.GetHead();
	for (int Val : Values)
	{
		EXPECT_TRUE(Current != nullptr);
		EXPECT_EQ(Current->Data, Val);
		Current = Current->Next;
	}
}

TEST(remove_first, first)
{
	int Values[] = {2, 4, 5, 2, 4};
	SmartList<int> List;
	for (int Val : Values)
	{
		List.Add(Val);
	}
	List.RemoveFirst(2, [](const int& E1, const int& E2) {return E1 == E2; });

	std::shared_ptr<SmartNode<int>> Current = List.GetHead();
	for (int i = 1; i < 5; ++i)
	{
		EXPECT_TRUE(Current != nullptr);
		EXPECT_EQ(Current->Data, Values[i]);
		Current = Current->Next;
	}
}
