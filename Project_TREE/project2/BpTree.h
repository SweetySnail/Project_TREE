#ifndef _BpTree_H_
#define _BpTree_H_

#include "BpTreeNode.h"
#include "BpTreeDataNode.h"
#include "BpTreeIndexNode.h"
#include "Characterdata.h"
// BpTree

class BpTree{
private:
	BpTreeNode*	root;
	int			order;		// m children

public:
	priority_queue<pair<pair<int, string>, Characterdata*>, vector<pair<pair<int, string>, Characterdata*> >, less<pair<pair<int, string>, Characterdata*> > > pick_heap;
	priority_queue<pair<pair<float, string>, Characterdata*>, vector<pair<pair<float, string>, Characterdata*> >, greater<pair<pair<float, string>, Characterdata*> > > ratio_heap;
	BpTree(int order = 3){
		root = NULL;
		this->order = order;
	}

	/* essential */
	bool		Insert(Characterdata* new_Character);
	bool		exceedDataNode(BpTreeNode* pDataNode);
	bool		exceedIndexNode(BpTreeNode* pIndexNode);
	void		splitDataNode(BpTreeNode* pDataNode);
	void		splitIndexNode(BpTreeNode* pIndexNode);

	void	SearchRange(string start, string end);	// print character range
	BpTreeNode*	searchDataNode(string n);//search character node based on name

	void	Print(); // PRINT_all 
	void	printCharacterInfo(Characterdata* pCharacter);//print character data
	void	printRoute(string n);//print n name character route
	void	printIndex_levelOrder();// print in b+ tree root to leaves order
	void	printPickRank();//print pick rank descending order
	void	printRatioRank();//print ratio rank ascending order
	
	
};

#endif