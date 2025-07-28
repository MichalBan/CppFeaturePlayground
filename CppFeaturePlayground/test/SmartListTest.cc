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

TEST(add_initializer, empty)
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

TEST(add_initializer, non_empty)
{
	std::initializer_list<int> Initializer = {2, 4, 5, 2, 4};

	SmartList<int> List;
	List.Add(0);
	List.Add(0);
	List.Add(Initializer);

	std::shared_ptr<SmartNode<int>> Current = List.GetHead()->Next->Next;
	for (int Val : Initializer)
	{
		EXPECT_TRUE(Current != nullptr);
		EXPECT_EQ(Current->Data, Val);
		Current = Current->Next;
	}
}

TEST(remove_first, empty)
{
	SmartList<int> List;
	EXPECT_FALSE(List.RemoveFirst(0, [](const int& E1, const int& E2) { return E1 == E2; }));
}

TEST(remove_first, no_element)
{
	SmartList<int> List;
	List.Add({2, 4, 5, 2, 4});
	EXPECT_FALSE(List.RemoveFirst(0, [](const int& E1, const int& E2) { return E1 == E2; }));
}

TEST(remove_first, first)
{
	std::initializer_list<int> Initializer = {2, 4, 5, 2, 4};
	std::vector<int> Values = Initializer;
	int RemovedIndex = 0;

	SmartList<int> List;
	List.Add(Initializer);
	EXPECT_TRUE(List.RemoveFirst(Values[RemovedIndex], [](const int& E1, const int& E2) { return E1 == E2; }));

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
	EXPECT_TRUE(List.RemoveFirst(Values[RemovedIndex], [](const int& E1, const int& E2) { return E1 == E2; }));

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

TEST(remove_all, empty)
{
	SmartList<int> List;
	EXPECT_TRUE(List.RemoveAll(0, [](const int& E1, const int& E2) { return E1 == E2; }) == 0);
}

TEST(remove_all, first)
{
	std::initializer_list<int> Initializer = {2, 4, 5, 2, 4};
	std::vector<int> Values = Initializer;

	SmartList<int> List;
	List.Add(Initializer);
	EXPECT_TRUE(List.RemoveAll(2, [](const int& E1, const int& E2) { return E1 == E2; }) == 2);

	std::shared_ptr<SmartNode<int>> Current = List.GetHead();
	for (int Value : Values)
	{
		if (Value != 2)
		{
			EXPECT_TRUE(Current != nullptr);
			EXPECT_EQ(Current->Data, Value);
			Current = Current->Next;
		}
	}
}

TEST(remove_all, remove_all_elements)
{
	std::initializer_list<int> Initializer = {2, 2};
	std::vector<int> Values = Initializer;

	SmartList<int> List;
	List.Add(Initializer);
	EXPECT_TRUE(List.RemoveAll(2, [](const int& E1, const int& E2) { return E1 == E2; }) == 2);
	EXPECT_TRUE(List.GetHead() == nullptr);
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

TEST(call_on_all, empty)
{
	SmartList<int> List;
	List.CallOnAll([](const int& E)
	{
	});
}

TEST(print, empty)
{
	SmartList<int> List;
	std::ostringstream StringStream;
	List.Print(&StringStream);
	EXPECT_EQ(StringStream.str(), "List is empty");
}

TEST(print, elements)
{
	SmartList<int> List;
	List.Add({1, 5, 2, 5, 3});
	std::ostringstream StringStream;
	List.Print(&StringStream);
	EXPECT_EQ(StringStream.str(), "List content: 1, 5, 2, 5, 3\n");
}

TEST(save_to, strings)
{
	std::initializer_list<std::string> Initializer = {"one", "two", "three"};
	std::string ExpectedResult = R"({"data":["one","two","three"],"log":false})";

	SmartList<std::string> List;
	List.Add(Initializer);
	List.SaveTo("TestFile");

	std::ifstream Filestream("TestFile.json");
	std::string FileContent;
	EXPECT_TRUE(Filestream.is_open());
	char C;
	Filestream.get(C);
	while (!Filestream.eof())
	{
		if (!isspace(C))
		{
			FileContent += C;
		}
		Filestream.get(C);
	}
	Filestream.close();

	EXPECT_EQ(FileContent, ExpectedResult);
	EXPECT_TRUE(std::remove("TestFile.json") == 0);
}

TEST(load_from, no_file)
{
	SmartList<std::string> List;
	List.LoadFrom("TestFile");
	EXPECT_TRUE(List.GetHead() == nullptr);
}

TEST(load_from, empty_list)
{
	std::string ExpectedResult = R"({"data":[],"log":false})";
	std::ofstream Filestream("TestFile.json");
	Filestream << ExpectedResult;
	Filestream.close();

	SmartList<std::string> List;
	List.LoadFrom("TestFile");
	EXPECT_TRUE(List.GetHead() == nullptr);
	EXPECT_TRUE(std::remove("TestFile.json") == 0);
}

TEST(load_from, strings)
{
	std::vector<std::string> ExpectedValues = {"one", "two", "three"};
	std::string ExpectedResult = R"({"data":["one","two","three"],"log":false})";
	std::ofstream Filestream("TestFile.json");
	Filestream << ExpectedResult;
	Filestream.close();

	SmartList<std::string> List;
	List.LoadFrom("TestFile");
	std::shared_ptr<SmartNode<std::string>> Current = List.GetHead();
	for (const std::string& ExpectedValue : ExpectedValues)
	{
		EXPECT_TRUE(Current != nullptr);
		EXPECT_EQ(Current->Data, ExpectedValue);
		Current = Current->Next;
	}
	EXPECT_TRUE(std::remove("TestFile.json") == 0);
}
