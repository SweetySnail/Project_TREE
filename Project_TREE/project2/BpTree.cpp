#include "BpTree.h"
bool BpTree::Insert(Characterdata* new_Character)
{
	// if B+ Tree is empty
	if (root == NULL)
	{
		root = new BpTreeDataNode;  // new Node is root
		root->insertDataMap(new_Character->getname(), new_Character);  // Data insert
		return true;
	}

	// search DataNode to insert
	BpTreeNode* Insert_Node = searchDataNode(new_Character->getname());

	map<string, Characterdata*>::iterator find_it = Insert_Node->getDataMap()->begin();  // make iterator
	
	// duplication data check
	while (find_it != Insert_Node->getDataMap()->end())
	{
		if (find_it->first == new_Character->getname())
			break;
		else
			find_it++;
	}

	// if this data is duplication
	if (find_it != Insert_Node->getDataMap()->end())
	{
		find_it->second->setpickinc();  // increase pick rate

		if (new_Character->getwin() != 0)  // if this character win
			find_it->second->setwininc();  // increase win rate

		return false;
	}
	// if this data isn't duplication
	else
	{
		// insert data in Insert_Node
		Insert_Node->insertDataMap(new_Character->getname(), new_Character);

		// if Insert_Node is exceed, split Insert_Node
		if (exceedDataNode(Insert_Node))
		{
			// split Data Node
			splitDataNode(Insert_Node);
			Insert_Node = Insert_Node->getParent();

			// split Index Node
			while (exceedIndexNode(Insert_Node))
			{
				splitIndexNode(Insert_Node);
				Insert_Node = Insert_Node->getParent();
			}
		}
		return true;
	}
}

BpTreeNode * BpTree::searchDataNode(string n)
{
	BpTreeNode* current_Node = root;
	// repeat search, until arrive at Data Node
	while (current_Node->getMostLeftChild() != NULL)
	{
		map<string, BpTreeNode*>::iterator it = current_Node->getIndexMap()->begin();
		
		if (n < it->first)  // if n is lower than MostLeftChild
			current_Node = current_Node->getMostLeftChild();  // go to MostLeftChild
		
		else  // if n is bigger than MostLeftchild
		{
			while (it != current_Node->getIndexMap()->end())  // search where to insert
			{
				it++;
				// insert data on the left side of it
				if (it == current_Node->getIndexMap()->end())
				{
					it--;
					current_Node = it->second;
					break;
				}
				// insert data on the left side of it
				if (n < it->first)
				{
					it--;
					current_Node = it->second;
					break;
				}
			}
		}
	}
	return current_Node;
}

void BpTree::splitDataNode(BpTreeNode* pDataNode)
{
	int split = ceil((order - 1) / 2.0) + 1;  // location to split
	
	BpTreeNode* New_Data = new BpTreeDataNode;  // make new Node

	map<string, Characterdata*>::iterator it = pDataNode->getDataMap()->begin();  // initial position setting
	for (int i = 1; i < split; i++)
		it++;

	// insert data in New_Data
	for (; it != pDataNode->getDataMap()->end(); it++)
		New_Data->insertDataMap(it->first, it->second);

	// delte New_Data's data in pDataNode
	it = pDataNode->getDataMap()->begin();
	for (int i = 1; i < split; i++)
		it++;

	pDataNode->getDataMap()->erase(it, pDataNode->getDataMap()->end());

	if (root == pDataNode)  // if pDataNode is root
	{
		BpTreeNode* New_Index = new BpTreeIndexNode;  // New_Index is root
		root = New_Index;
		// insert data in New_Index
		New_Index->insertIndexMap(New_Data->getDataMap()->begin()->first, New_Data);
		New_Data->setParent(New_Index);

		// link Nodes
		pDataNode->setParent(New_Index);
		New_Index->setMostLeftChild(pDataNode);

		pDataNode->setNext(New_Data);
		New_Data->setPrev(pDataNode);
	}
	else  // if pDataNode isn't root
	{
		BpTreeNode* Parent_Node = pDataNode->getParent();

		// insert New_Data's data in Parent_Node
		Parent_Node->insertIndexMap(New_Data->getDataMap()->begin()->first, New_Data);
		New_Data->setParent(Parent_Node);
		pDataNode->setParent(Parent_Node);

		map<string, BpTreeNode*>::iterator it = Parent_Node->getIndexMap()->begin();

		// search exact location of inserted data
		while (it->first != New_Data->getDataMap()->begin()->first)
			it++;

		if (it->first != Parent_Node->getIndexMap()->begin()->first)  // if not inserted at the most left
		{
			it--;
			it->second = pDataNode;
		}
		else  // if inserted at the most left
			Parent_Node->setMostLeftChild(pDataNode);

		// link Nodes
		if(pDataNode->getNext() != NULL)
			pDataNode->getNext()->setPrev(New_Data);
		New_Data->setNext(pDataNode->getNext());
		pDataNode->setNext(New_Data);
		New_Data->setPrev(pDataNode);
	}
}

