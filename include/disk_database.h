#ifndef DISK_DATABASE_H
#define DISK_DATABASE_H

#include "btree.h"
#include "hashtable.h"
#include "food_spot_structures.h"
#include <fstream>
#include <string>
#include <vector>

using namespace std;
typedef long long FileOffset;

class DiskDatabase {
private:
    string dataFilePath;
    
    BTree<float, FileOffset> ratingIndex;
    BTree<float, FileOffset> priceIndex;
    
    HashTable<string, FileOffset> idIndex;
    MultiValueHashTable<string, FileOffset> cuisineIndex;
    MultiValueHashTable<string, FileOffset> locationIndex;
    
    int nextId;
    
    string generateId();
    FileOffset writeRestaurantToDisk(const Restaurant& r);
    Restaurant readRestaurantFromDisk(FileOffset offset);
    void rebuildIndexes();
    
    void writeString(ofstream& file, const string& str);
    string readString(ifstream& file);
    void writeStringVector(ofstream& file, const vector<string>& vec);
    vector<string> readStringVector(ifstream& file);
    void writeDish(ofstream& file, const Dish& dish);
    Dish readDish(ifstream& file);
    void writeDishVector(ofstream& file, const vector<Dish>& dishes);
    vector<Dish> readDishVector(ifstream& file);

public:
    DiskDatabase(const string& filepath = "restaurants_data.dat");
    ~DiskDatabase();
    
    string addRestaurant(const string& name, const string& location,const vector<string>& cuisineTypes, float rating, float avgPrice, const vector<Dish>& dishes, const string& notes = "");
    
    vector<Restaurant> searchByRatingRange(float minRating, float maxRating);
    vector<Restaurant> searchByPriceRange(float minPrice, float maxPrice);
    vector<Restaurant> searchByCuisine(const string& cuisine);
    vector<Restaurant> searchByLocation(const string& location);
    
    Restaurant getRestaurant(const string& id);
    
    void displayAll();
    
    int getTotalRestaurants() const;
    vector<Restaurant> getAllRestaurants();
};

#endif