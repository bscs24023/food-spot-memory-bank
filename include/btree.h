#ifndef BTREE_H
#define BTREE_H

#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

template <typename K, typename V>
class BTreeNode {
public:
    vector<K> keys; 
    vector<V> values;
    vector<BTreeNode*> children;
    bool leaf; 
    int t;

    BTreeNode(int _t, bool _leaf);
    ~BTreeNode();

    void insertNonFull(K key, V value);
    void splitChild(int i, BTreeNode* y);
    
    V* search(K key);
    void searchRange(K minKey, K maxKey, vector<V>& results);
    
    void traverse();
};

template <typename K, typename V>
class BTree {
private:
    BTreeNode<K, V>* root;
    int t;

public:
    BTree(int _t);
    ~BTree();

    void insert(K key, V value);
    V* search(K key);
    vector<V> searchRange(K minKey, K maxKey);
    void traverse();
    void clear(BTreeNode<K, V>* node);
};


template <typename K, typename V>
BTreeNode<K, V>::BTreeNode(int _t, bool _leaf) : t(_t), leaf(_leaf) {}

template <typename K, typename V>
BTreeNode<K, V>::~BTreeNode() {
    for (auto child : children) {
        delete child;
    }
}

template <typename K, typename V>
void BTreeNode<K, V>::insertNonFull(K key, V value) {
    int i = keys.size() - 1;

    if (leaf) {
        keys.push_back(K());
        values.push_back(V());
        
        while (i >= 0 && keys[i] > key) {
            keys[i + 1] = keys[i];
            values[i + 1] = values[i];
            i--;
        }
        
        keys[i + 1] = key;
        values[i + 1] = value;
    } else {
        while (i >= 0 && keys[i] > key) {
            i--;
        }
        i++;
        
        if (children[i]->keys.size() == 2 * t - 1) {
            splitChild(i, children[i]);
            
            if (keys[i] < key) {
                i++;
            }
        }
        children[i]->insertNonFull(key, value);
    }
}

template <typename K, typename V>
void BTreeNode<K, V>::splitChild(int i, BTreeNode* y) {
    BTreeNode* z = new BTreeNode(y->t, y->leaf);
    
    int mid = t - 1;
    
    for (int j = 0; j < t - 1; j++) {
        z->keys.push_back(y->keys[mid + 1 + j]);
        z->values.push_back(y->values[mid + 1 + j]);
    }
    
    if (!y->leaf) {
        for (int j = 0; j < t; j++) {
            z->children.push_back(y->children[mid + 1 + j]);
        }
        y->children.resize(t);
    }
    
    keys.insert(keys.begin() + i, y->keys[mid]);
    values.insert(values.begin() + i, y->values[mid]);
    
    children.insert(children.begin() + i + 1, z);
    
    y->keys.resize(mid);
    y->values.resize(mid);
}

template <typename K, typename V>
V* BTreeNode<K, V>::search(K key) {
    int i = 0;
    while (i < keys.size() && key > keys[i]) {
        i++;
    }
    
    if (i < keys.size() && keys[i] == key) {
        return &values[i];
    }
    
    if (leaf) {
        return nullptr;
    }
    
    return children[i]->search(key);
}

template <typename K, typename V>
void BTreeNode<K, V>::searchRange(K minKey, K maxKey, vector<V>& results) {
    int i = 0;
    
    while (i < keys.size() && keys[i] < minKey) {
        i++;
    }
    
    while (i < keys.size() && keys[i] <= maxKey) {
        if (keys[i] >= minKey) {
            results.push_back(values[i]);
        }
        
        if (!leaf) {
            children[i]->searchRange(minKey, maxKey, results);
        }
        i++;
    }
    
    if (!leaf && i < children.size()) {
        children[i]->searchRange(minKey, maxKey, results);
    }
}

template <typename K, typename V>
void BTreeNode<K, V>::traverse() {
    int i;
    for (i = 0; i < keys.size(); i++) {
        if (!leaf) {
            children[i]->traverse();
        }
        cout << keys[i] << " ";
    }
    
    if (!leaf) {
        children[i]->traverse();
    }
}


template <typename K, typename V>
BTree<K, V>::BTree(int _t) : t(_t) {
    root = new BTreeNode<K, V>(t, true);
}

template <typename K, typename V>
BTree<K, V>::~BTree() {
    clear(root);
}

template <typename K, typename V>
void BTree<K, V>::clear(BTreeNode<K, V>* node) {
    if (node) {
        for (auto child : node->children) {
            clear(child);
        }
        node->children.clear();
        delete node;
    }
}

template <typename K, typename V>
void BTree<K, V>::insert(K key, V value) {
    if (root->keys.size() == 2 * t - 1) {
        BTreeNode<K, V>* s = new BTreeNode<K, V>(t, false);
        s->children.push_back(root);
        s->splitChild(0, root);
        root = s;
    }
    root->insertNonFull(key, value);
}

template <typename K, typename V>
V* BTree<K, V>::search(K key) {
    return root->search(key);
}

template <typename K, typename V>
vector<V> BTree<K, V>::searchRange(K minKey, K maxKey) {
    vector<V> results;
    root->searchRange(minKey, maxKey, results);
    return results;
}

template <typename K, typename V>
void BTree<K, V>::traverse() {
    if (root) {
        root->traverse();
    }
    cout << endl;
}

#endif