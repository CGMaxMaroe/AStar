#include <iostream>
#include "AStar.h"

int main()
{
	Node a("a");
	Node b("b");
	Node c("c");
	Node d("d");
	Node e("e");

	a.AddConnection(&b, 5);
	b.AddConnection(&c, 1);
	a.AddConnection(&d, 7);
	c.AddConnection(&d, 1);
	d.AddConnection(&e, 2);
	c.AddConnection(&e, 2);

	std::vector<Node*> path = AStar::FindPath(&a, &e);


	if (!path.empty())
	{
		for (auto nodeIterator = path.begin(); nodeIterator < path.end() - 1; ++nodeIterator)
		{
			std::cout << nodeIterator[0]->name << "<--" << nodeIterator[0]->GetConnectedNodes()[nodeIterator[1]] << "-->";
			if (nodeIterator == path.end() - 2)
			{
				std::cout << nodeIterator[1]->name << " Total cost: " << nodeIterator[1]->gScore;
			}
		}
	}

	return 0;
}