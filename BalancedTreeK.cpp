//
// Created by alonj on 23-Dec-17.
//
#include "Key.h"
#include "BalancedTreeK.h"

/*
 * Get a pointer to Node of known Key
 * */
Node* BalancedTreeK::search_key(const Key* key) const{
    Node* y = _root;
    if (*(y->get_key()) < *key) {
        return NULL;
    }
    while(!y->isLeaf){
        int i=0;
        while (i < y->direct_children && *(y->get_child(i)->get_key()) < *key) {
            i++;
        }
        y = y->get_child(i);
    }
    y = y->get_parent();
    int n = 0;
    while(!(y->get_child(n)->compare_key(key)) && n < y->direct_children-1){
        n++;
    }
    if(n == 2*K - 1 or !(y->get_child(n)->compare_key(key))){
        return NULL;
    }
    else{
        return y->get_child(n);
    }
}

/*
 * Constructor
 * */
BalancedTreeK::BalancedTreeK(const Key *min, const Key *max){
    Key* minKey = min->clone();
    Key* maxKey = max->clone();
    minNode = new Node(NULL, minKey);
    maxNode = new Node(NULL, maxKey);
    Node* root = new Node;
    minNode->sentinel = true;
    maxNode->sentinel = true;
    minNode->set_parent(root, true);
    maxNode->set_parent(root, true);
    _root=root;
    _root->total_children=0;
    minNode->total_children=0;
    maxNode->total_children=0;
}

/*
 * Split nodes for Insert method
 * */
Node* insert_and_split(Node* x, Node* z){ // ex. : x=E, z=Z
    if(x->direct_children < 2*K - 1){
        z->set_parent(x);
        return NULL;
    }
    else {
        Node* y = new Node();
        int split_point = K-1;
        /*
         * If new node is larger than mid-point node, split the array after the middle point (k-1 nodes remain in new parent)
         * */
        if (*(x->get_child(K-1)->get_key()) < *z->get_key()) {
            split_point++;
        }
        for (int i = split_point; i < 2 * K - 1; i++) {
            Node* child = x->get_child(split_point);
            child->set_parent(y);
        }
        Node* find_parent = y->direct_children > x->direct_children?x:y;
        z->set_parent(find_parent);
        y->update_attributes();
        x->update_attributes();
        return y;
    }
}

/*
 * Insert new node (key, val) into the tree
 * */
void BalancedTreeK::Insert(const Key* nkey, const Value* nval){
    Key* key;
    Value* val;
    if(nkey != NULL && nval != NULL){
        key = nkey->clone();
        val = nval->clone();
    }
    else {
        return;
    }
    Node* z = new Node(val, key);
    Node* y = _root;
    while(!y->get_child(0)->isLeaf){
        int i = 0;
        while(i < y->direct_children-1 && !(*nkey < *(y->get_child(i)->get_key()))){
            i++;
        }
        y = y->get_child(i);
    }
    // Node* x = y->get_parent();
    Node* x = y;
	z = insert_and_split(x, z); 
	//int flag = z==NULL ? 0 : 1;
    while(x != _root){
        x = x->get_parent();
        if(z != NULL){
            z = insert_and_split(x, z);
        }
        else {
            x->update_attributes();
        }
    }
    if(z != NULL){
        Node* new_root = new Node;
        z->update_attributes();
        z->set_parent(new_root);
        x->update_attributes();
        x->set_parent(new_root);
        _root = new_root;
    }

}


/*
 * Search value by known Key
 * */
Value* BalancedTreeK::Search(const Key *key) const {
    if(*(_root->get_key()) < *key ){
        return NULL;
    }
    else{
        Node* result = search_key(key);
        if(result != NULL){
            return result->get_value();
        }
        else{
            return NULL;
        }
    }
}

/*
 * Get ordered rank of known Key
 * */
