#include "btree.h"

b_tree* init_tree() { return NULL; }

b_tree* make_node(){
	b_tree* new = (b_tree*) malloc(sizeof(b_tree));
	new->keys_number = 0;
	new->keys = (int*) malloc(sizeof(int) * ((2*t) - 1));
	new->leaf = 1;
	new->children = (b_tree**) malloc(sizeof(b_tree) * 2 * t);
	int i;
	for(i = 0; i < (2 * t); i++) new->children[i] = NULL;
	return new;
}

b_tree* free_tree( b_tree* T ){
	if(!T) return NULL;
	if(!T->leaf){
		unsigned i;
		for(i = 0; i <= T->keys_number; i++) free_tree(T->children[i]);
	}
	free(T->children);
	free(T->keys);
	free(T);
	return NULL;
}

void print_tree( b_tree* T, unsigned height ){
	if(!T) return;
	unsigned i, j;
	for(i = 0; i <= T->keys_number - 1; i++){
		print_tree(T->children[i], height + 1);
		for(j = 0; j <= height; j++) printf("    ");
		printf("%d\n", T->keys[i]);
	}
	print_tree(T->children[i], height + 1);
}

b_tree* search_key( b_tree* T, int key ){
	if(!T) return NULL;
	unsigned i = 0;
	while(i < T->keys_number && key > T->keys[i]) i++;
	if(i < T->keys_number && key == T->keys[i]) return T;
	if(T->leaf) return NULL;
	return search_key(T->children[i], key);
}

b_tree* divide_node( b_tree* x, int index, b_tree* y ){
	// Divide node y into node x->(z,y), where x will be the father,
	// z will take t-1 first keys and y will take the remaining children
	b_tree* z = make_node();
	z->keys_number = t - 1;
	z->leaf = y->leaf;

	// Passing the first t-1 keys to z as well as the t first children
	unsigned j;
	for(j = 0; j < t - 1; j++)
		z->keys[j] = y->keys[j + t];
	if(!y->leaf)
		for(j = 0; j < t; j++){
			z->children[j] = y->children[j+t];
			y->children[j + t] = NULL;
		}

	// Adjusting the children from x to add the new child z on index pos
	y->keys_number = t - 1;
	for(j = x->keys_number; j >= index; j--)
		x->children[j + 1] = x->children[j];
	x->children[index] = z;

	// Adjusting the keys from x to add the key t-1 from y on index-1
	for(j = x->keys_number; j >= index; j--)
		x->keys[j] = x->keys[j - 1];
	x->keys[index - 1] = y->keys[t - 1];
	x->keys_number++;

	return x;
}

b_tree* partial_insert( b_tree* x, int key ){
	int i = x->keys_number - 1;

	// If the node is a leaf, then the key is inserted
	if(x->leaf){
		while((i >= 0) && (key < x->keys[i])){
			x->keys[i + 1] = x->keys[i];
			i--;
		}
		x->keys[i + 1] = key;
		x->keys_number++;
		return x;
	}

	// Finding the child where key must be inserted
	// If the child has reached keys limit, then it is divided and the 
	//		new key "root" will be added in x
	while((i >= 0) && (key < x->keys[i])) i--;
	i++;
	if(x->children[i]->keys_number == (2 * t) - 1){
		x = divide_node(x, i+1, x->children[i]);
		if(key > x->keys[i]) i++;
	}

	// Try to insert on the correspondent child
	x->children[i] = partial_insert(x->children[i], key);
	
	return x;
}

b_tree* insert_key( b_tree* T, int key ){
	if(search_key(T, key)) return T; // Element already inserted
	if(!T){ // NULL tree
		T = make_node();
		T->keys[0] = key;
		T->keys_number++;
		return T;
	}
	if(T->keys_number == (2 * t) - 1){ // Root node is full, needs division
		b_tree* aux = make_node();
		aux->leaf = 0;
		aux->children[0] = T;
		aux = divide_node(aux, 1, T);
		aux = partial_insert(aux, key);
		return aux;
	}
	// Insert the key
	T = partial_insert(T, key);
	return T;
}

