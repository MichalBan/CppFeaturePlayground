#pragma once

#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <functional>
#include <mutex>
#include <thread>

template <typename T>
struct SmartNode
{
	std::shared_ptr<SmartNode> Next = nullptr;
	T Data;
};

template <typename T>
class SmartList
{
public:
	SmartList();

	void Add(T NewValue);
	bool RemoveFirst(T Value, std::function<bool(const T&, const T&)> const& Comparator);
	int RemoveAll(T Value, std::function<bool(const T&, const T&)> const& Comparator);
	void CallOnAll(std::function<void(const T&)> const& Callback);
	void Print();
	void Clear();

	bool Log = false;

private:
	std::ostream& GetLogStream();

	std::shared_ptr<SmartNode<T>> Head = nullptr;
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
		Head = std::make_shared<SmartNode<T>>();
		Head->Data = NewValue;
		return;
	}

	std::shared_ptr<SmartNode<T>> Current = Head;
	while (Current->Next != nullptr)
	{
		Current = Current->Next;
	}
	Current->Next = std::make_shared<SmartNode<T>>();
	Current->Next->Data = NewValue;
}

template <typename T>
bool SmartList<T>::RemoveFirst(T Value, std::function<bool(const T&, const T&)> const& Comparator)
{
	if (!Head)
	{
		GetLogStream() << "List is empty. No elements removed\n";
		return false;
	}

	if (Comparator(Head->Data, Value))
	{
		Head = Head->Next;
		Head = nullptr;
		GetLogStream() << "Removed 1 element\n";
		return true;
	}

	std::shared_ptr<SmartNode<T>> Current = Head;
	while (Current->Next)
	{
		if (Comparator(Current->Next->Data, Value))
		{
			Current->Next = Current->Next->Next;
			GetLogStream() << "Removed 1 element\n";
			return true;
		}
		Current = Current->Next;
	}

	GetLogStream() << "Searched the list. No elements removed\n";
	return false;
}

template <typename T>
int SmartList<T>::RemoveAll(T Value, std::function<bool(const T&, const T&)> const& Comparator)
{
	if (!Head)
	{
		GetLogStream() << "List is empty. No elements removed\n";
		return 0;
	}

	int Removed = 0;
	while (Comparator(Head->Data, Value))
	{
		Head = Head->Next;
		++Removed;
		if (!Head)
		{
			GetLogStream() << "Removed " << Removed << " elements\n";
			return Removed;
		}
	}

	std::shared_ptr<SmartNode<T>> Current = Head;
	while (Current->Next)
	{
		if (Comparator(Current->Next->Data, Value))
		{
			Current->Next = Current->Next->Next;
			++Removed;
		}
		else
		{
			Current = Current->Next;
		}
	}
	GetLogStream() << "Removed " << Removed << " elements\n";
	return Removed;
}

template <typename T>
void SmartList<T>::CallOnAll(std::function<void(const T&)> const& Callback)
{
	static int NumThreads = std::max<int>(int(std::thread::hardware_concurrency()), 1);
	GetLogStream() << "Call on all with " << NumThreads << " threads\n";
	if (!Head)
	{
		GetLogStream() << "List is empty\n";
		return;
	}
	int NumElements = 1;
	std::shared_ptr<SmartNode<T>> Current = Head;
	while (Current->Next)
	{
		++NumElements;
		Current = Current->Next;
	}
	GetLogStream() << "List has " << NumElements << " elements\n";
	int NumThreadElements = NumElements / NumThreads;
	int NumExtras = NumElements - NumThreadElements * NumThreads;
	GetLogStream() << "Creating " << NumThreads - NumExtras << " threads to handle " << NumThreadElements <<
		" elements and "
		<< NumExtras
		<< " threads to handle " << NumThreadElements + 1 << " elements\n";

	std::vector<std::pair<std::shared_ptr<SmartNode<T>>, int>> ThreadLists;
	ThreadLists.resize(NumThreads);
	Current = Head;
	int CurrentIndex = 0;
	GetLogStream() << "Threads will start at elements: ";
	for (int i = 0; i < NumThreads; ++i)
	{
		GetLogStream() << CurrentIndex << " ";
		int ThisThreadElements = i < NumExtras ? NumThreadElements + 1 : NumThreadElements;
		ThreadLists[i].first = Current;
		ThreadLists[i].second = ThisThreadElements;
		for (int j = 0; j < ThisThreadElements; ++j)
		{
			Current = Current->Next;
			++CurrentIndex;
		}
	}
	GetLogStream() << "\n";

	std::atomic ThreadCounter = 0;
	std::vector<std::thread> Threads;
	std::mutex OutputMutex;
	Threads.resize(NumThreads);
	for (int i = 0; i < NumThreads; ++i)
	{
		Threads[i] = std::thread([i, ThreadLists, &ThreadCounter, &OutputMutex, this, Callback]
		{
			OutputMutex.lock();
			GetLogStream() << "Thread " << i << " starting with " << ThreadLists[i].second << " local nodes\n";
			OutputMutex.unlock();
			std::shared_ptr<SmartNode<T>> ThreadCurrent = ThreadLists[i].first;
			for (int j = 0; j < ThreadLists[i].second; ++j)
			{
				OutputMutex.lock();
				GetLogStream() << "Thread " << i << " running callback for local node " << j << " of value "
					<< ThreadCurrent->Data << "\n";
				OutputMutex.unlock();

				Callback(ThreadCurrent->Data);
				ThreadCurrent = ThreadCurrent->Next;
			}
			OutputMutex.lock();
			GetLogStream() << "Thread " << i << " ending (" << ThreadCounter.fetch_add(1) + 1 << "/"
				<< ThreadLists.size() << ")\n";
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
void SmartList<T>::Print()
{
	if (!Head)
	{
		std::cout << "List is empty";
	}

	std::ostringstream Stream;
	Stream << "List content: " << Head->Data;
	std::shared_ptr<SmartNode<T>> Current = Head;
	while (Current->Next)
	{
		Stream << ", " << Current->Next->Data;
		Current = Current->Next;
	}
	std::cout << Stream.str() << '\n';
}

template <typename T>
void SmartList<T>::Clear()
{
	GetLogStream() << "Clearing the list\n";
	Head = nullptr;
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
