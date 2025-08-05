#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <functional>
#include <mutex>
#include <thread>
#include <type_traits>
#include <nlohmann/json.hpp>
#include "gtest/gtest_prod.h"

template <typename T>
struct SmartNode
{
	std::unique_ptr<SmartNode> Next = nullptr;
	T Data;
};

template <typename T>
struct Sublist
{
	SmartNode<T>* Head = nullptr;
	int HeadIndex;
	int NumElements;
};

template <typename T>
class SmartList
{
public:
	SmartList();

	void Add(T NewValue);
	void Add(std::initializer_list<T> NewValues);
	bool RemoveFirst(const T& Value, std::function<bool(const T&, const T&)> const& Comparator);
	int RemoveAll(const T& Value, std::function<bool(const T&, const T&)> const& Comparator);
	void CallOnAll(std::function<void(const T&)> const& Callback);
	void CallOnAll(std::function<void(const T&, int Index)> const& Callback);
	void Print(std::ostream& InStream = std::cout);
	void Clear();
	bool SaveTo(const std::string& Filename);
	void LoadFrom(const std::string& Filename);
	SmartNode<T>* GetHead();

	bool Log = false;

private:
	FRIEND_TEST(get_log_stream, cout);
	FRIEND_TEST(get_log_stream, null_stream);
	std::ostream& GetLogStream();

	std::unique_ptr<SmartNode<T>> Head = nullptr;
	std::ofstream NullStream;
};

template <typename T>
SmartList<T>::SmartList()
{
	NullStream.setstate(std::ios_base::badbit);
}

template <typename T>
void SmartList<T>::Add(T NewValue)
{
	GetLogStream() << "Adding new element " << NewValue << "\n";

	if (Head == nullptr)
	{
		Head = std::make_unique<SmartNode<T>>();
		Head->Data = NewValue;
		return;
	}

	SmartNode<T>* Current = Head.get();
	while (Current->Next != nullptr)
	{
		Current = Current->Next.get();
	}
	Current->Next = std::make_unique<SmartNode<T>>();
	Current->Next->Data = NewValue;
}

template <typename T>
void SmartList<T>::Add(std::initializer_list<T> NewValues)
{
	SmartNode<T>* Current = Head.get();
	const T* Iter = NewValues.begin();
	if (!Head)
	{
		Head = std::make_unique<SmartNode<T>>();
		Head->Data = *Iter;
		Current = Head.get();
		++Iter;
	}
	else
	{
		while (Current->Next)
		{
			Current = Current->Next.get();
		}
	}

	for (; Iter < NewValues.end(); ++Iter)
	{
		Current->Next = std::make_unique<SmartNode<T>>();
		Current->Next->Data = *Iter;
		Current = Current->Next.get();
	}
}

template <typename T>
bool SmartList<T>::RemoveFirst(const T& Value, std::function<bool(const T&, const T&)> const& Comparator)
{
	if (!Head)
	{
		GetLogStream() << "List is empty. No elements removed\n";
		return false;
	}

	if (Comparator(Head->Data, Value))
	{
		Head = std::move(Head->Next);
		GetLogStream() << "Removed 1 element\n";
		return true;
	}

	SmartNode<T>* Current = Head.get();
	while (Current->Next)
	{
		if (Comparator(Current->Next->Data, Value))
		{
			Current->Next = std::move(Current->Next->Next);
			GetLogStream() << "Removed 1 element\n";
			return true;
		}
		Current = Current->Next.get();
	}

	GetLogStream() << "Searched the list. No elements removed\n";
	return false;
}

template <typename T>
int SmartList<T>::RemoveAll(const T& Value, std::function<bool(const T&, const T&)> const& Comparator)
{
	if (!Head)
	{
		GetLogStream() << "List is empty. No elements removed\n";
		return 0;
	}

	int Removed = 0;
	while (Comparator(Head->Data, Value))
	{
		Head = std::move(Head->Next);
		++Removed;
		if (!Head)
		{
			GetLogStream() << "Removed " << Removed << " elements\n";
			return Removed;
		}
	}

	SmartNode<T>* Current = Head.get();
	while (Current->Next)
	{
		if (Comparator(Current->Next->Data, Value))
		{
			Current->Next = std::move(Current->Next->Next);
			++Removed;
		}
		else
		{
			Current = Current->Next.get();
		}
	}
	GetLogStream() << "Removed " << Removed << " elements\n";
	return Removed;
}

template <typename T>
void SmartList<T>::CallOnAll(std::function<void(const T&)> const& Callback)
{
	CallOnAll([&Callback](const T& E, int)
	{
		Callback(E);
	});
}