b_tree* remove_key( b_tree* T, int key ){
	if(!T || !search_key(T, key)) return T;
	return _remove_key(T, key);
}
b_tree* _remove_key( b_tree* T, int key ){
	if(!T) return T;
	unsigned i; // Position of the key on T
	printf("Removendo.. %d...\n", key);

	// Finding the the node, or its child, where key must be on node T
	for(i = 0; i < T->keys_number && T->keys[i] < key; i++);

	// The key is in node T
	if((i < T->keys_number) && (key == T->keys[i])){ // Cases 1, 2A, 2B & 2C
		if(T->leaf){ // Case 1
			printf("Case 1\n");
			unsigned j;
			for(j = i; j < T->keys_number - 1; j++)
				T->keys[j] = T->keys[j+1];
			T->keys_number--;
			return T;
		}
		if((!T->leaf) && (T->children[i]->keys_number >= t)){ // Case 2A
			printf("Case 2A\n");

			// Finding the ancestor k' of the left child from key
			b_tree* y = T->children[i];
			while(!y->leaf)
				y = y->children[y->keys_number];
			
			// Eliminating k' and swaping it for k in T
			int temp = y->keys[y->keys_number - 1];
			T->children[i] = _remove_key(T->children[i], temp);
			T->keys[i] = temp;
			return T;
		}
		if((!T->leaf) && (T->children[i + 1]->keys_number >= t)){ // Case 2B
			printf("Case 2B\n");

			// Finding the successor k' of the right child from key
			b_tree* y = T->children[i + 1];
			while(!y->leaf)
				y = y->children[0];

			// Eliminating k' and swaping it for k in T
			int temp = y->keys[0];
			y = _remove_key(T->children[i + 1], temp);
			T->keys[i] = temp;
			return T;
		}
		if((!T->leaf) && (T->children[i + 1]->keys_number == t - 1) && (T->children[i]->keys_number == t - 1)){ // Case 2C
			printf("Case 2C\n");

			// Merging the two children of key
			b_tree* y = T->children[i];
			b_tree* z = T->children[i + 1];

			// Adding key to the end of its left child
			y->keys[y->keys_number] = key;
			
			// Joining the keys from the left child + key (y) and the keys from the right child (z)
			unsigned j;
			for(j = 0; j < t - 1; j++)
				y->keys[t + j] = z->keys[j];

			// Joining the children from left and right children
			for(j = 0; j <= t; j++)
				y->children[t + j] = z->children[j];

			// Removing key and its right child from T
			y->keys_number = 2 * t - 1;
			for(j = i; j < T->keys_number - 1; j++)
				T->keys[j] = T->keys[j + 1];
			for(j = i + 1; j <= T->keys_number; j++)
				T->children[j] = T->children[j + 1];
			T->children[j] = NULL;
			T->keys_number--;

			// Removing key from the new merged child
			T->children[i] = _remove_key(T->children[i], key);
			return T;
		}
	}

	// If the key isn't in the node T
	b_tree* y = T->children[i]; // Child where key must be
	b_tree* z = NULL;
	if(y->keys_number == t - 1){ // Cases 3A & 3B
		if((i < T->keys_number) && (T->children[i + 1]->keys_number >= t)){ // Case 3A, i < keys_number
			printf("Case 3A: i less than keys_number\n");

			// Giving to y the key i from T (father giving a key to its left child)
			z = T->children[i + 1];
			y->keys[t - 1] = T->keys[i];
			y->keys_number++;

			// Giving to T a key from z (right child giving a key to father) & adjusting child's keys
			T->keys[i] = z->keys[0];
			unsigned j;
			for(j = 0; j < z->keys_number - 1; j++)
				z->keys[j] = z->keys[j + 1];
			
			// Giving to new key in y the first child of z & adjusting z's children
			y->children[y->keys_number] = z->children[0];
			for(j = 0; j < z->keys_number; j++)
				z->children[j] = z->children[j + 1];
			z->keys_number--;

			// Removing key in the child i
			T->children[i] = _remove_key(T->children[i], key);
			return T;
		}
		if((i > 0) && (!z) && (T->children[i - 1]->keys_number >= t)){ // Case 3A, i == keys_number
			printf("Case 3A: i equals to keys_number\n");

			// Adjusting keys & children to add new key (father giving a key to the right child)
			z = T->children[i - 1];
			unsigned j;
			for(j = y->keys_number; j > 0; j--)
				y->keys[j] = y->keys[j - 1];
			for(j = y->keys_number + 1; j > 0; j--)
				y->children[j] = y->children[j - 1];

			// Giving to right child the father's key
			y->keys[0] = T->keys[i - 1];
			y->keys_number++;

			// Father receiving a key from its left child
			T->keys[i - 1] = z->keys[z->keys_number - 1];

			// Right child's new key receive the last child from father's left child
			y->children[0] = z->children[z->keys_number];
			z->keys_number--;

			// Removing key in the child i
			T->children[i] = _remove_key(y, key);
			return T;
		}
		if(!z){
			if((i < T->keys_number) && (T->children[i + 1]->keys_number == t - 1)){ // Case 3B, i < keys_number
				printf("Case 3B: i less than keys_number\n");

				z = T->children[i + 1];

				// Giving to left child (y) the key i from T (father giving a key to its left child)
				y->keys[t - 1] = T->keys[i];
				y->keys_number++;

				// Left child (y) receive right child's keys & children
				unsigned j;
				for(j = 0; j < t - 1; j++){
					y->keys[t + j] = z->keys[j];
					y->keys_number++;
				}
				if(!y->leaf)
					for (j = 0; j < t; j++)
						y->children[t + j] = z->children[j];

				// Adjusting keys & children after the moves in T
				for(j = i; j < T->keys_number - 1; j++){
					T->keys[j] = T->keys[j + 1];
					T->children[j + 1] = T->children[j + 2];
				}
				T->keys_number--;

				// Removing key in T
				T = _remove_key(T, key);
				return T;
			}
			if((i > 0) && (T->children[i - 1]->keys_number == t - 1)){ // Case 3B, i == keys_number
				printf("Case 3B: i equals to keys_number\n");

				// If key is on the last child (extreme right) of T, then T gives the key i-1 to its (i-1)th child
				// 		otherwise T'll give the key i to its (i-1)th child
				z = T->children[i - 1];
				if(i == T->keys_number)
					z->keys[t - 1] = T->keys[i - 1];
				else
					z->keys[t - 1] = T->keys[i];
				z->keys_number++;

				// Giving to the (i-1)th child the keys & children of the i-th child
				unsigned j;
				for(j = 0; j < t - 1; j++){
					z->keys[t + j] = y->keys[j];
					z->keys_number++;
				}
				if(!z->leaf)
					for(j = 0; j < t; j++)
						z->children[t + j] = y->children[j];

				// Updating T
				T->keys_number--;
				T->children[i - 1] = z;

				// Removing key in T
				T = _remove_key(T, key);
				return T;
			}
		}
	}
	// If none of the cases occurs
	T->children[i] = _remove_key(T->children[i], key);
	return T;
}