unsigned BalancedTreeK::Rank(const Key *key) const {
    if(*(_root->get_key())<*key) {
        return 0;
    }
    Node* x = search_key(key);
    if (x != NULL) {
        unsigned rank = 1;
        Node *y = x->get_parent();
        while (y != NULL) {
            int i = 0;
            while (y->get_child(i) != x) {
                rank += y->get_child(i)->total_children;
                i++;
            }
            x = y;
            y = y->get_parent();
        }
        return rank;
    }
    else{
        return 0;
    }
}

/*
 * Get key of known ordered rank
 * */
const Key* BalancedTreeK::Select(unsigned index) const {
    Node* x = _root;
//    index++;
    if(x->total_children < index){
        return NULL;
    }
    else if(x->isLeaf){
        return x->get_key();
    }
    else {
        while (!x->isLeaf) {
            int count_sum_siblings = 0;
            int i = 0;
            while (i < x->direct_children-1 && index > count_sum_siblings + x->get_child(i)->total_children) {
                count_sum_siblings += x->get_child(i)->total_children;
                i++;
            }
            x = x->get_child(i);
            index -= count_sum_siblings;
        }
        return x->get_key();
    }
}

Node* borrow_and_merge(Node* y){
    Node* z = y->get_parent();
    int y_place = 0;
    Node* x = NULL;
    while(z->get_child(y_place) != y){
        y_place++;
    }
    bool left = false;
    if(y_place == 0){
        x = z->get_child(y_place+1);
    }
    else{
        x = z->get_child(y_place-1);
        left = true;
    }
    if(y->direct_children + x->direct_children > 2*K-1){
        if(!left){
            int i = 0;
            while(y->direct_children<K){
                Node* x_child = x->get_child(i);
                x_child->set_parent(y);
                i++;
            }
        }
        else{
            int i = x->direct_children-1;
            while(y->direct_children<K){
                x->get_child(i)->set_parent(y);
                i--;
            }

        }
    }
    else{
        while(y->direct_children > 0){
            y->get_child(0)->set_parent(x);
        }
    }
    if(x->direct_children == 0){
        z->remove_child(x);
        z->update_attributes();
        x->isLeaf = false;
        delete x;
    }
    if(y->direct_children == 0){
        z->remove_child(y);
        z->update_attributes();
        y->isLeaf = false;
        delete y;
    }
    return z;
}

void BalancedTreeK::Delete(const Key *dkey) {
    Node* dNode = search_key(dkey);
    if (dNode == NULL){
        return;
    }
    Node* y = dNode->get_parent();
    y->remove_child(dNode);
    while(y != NULL){
        if(y->direct_children<K){
            if(y != this->_root){
                y = borrow_and_merge(y);
            }
            else if(_root->direct_children == 1){
                this->_root = y->get_child(0);
                _root->null_p();
                y->set_key(NULL);
                y->set_value(NULL);
                y->set_minkey(NULL);
                delete y;
                return;
            }
            else{
                y->update_attributes();
                y = y->get_parent();
            }
        }
        else{
            y->update_attributes();
            y = y->get_parent();
        }
    }
    delete dNode;
}