template <typename T>
void SmartList<T>::CallOnAll(std::function<void(const T&, int Index)> const& Callback)
{
	static int NumThreads = std::max<int>(int(std::thread::hardware_concurrency()), 1);
	GetLogStream() << "Call on all with " << NumThreads << " threads\n";
	if (!Head)
	{
		GetLogStream() << "List is empty\n";
		return;
	}
	int NumElements = 1;
	SmartNode<T>* Current = Head.get();
	while (Current->Next)
	{
		++NumElements;
		Current = Current->Next.get();
	}
	GetLogStream() << "List has " << NumElements << " elements\n";
	int NumThreadElements = NumElements / NumThreads;
	int NumExtras = NumElements - NumThreadElements * NumThreads;
	GetLogStream() << "Creating " << NumThreads - NumExtras << " threads to handle " << NumThreadElements <<
		" elements and " << NumExtras << " threads to handle " << NumThreadElements + 1 << " elements\n";

	std::vector<Sublist<T>> ThreadLists;
	ThreadLists.resize(NumThreads);
	Current = Head.get();
	int CurrentIndex = 0;
	GetLogStream() << "Threads will start at elements: ";
	for (int i = 0; i < NumThreads; ++i)
	{
		GetLogStream() << CurrentIndex << " ";
		int ThisThreadElements = i < NumExtras ? NumThreadElements + 1 : NumThreadElements;
		ThreadLists[i].Head = Current;
		ThreadLists[i].HeadIndex = CurrentIndex;
		ThreadLists[i].NumElements = ThisThreadElements;
		for (int j = 0; j < ThisThreadElements; ++j)
		{
			Current = Current->Next.get();
			++CurrentIndex;
		}
	}
	GetLogStream() << "\n";

	int ThreadCounter = 0;
	std::vector<std::thread> Threads;
	std::mutex OutputMutex;
	Threads.resize(NumThreads);
	for (int i = 0; i < NumThreads; ++i)
	{
		Threads[i] = std::thread([i, ThreadLists, &ThreadCounter, &OutputMutex, this, Callback]
		{
			OutputMutex.lock();
			GetLogStream() << "Thread " << i << " starting with " << ThreadLists[i].NumElements << " local nodes\n";
			OutputMutex.unlock();
			SmartNode<T>* ThreadCurrent = ThreadLists[i].Head;
			for (int j = 0; j < ThreadLists[i].NumElements; ++j)
			{
				OutputMutex.lock();
				GetLogStream() << "Thread " << i << " running callback for local node " << j << " (global " <<
					ThreadLists[i].HeadIndex + j << ") of value " << ThreadCurrent->Data << "\n";
				OutputMutex.unlock();

				Callback(ThreadCurrent->Data, ThreadLists[i].HeadIndex + j);
				ThreadCurrent = ThreadCurrent->Next.get();
			}
			OutputMutex.lock();
			++ThreadCounter;
			GetLogStream() << "Thread " << i << " ending (" << ThreadCounter << "/" << ThreadLists.size() << ")\n";
			OutputMutex.unlock();
		});
	}

	for (int i = 0; i < NumThreads; ++i)
	{
		Threads[i].join();
	}
	GetLogStream() << "All threads ended\n";
}

template <typename T>
void SmartList<T>::Print(std::ostream& InStream)
{
	if (!Head)
	{
		InStream << "List is empty";
		return;
	}

	InStream << "List content: " << Head->Data;
	SmartNode<T>* Current = Head.get();
	while (Current->Next)
	{
		InStream << ", " << Current->Next->Data;
		Current = Current->Next.get();
	}
	InStream << '\n';
}

template <typename T>
void SmartList<T>::Clear()
{
	GetLogStream() << "Clearing the list\n";
	Head = nullptr;
}

template <typename T>
bool SmartList<T>::SaveTo(const std::string& Filename)
{
	std::string FullName = Filename + ".json";
	GetLogStream() << "Saving to file " << FullName << "\n";
	std::ofstream Filestream(FullName);
	if (!Filestream.is_open())
	{
		GetLogStream() << "Saving failed. Failed to open file " << FullName << "\n";
		return false;
	}

	nlohmann::json JsonObject;
	JsonObject["log"] = Log;
	JsonObject["data"] = nlohmann::json::array();
	SmartNode<T>* Current = Head.get();
	while (Current)
	{
		JsonObject["data"].push_back(Current->Data);
		Current = Current->Next.get();
	}
	std::string Content = JsonObject.dump(4);
	GetLogStream() << "File content:\n" << Content << "\n";
	Filestream << Content;
	Filestream.close();
	return true;
}

template <typename T>
void SmartList<T>::LoadFrom(const std::string& Filename)
{
	std::string FullName = Filename + ".json";
	GetLogStream() << "Loading from file " << FullName << "\n";
	std::ifstream Filestream(FullName);
	if (!Filestream.is_open())
	{
		GetLogStream() << "Loading failed. Failed to open file " << FullName << "\n";
		return;
	}

	Clear();
	nlohmann::json JsonObject = nlohmann::json::parse(Filestream);
	Log = JsonObject["log"];

	int NumElements = int(JsonObject["data"].size());
	if (NumElements == 0)
	{
		GetLogStream() << "Loaded list is empty\n";
		return;
	}

	Head = std::make_unique<SmartNode<T>>();
	Head->Data = JsonObject["data"].at(0);
	SmartNode<T>* Current = Head.get();
	for (int i = 1; i < NumElements; ++i)
	{
		Current->Next = std::make_unique<SmartNode<T>>();
		Current = Current->Next.get();
		Current->Data = T(JsonObject["data"].at(i));
	}
	GetLogStream() << "Loaded list with " << NumElements << " elements\n";
}

template <typename T>
std::ostream& SmartList<T>::GetLogStream()
{
	if (Log)
	{
		return std::cout;
	}
	return NullStream;
}

template <typename T>
SmartNode<T>* SmartList<T>::GetHead()
{
	return Head.get();
}
