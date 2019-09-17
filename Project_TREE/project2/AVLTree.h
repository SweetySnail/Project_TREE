#ifndef _AVLTREE_H_
#define _AVLTREE_H_

#include "Gamedata.h"
#include "AVLNode.h"
/* class AVL */

class AVLTree{
private:
	AVLNode*	root;

public:
	
	AVLTree() { root=NULL; }
	~AVLTree(){};
	bool		Insert(Gamedata* pGame);
	void		Print();
	Gamedata*	Search(int id);
};



#endif