const Value* BalancedTreeK::GetMaxValue(const Key *key1, const Key *key2) const{
    if((*(this->_root->get_key())<*key1 || *key2 < *key1) || _root == NULL || _root->total_children == 0) {
        return NULL;
    }
    Key* zero_key = this->Select(0)->clone();
    unsigned left_boundary = this->Rank(key1);
    unsigned right_boundary = this->Rank(key2);
    Node* left = search_key(Select(left_boundary));
    Node* right = search_key(Select(right_boundary));
    if (left_boundary == 0 && (*zero_key < *key1 || *key1 < *zero_key)) { // rank = 0 either didnt find or real 0, check if key1 different to actual key in place 0.
        Node *y = _root;
        while (!y->isLeaf) {
            int i = 0;
            while (*(y->get_child(i)->get_key()) < *(key1) && y->get_child(i)->get_key() != NULL) {
                i++;
            }
            if(y->get_child(i)->sentinel && i == 0){
                i++;
            }
            if(y->get_child(i)->sentinel && i != 0){
                i--;
            }
            y = y->get_child(i);
        }
        left = y;
//        left_boundary = this->Rank(y->get_key());
    }
    if (right_boundary == 0 && (*zero_key < *key2 || *key2 < *zero_key)) {
        Node *y = _root;
        while (!y->isLeaf) {
            int i = 0;
            while (*(y->get_child(i)->get_key()) < *(key2) && i < y->direct_children-1) {
                i++;
            }
            if(i>0 && *key2 < *(y->get_child(i)->get_minKey())){
                i--;
            }
            if(y->get_child(i)->sentinel){
                i--;
            }
            y = y->get_child(i);
        }
        right = y;
    }
    delete zero_key;
    if(*right->get_key() < *key1 || *key2 < *left->get_key()){
        return NULL;
    }
    Value* max = *left->get_value()<*right->get_value()?right->get_value():left->get_value();
    Node* x = left;
    Node* x_parent = x->get_parent();
    while(*(x_parent->get_key()) < *(right->get_key())) {
        for(int i = 0; i < x_parent->direct_children; i++){
            if(x_parent->get_child(i)->sentinel){
                i++;
            }
            if( *x->get_key() < *(x_parent->get_child(i)->get_key()) && *max < *(x_parent->get_child(i)->get_value())){
                max = x_parent->get_child(i)->get_value();
            }
        }

        x = x_parent;
        x_parent = x->get_parent();
    }
    for(int i = 0; i < x_parent->direct_children; i++){
        if(x_parent->get_child(i)->sentinel){
            i++;
        }
        if(
            i < x_parent->direct_children &&
            !x_parent->get_child(i)->sentinel &&
            *left->get_key() < *(x_parent->get_child(i)->get_minKey()) && // changed get_key to minkey
            *x_parent->get_child(i)->get_key() < *right->get_key() && // changed minkey to key
            *max < *(x_parent->get_child(i)->get_value())
                ){
            max = x_parent->get_child(i)->get_value();
        }
    }
    x = right;
    x_parent = x->get_parent();
    while(*(left->get_key()) < *(x_parent->get_minKey())) {
        for(int i = 0; i < x_parent->direct_children; i++){
            if(*(x_parent->get_child(i)->get_key()) < *x->get_key() && *max < *(x_parent->get_child(i)->get_value())){
                max = x_parent->get_child(i)->get_value();
            }
        }

        x = x_parent;
        x_parent = x->get_parent();
    }
    return max;
}

/*
void BalancedTreeK::Delete_BalanceTreek(Node * node) {
	int i = 0;
	while (node->get_child(i) != NULL) {
		i++;
	}
	if (i == 0)
	{
		delete node;
		return;
	}
	for (int j = 0; j < i; j++) {
		Delete_BalanceTreek(node->get_child(j));
	}
	delete node;
	node = NULL;

}
*/

void DFS(Node* current, Node* finished_arr[], int& time){
    for(int i=0; i<current->direct_children; i++){
        if(current->get_child(i) != NULL){
            Node *new_current = current->get_child(i);
            DFS(new_current, finished_arr, time);
        }
    }
    finished_arr[time] = current;
    time++;
}

BalancedTreeK::~BalancedTreeK() {
//	Delete_BalanceTreek(_root);
    int size_of_arr = _root->total_children;
    int log = size_of_arr;
    while(log != 0){
        log >>= 1;
        size_of_arr += log;
    }
    Node* finished_arr[size_of_arr] = {NULL};
    int time = 0;
    DFS(_root, finished_arr, time);
    for(int i=0; i<time; i++){
        Node* node_dest = finished_arr[i];
        delete node_dest;
    }
}

