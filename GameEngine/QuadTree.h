#ifndef __QuadTree_H__
#define __QuadTree_H__

#include <vector>
#include "MathGeoLib/include/MathGeoLib.h"

#include "Globals.h"
#include "GameObject.h"

#define QUADTREE_MAX_ITEMS 2


class QuadTreeNode {

public:

	math::AABB box;
	std::vector<GameObject*> objects;
	QuadTreeNode* parent;
	QuadTreeNode* children[4];

public:
	QuadTreeNode(math::AABB limits)
	{
		box.Enclose(limits);
	
		box.Distance((*objects.begin())->GetPosition());
		
	}



};


class QuadTree {


private:
	QuadTreeNode* root;
};

#endif