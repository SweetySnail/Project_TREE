#include "AVLTree.h"
bool AVLTree::Insert(Gamedata* pGame)
{
	// make new Node to insert and insert data
	AVLNode* New_Node = new AVLNode;
	New_Node->setGD(pGame);

	if (root == NULL)  // if root has nothing
	{
		root = New_Node;  // set New_Node to root
		return true;
	}
	else  // if root has anything
	{
		AVLNode* a = root;  // to change BF(change BF's position)
		AVLNode* pa = NULL;  // a's parent Node
		AVLNode* p = root;  // to move Node(insert Node's position)
		AVLNode* pp = NULL;  // p's parent Node
		AVLNode* rootSub = NULL;  // Subtree's root

		while (p != NULL)  // move p to leaf Node
		{
			if (p->getBF() != 0)  // if p is balanced tree
			{
				a = p;  // set a
				pa = pp;  // set pa
			}
			if (p->getGD()->getGameid() > pGame->getGameid())  // if p has to go left
			{
				pp = p;  // set pp
				p = p->getLeft();  // move p
			}
			else if (p->getGD()->getGameid() < pGame->getGameid())  // if p has to go right
			{
				pp = p;  // set pp
				p = p->getRight();  // move p
			}
			else  // if p is duplication
				return false;
		}

		// set New_Node
		if (pp->getGD()->getGameid() > pGame->getGameid())
			pp->setLeft(New_Node);
		else
			pp->setRight(New_Node);

		int d;  // temporary Balance Factor
		AVLNode* b;  // a's child Node
		AVLNode* c;  // a's child Node

		if (a->getGD()->getGameid() > pGame->getGameid())  // if p has to go left
		{
			b = p = a->getLeft();
			d = 1;
		}
		else  // if p has to go right
		{
			b = p = a->getRight();
			d = -1;
		}

		// set Balance Factor
		while (p != New_Node)
		{
			if (p->getGD()->getGameid() > pGame->getGameid())  // if p is bigger than pGame
			{
				p->setBF(1);
				p = p->getLeft();
			}
			else  // if p is smaller than pGame
			{
				p->setBF(-1);
				p = p->getRight();
			}
		}

		if (a->getBF() == 0 || a->getBF() + d == 0)  // if tree is balanced
		{
			a->setBF(a->getBF() + d);
			return true;
		}
		else  // if tree is unbalanced
		{
			// if tree has left unbalanced
			if (d == 1)
			{
				if (b->getBF() == 1)  // LL unbalanced
				{
					a->setLeft(b->getRight());
					b->setRight(a);
					a->setBF(0);
					b->setBF(0);
					rootSub = b;
				}
				else  // LR unbalanced
				{
					c = b->getRight();
					b->setRight(c->getLeft());
					a->setLeft(c->getRight());
					c->setLeft(b);
					c->setRight(a);

					switch (c->getBF())  // set Balance Factor to use c's Balance Factor
					{
					case 0:  // if it is balanced
						a->setBF(0);
						b->setBF(0);
						break;

					case 1:  // if it is unbalanced
						a->setBF(-1);
						b->setBF(0);
						break;

					case -1:  // if it is unbalanced
						a->setBF(0);
						b->setBF(1);
						break;
					}
					c->setBF(0);
					rootSub = c;
				}
			}
			else  // if tree has right unbalanced
			{
				if (b->getBF() == -1)  // RR unbalanced
				{
					a->setRight(b->getLeft());
					b->setLeft(a);
					a->setBF(0);
					b->setBF(0);
					rootSub = b;
				}
				else    // RL unbalanced
				{
					c = b->getLeft();
					a->setRight(c->getLeft());
					b->setLeft(c->getRight());
					c->setLeft(a);
					c->setRight(b);

					switch (c->getBF())  // set Balance Factor to use c's Balance Factor
					{
					case 0:  // if it is left unbalanced
						a->setBF(0);
						b->setBF(0);
						break;

					case 1:  // if it is unbalanced
						a->setBF(0);
						b->setBF(-1);
						break;

					case -1:  // if it is unbalanced
						a->setBF(1);
						b->setBF(0);
						break;
					}
					c->setBF(0);
					rootSub = c;
				}
			}
		}

		if (pa == NULL)  // if a was root
			root = rootSub;
		else if (a == pa->getLeft())  // if a was pa's left child
			pa->setLeft(rootSub);
		else  // if a was pa's right child
			pa->setRight(rootSub);
		return true;
	}
	return true; // no duplication
}

void AVLTree::Print()
{
	// print Inorder traversal
	stack<AVLNode*> q;  // make stack
	AVLNode* current_Node = root;
	ofstream write("log.txt", ios::app);
	while (1)
	{
		while (current_Node != NULL)  // move most left child and push each nodes
		{
			q.push(current_Node);
			current_Node = current_Node->getLeft();
		}
		if (q.empty())  // if stack is empty
			break;

		current_Node = q.top();  // set current_Node's position

		cout << current_Node->getGD()->getGameid() << "	" << current_Node->getGD()->getDuration() << "	" << current_Node->getGD()->getWinner() << "	";
		for (int i = 0; i < 5; i++)
			cout << current_Node->getGD()->getTeamA()[i] << "	";
		for (int i = 0; i < 4; i++)
			cout << current_Node->getGD()->getTeamB()[i] << "	";
		cout << current_Node->getGD()->getTeamB()[4] << endl;

		
		write << current_Node->getGD()->getGameid() << "	" << current_Node->getGD()->getDuration() << "	" << current_Node->getGD()->getWinner() << "	";
		for (int i = 0; i < 5; i++)
			write << current_Node->getGD()->getTeamA()[i] << "	";
		for (int i = 0; i < 4; i++)
			write << current_Node->getGD()->getTeamB()[i] << "	";
		write << current_Node->getGD()->getTeamB()[4] << endl;

		q.pop();  // after write output, pop that node
		current_Node = current_Node->getRight();  // if current_Node has right child, go to right child
	}
	write.close();
}

Gamedata* AVLTree::Search(int id)
{
	AVLNode* Search_Node = root;

	// move Search_Node to search id
	while (Search_Node != NULL)
	{
		if (Search_Node->getGD()->getGameid() > id)  // if Search_Node has bigger than id
			Search_Node = Search_Node->getLeft();
		else if (Search_Node->getGD()->getGameid() < id)  // if Search_Node has smaller than id
			Search_Node = Search_Node->getRight();
		else if (Search_Node->getGD()->getGameid() == id)  // if Search_Node has same id
			return Search_Node->getGD();
	}

	if (Search_Node == NULL)  // if fail to search id
		return NULL;
	else  // if success to search id
		return Search_Node->getGD();
}