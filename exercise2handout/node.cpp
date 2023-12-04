#include "Node.h"
#include "maths_funcs.h"

Node::Node()
	:parent(0)
{ ; }

void Node::init() { 
	position = vec3(0, 0, 0);
	rotation = versor(0,0,0,1);
	scale = vec3(1,1,1);

	localMatrix = identity_mat4();
	worldMatrix = identity_mat4();
	localInverseMatrix = identity_mat4();
	worldInverseMatrix = identity_mat4();
}

void Node::addChild(Node& node) { 
	node.parent = this;
	children.push_back(&node);
}

void Node::removeChild(Node& node) { 
	auto it = std::find(children.begin(), children.end(), &node);
	if (it != children.end()) {
		node.parent = nullptr;
		children.erase(it);
	}
}

void  Node::updateLocal() 
{ 
	// todo: given position, rotation and scale, create T,R and S such that

	mat4 T = identity_mat4();
	T.col[3].x = position.x;
	T.col[3].y = position.y;
	T.col[3].z = position.z;
	
	mat4 R = quat_to_mat4(rotation);

	mat4 S = identity_mat4();
	S.c[0][0] = scale.x;
	S.c[1][1] = scale.y;
	S.c[2][2] = scale.z;
	
	localMatrix = T*R*S;

	// todo: given all above, create Sinv, Rinv and Tinv such that

	mat4 TInverse = identity_mat4();
	T.col[3].x = -position.x;
	T.col[3].y = -position.y;
	T.col[3].z = -position.z;

	versor rotationInverted = rotation;
	rotationInverted.w *= -1;

	mat4 RInverse = quat_to_mat4(rotationInverted);


	mat4 SInverse = identity_mat4();
	S.c[0][0] = 1 / scale.x;
	S.c[1][1] = 1 / scale.y;
	S.c[2][2] = 1 / scale.z;

	localInverseMatrix = SInverse * RInverse * TInverse;
}

void  Node::updateHierarchy()
{
	updateLocal();
	mat4 parentMatrix = identity_mat4();
	mat4 parentInverseMatrix = identity_mat4();
	if (parent!= nullptr) {
		parentMatrix = (*parent).worldMatrix;
		parentInverseMatrix = (*parent).worldInverseMatrix;
	}
	worldMatrix = parentMatrix*localMatrix;
	worldInverseMatrix = parentInverseMatrix*localInverseMatrix;

	for (size_t i = 0; i < children.size(); ++i) {
		Node* child = children[i];
		if (child != nullptr) {
			(*child).updateHierarchy();
		}
	}
}
