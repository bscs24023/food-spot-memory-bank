#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <vector>
#include <list>
#include <string>
#include <functional>
#include <algorithm>

using namespace std;

template <typename K, typename V>
class HashTable {
private:
    struct Entry {
        K key;
        V value;
        Entry(K k, V v) : key(k), value(v) {}
    };
    
    vector<list<Entry>> table;
    int size;
    int count;
    
    int hashFunction(const K& key) const {
        return hash<K>{}(key) % size;
    }
    
    void rehash() {
        vector<list<Entry>> oldTable = table;
        size *= 2;
        table.clear();
        table.resize(size);
        count = 0;
        
        for (auto& bucket : oldTable) {
            for (auto& entry : bucket) {
                insert(entry.key, entry.value);
            }
        }
    }

public:
    HashTable(int initialSize = 100) : size(initialSize), count(0) {
        table.resize(size);
    }
    
    void insert(const K& key, const V& value) {
        int index = hashFunction(key);
        
        for (auto& entry : table[index]) {
            if (entry.key == key) {
                entry.value = value;
                return;
            }
        }
        
        table[index].push_back(Entry(key, value));
        count++;
        
        if ((float)count / size > 0.7) {
            rehash();
        }
    }
    
    V* get(const K& key) {
        int index = hashFunction(key);
        
        for (auto& entry : table[index]) {
            if (entry.key == key) {
                return &entry.value;
            }
        }
        
        return nullptr;
    }
    
    bool remove(const K& key) {
        int index = hashFunction(key);
        
        auto& bucket = table[index];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->key == key) {
                bucket.erase(it);
                count--;
                return true;
            }
        }
        
        return false;
    }
    
    bool contains(const K& key) const {
        return const_cast<HashTable*>(this)->get(key) != nullptr;
    }
    
    int getSize() const {
        return count;
    }
    
    vector<K> getAllKeys() const {
        vector<K> keys;
        for (const auto& bucket : table) {
            for (const auto& entry : bucket) {
                keys.push_back(entry.key);
            }
        }
        return keys;
    }
    
    vector<V> getAllValues() const {
        vector<V> values;
        for (const auto& bucket : table) {
            for (const auto& entry : bucket) {
                values.push_back(entry.value);
            }
        }
        return values;
    }
};

template <typename K, typename V>
class MultiValueHashTable {
private:
    struct Entry {
        K key;
        vector<V> values;
        Entry(K k) : key(k) {}
    };
    
    vector<list<Entry>> table;
    int size;
    int count;
    
    int hashFunction(const K& key) const {
        return hash<K>{}(key) % size;
    }
    
    void rehash() {
        vector<list<Entry>> oldTable = table;
        size *= 2;
        table.clear();
        table.resize(size);
        count = 0;
        
        for (auto& bucket : oldTable) {
            for (auto& entry : bucket) {
                for (auto& value : entry.values) {
                    insert(entry.key, value);
                }
            }
        }
    }

public:
    MultiValueHashTable(int initialSize = 100) : size(initialSize), count(0) {
        table.resize(size);
    }
    
    void insert(const K& key, const V& value) {
        int index = hashFunction(key);
        
        for (auto& entry : table[index]) {
            if (entry.key == key) {
                if (find(entry.values.begin(), entry.values.end(), value) == entry.values.end()) {
                    entry.values.push_back(value);
                }
                return;
            }
        }
        
        Entry newEntry(key);
        newEntry.values.push_back(value);
        table[index].push_back(newEntry);
        count++;
        
        if ((float)count / size > 0.7) {
            rehash();
        }
    }
    
    vector<V> get(const K& key) const {
        int index = hashFunction(key);
        
        for (const auto& entry : table[index]) {
            if (entry.key == key) {
                return entry.values;
            }
        }
        
        return vector<V>();
    }
    
    bool remove(const K& key, const V& value) {
        int index = hashFunction(key);
        
        auto& bucket = table[index];
        for (auto& entry : bucket) {
            if (entry.key == key) {
                auto it = find(entry.values.begin(), entry.values.end(), value);
                if (it != entry.values.end()) {
                    entry.values.erase(it);
                    return true;
                }
            }
        }
        
        return false;
    }
    
    bool removeAll(const K& key) {
        int index = hashFunction(key);
        
        auto& bucket = table[index];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->key == key) {
                bucket.erase(it);
                count--;
                return true;
            }
        }
        
        return false;
    }
    
    bool contains(const K& key) const {
        int index = hashFunction(key);
        
        for (const auto& entry : table[index]) {
            if (entry.key == key) {
                return true;
            }
        }
        
        return false;
    }
    
    int getSize() const {
        return count;
    }
    
    vector<K> getAllKeys() const {
        vector<K> keys;
        for (const auto& bucket : table) {
            for (const auto& entry : bucket) {
                keys.push_back(entry.key);
            }
        }
        return keys;
    }
};

#endif