void BpTree::splitIndexNode(BpTreeNode* pIndexNode)
{
	int split = ceil((order - 1) / 2.0) + 1;  // location to split

	BpTreeNode* New_Index = new BpTreeIndexNode;  // make new Index Node
	BpTreeNode* Parent_Index = new BpTreeIndexNode;  // make new Index Node

	map<string, BpTreeNode*>::iterator it = pIndexNode->getIndexMap()->begin();  // initial position setting
	for (int i = 1; i < split; i++)
		it++;

	// insert data into Parent_Index Node
	Parent_Index->insertIndexMap(it->first, it->second);
	it->second->setParent(Parent_Index);
	it++;

	// insert data into New_Index Node
	for (; it != pIndexNode->getIndexMap()->end(); it++)
	{
		New_Index->insertIndexMap(it->first, it->second);
		it->second->setParent(New_Index);
	}

	// link Nodes
	New_Index->setMostLeftChild(Parent_Index->getIndexMap()->begin()->second);
	Parent_Index->getIndexMap()->begin()->second->setParent(New_Index);
	Parent_Index->getIndexMap()->begin()->second = New_Index;

	New_Index->setParent(Parent_Index);

	// delete the contents of New_Index, Parent_Index in pDataNode
	it = pIndexNode->getIndexMap()->begin();
	for (int i = 1; i < split; i++)
		it++;

	pIndexNode->getIndexMap()->erase(it, pIndexNode->getIndexMap()->end());

	if (root == pIndexNode)  // if pIndexNode was root
	{
		root = Parent_Index;  // Parent_Node is root

		// link Nodes
		Parent_Index->setMostLeftChild(pIndexNode);
		pIndexNode->setParent(Parent_Index);
	}
	else  // if pIndexNode wasn't root
	{
		BpTreeNode* Parent_Node = pIndexNode->getParent();

		Parent_Node->insertIndexMap(Parent_Index->getIndexMap()->begin()->first, Parent_Index->getIndexMap()->begin()->second);
		New_Index->setParent(Parent_Node);
		
		map<string, BpTreeNode*>::iterator it = Parent_Node->getIndexMap()->begin();

		// search exact location of inserted data
		while (it->first != Parent_Index->getIndexMap()->begin()->first)
			it++;

		// insert pIndexNode
		if (it->first != Parent_Node->getIndexMap()->begin()->first)
		{
			it--;
			it->second = pIndexNode;
		}
		else
			Parent_Node->setMostLeftChild(pIndexNode);

		// link Nodes
		pIndexNode->setParent(Parent_Node);
	}
}

void BpTree::printRoute(string n) { }

bool BpTree::exceedDataNode(BpTreeNode* pDataNode)
{
	// Number of Node_order changes while scanning the DataMap of the Node
	if (pDataNode->getDataMap()->size() >= this->order)
		return true;//over size
	else
		return false;
}

bool BpTree::exceedIndexNode(BpTreeNode* pIndexNode)
{
	// Number of Node_order changes while scanning the IndexMap of the Node
	if (pIndexNode->getIndexMap()->size() >= this->order)
		return true;//over size
	else
		return false;
}

void BpTree::printIndex_levelOrder()
{
	BpTreeNode* cNode = root;

	// move to Data Node
	while (cNode->getMostLeftChild() != NULL)
		cNode = cNode->getMostLeftChild();

	// repeat until last Data Node
	while (cNode->getNext() != NULL)
	{
		map<string, Characterdata*>::iterator it = cNode->getDataMap()->begin();
		// output all contents of the map
		for (; it != cNode->getDataMap()->end(); it++)
			printCharacterInfo(it->second);
		cNode = cNode->getNext();  // move next Node
	}
}

