#pragma once
#include <vector>
template <class T>
class SearchRequest
{
public:
	std::vector<T> searchResults;

	//31 zoom tiles
	//common variables
	int x ;
	int y;
	int left;
	int right;
	int top;
	int bottom;
	SearchRequest()
		:x(0),y(0),left(0),right(0),top(0),bottom(0),zoom(15),numberOfReadSubtrees(0),numberOfAcceptedSubtrees(0)
	{

	}
	bool isCancelled()
	{
		return false;
	}
	int zoom;
	int numberOfReadSubtrees;
	int numberOfAcceptedSubtrees;
};