void BpTree::SearchRange(string start, string end)
{
	BpTreeNode* Begin_Node = searchDataNode(start);
	BpTreeNode* End_Node = searchDataNode(end);

	map<string, Characterdata*>::iterator begin_it = Begin_Node->getDataMap()->find(start);
	map<string, Characterdata*>::iterator end_it = End_Node->getDataMap()->find(end);

	if (Begin_Node == End_Node)  // if start Node and end Node are the same
	{
		while (begin_it != end_it)
		{
			printCharacterInfo(begin_it->second);
			begin_it++;
		}
	}
	else  // if start Node and end Node ar different
	{
		// print to the end of the first node
		while (begin_it != Begin_Node->getDataMap()->end())
		{
			printCharacterInfo(begin_it->second);
			begin_it++;
		}

		Begin_Node = Begin_Node->getNext();
		begin_it = Begin_Node->getDataMap()->begin();

		// print all map of the Node until it reaches the End_Node
		while (Begin_Node != End_Node)
		{
			printCharacterInfo(begin_it->second);
			begin_it++;

			if (begin_it == Begin_Node->getDataMap()->end())
			{
				Begin_Node = Begin_Node->getNext();
				begin_it = Begin_Node->getDataMap()->begin();
			}
		}

		// print map until reaches end_it
		while (begin_it != end_it)
		{
			printCharacterInfo(begin_it->second);
			begin_it++;
		}
		printCharacterInfo(begin_it->second);
	}
}

void BpTree::Print()
{
	printIndex_levelOrder();
}

void BpTree::printCharacterInfo(Characterdata* pCharacter)
{
	cout << pCharacter->getname() << "	" << pCharacter->getpick() << "	" << pCharacter->getwin() << "	" << pCharacter->getratio() << endl;
	ofstream write("log.txt", ios::app);
	write<< pCharacter->getname() << "	" << pCharacter->getpick() << "	" << pCharacter->getwin() << "	" << pCharacter->getratio() << endl;
	write.close();
}

void BpTree::printPickRank()
{
	// use Max-Heap
	BpTreeNode* current_Node = root;

	// move to the most left Data Node
	while (current_Node->getMostLeftChild() != NULL)
		current_Node = current_Node->getMostLeftChild();

	map<string, Characterdata*>::iterator it = current_Node->getDataMap()->begin();

	while (current_Node->getNext() != NULL)  // repeat until last Data Node
	{
		// data to insert in pick_heap
		int pick = it->second->getpick();
		string name = it->second->getname();
		Characterdata* data = it->second;
		
		// insert data into pick_heap
		pick_heap.push(make_pair(make_pair(pick, name), data));

		it++;

		// at the end of the map, move to next Node
		if (it == current_Node->getDataMap()->end())
		{
			current_Node = current_Node->getNext();
			it = current_Node->getDataMap()->begin();
		}
	}

	while (!pick_heap.empty())  // until pick_heap is emtpy
	{
		// print and pop most top data in pick_heap
		Characterdata *print = pick_heap.top().second;
		pick_heap.pop();
		printCharacterInfo(print);
	}
}

void BpTree::printRatioRank()
{
	// use Min-Heap
	BpTreeNode* current_Node = root;

	// move to the most left Data Node
	while (current_Node->getMostLeftChild() != NULL)
		current_Node = current_Node->getMostLeftChild();

	map<string, Characterdata*>::iterator it = current_Node->getDataMap()->begin();

	
	while (current_Node->getNext() != NULL)  // repeat until last Data Node
	{
		// data to insert in ratio_heap
		float ratio = it->second->getratio();
		string name = it->second->getname();
		Characterdata* data = it->second;

		// insert data into ratio_heap
		ratio_heap.push(make_pair(make_pair(ratio, name), data));
		
		it++;

		// at the end of the map, move to next Node
		if (it == current_Node->getDataMap()->end())
		{
			current_Node = current_Node->getNext();
			it = current_Node->getDataMap()->begin();
		}
	}

	while (!ratio_heap.empty())  // until ratio_heap is emtpy
	{
		// print and pop most top data in ratio_heap
		Characterdata *print = ratio_heap.top().second;
		ratio_heap.pop();
		printCharacterInfo(print);
	}
}