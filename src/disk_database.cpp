#include "../include/disk_database.h"
#include <iostream>
#include <algorithm>

using namespace std;

void DiskDatabase::writeString(ofstream& file, const string& str) {
    int len = str.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(str.c_str(), len);
}

string DiskDatabase::readString(ifstream& file) {
    int len;
    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    
    if (len <= 0 || len > 10000) {
        return "";
    }
    
    char* buffer = new char[len + 1];
    file.read(buffer, len);
    buffer[len] = '\0';
    
    string result(buffer);
    delete[] buffer;
    
    return result;
}

void DiskDatabase::writeStringVector(ofstream& file, const vector<string>& vec) {
    int size = vec.size();
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));
    
    for (const auto& str : vec) 
    {
        writeString(file, str);
    }
}

vector<string> DiskDatabase::readStringVector(ifstream& file) {
    int size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));
    
    vector<string> result;
    for (int i = 0; i < size; i++) {
        result.push_back(readString(file));
    }
    
    return result;
}

void DiskDatabase::writeDish(ofstream& file, const Dish& dish) {
    writeString(file, dish.dishName);
    file.write(reinterpret_cast<const char*>(&dish.rating), sizeof(dish.rating));
    file.write(reinterpret_cast<const char*>(&dish.price), sizeof(dish.price));
}

Dish DiskDatabase::readDish(ifstream& file) {
    Dish dish;
    dish.dishName = readString(file);
    file.read(reinterpret_cast<char*>(&dish.rating), sizeof(dish.rating));
    file.read(reinterpret_cast<char*>(&dish.price), sizeof(dish.price));
    return dish;
}

void DiskDatabase::writeDishVector(ofstream& file, const vector<Dish>& dishes) {
    int size = dishes.size();
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));
    
    for (const auto& dish : dishes) {
        writeDish(file, dish);
    }
}

vector<Dish> DiskDatabase::readDishVector(ifstream& file) {
    int size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));
    
    vector<Dish> result;
    for (int i = 0; i < size; i++) {
        result.push_back(readDish(file));
    }
    
    return result;
}


DiskDatabase::DiskDatabase(const string& filepath) : dataFilePath(filepath), ratingIndex(3), priceIndex(3),idIndex(1000), cuisineIndex(500), locationIndex(200), nextId(1) 
{
    cout << "Data file: " << dataFilePath << endl;
    
    ifstream testFile(dataFilePath, ios::binary);
    if (testFile.good()) 
    {
        testFile.close();
        cout << "data file found" << endl;
        rebuildIndexes();
        cout << "Indexes built" << endl;
    } 
    else 
    {
        cout << "No existing file." << endl;
    }

}

DiskDatabase::~DiskDatabase() 
{
    //cout << "Database closed" << endl;
}

string DiskDatabase::generateId() 
{
    return "rest_" + to_string(nextId++);
}

FileOffset DiskDatabase::writeRestaurantToDisk(const Restaurant& r) {
    ofstream file(dataFilePath, ios::binary | ios::app);
    if (!file) {
        cerr << "Error. can not open file" << endl;
        return -1;
    }
    
    FileOffset offset = file.tellp();
    
    writeString(file, r.restaurantId);
    writeString(file, r.name);
    writeString(file, r.location);
    writeStringVector(file, r.cuisineTypes);
    
    file.write(reinterpret_cast<const char*>(&r.overallRating), sizeof(r.overallRating));
    file.write(reinterpret_cast<const char*>(&r.averagePrice), sizeof(r.averagePrice));
    
    writeDishVector(file, r.dishes);
    
    file.write(reinterpret_cast<const char*>(&r.lastVisitDate), sizeof(r.lastVisitDate));
    file.write(reinterpret_cast<const char*>(&r.totalVisits), sizeof(r.totalVisits));
    
    writeString(file, r.notes);
    
    file.close();
    
    //cout << "Written to disk at offset: " << offset << endl;
    
    return offset;
}

Restaurant DiskDatabase::readRestaurantFromDisk(FileOffset offset) {
    Restaurant r;
    
    ifstream file(dataFilePath, ios::binary);
    if (!file) {
        cerr << "Error. cannot open file" << endl;
        return r;
    }
    
    file.seekg(offset);
    
    r.restaurantId = readString(file);
    r.name = readString(file);
    r.location = readString(file);
    r.cuisineTypes = readStringVector(file);
    
    file.read(reinterpret_cast<char*>(&r.overallRating), sizeof(r.overallRating));
    file.read(reinterpret_cast<char*>(&r.averagePrice), sizeof(r.averagePrice));
    
    r.dishes = readDishVector(file);
    
    file.read(reinterpret_cast<char*>(&r.lastVisitDate), sizeof(r.lastVisitDate));
    file.read(reinterpret_cast<char*>(&r.totalVisits), sizeof(r.totalVisits));
    
    r.notes = readString(file);
    
    file.close();
    
    return r;
}

void DiskDatabase::rebuildIndexes() {
    ifstream file(dataFilePath, ios::binary);
    if (!file) {
        return;
    }
    
    int count = 0;
    
    while (file.good() && file.peek() != EOF) {
        FileOffset offset = file.tellg();
        
        Restaurant r;
        r.restaurantId = readString(file);
        if (r.restaurantId.empty()) break;
        
        r.name = readString(file);
        r.location = readString(file);
        r.cuisineTypes = readStringVector(file);
        
        file.read(reinterpret_cast<char*>(&r.overallRating), sizeof(r.overallRating));
        file.read(reinterpret_cast<char*>(&r.averagePrice), sizeof(r.averagePrice));
        
        r.dishes = readDishVector(file);
        
        file.read(reinterpret_cast<char*>(&r.lastVisitDate), sizeof(r.lastVisitDate));
        file.read(reinterpret_cast<char*>(&r.totalVisits), sizeof(r.totalVisits));
        
        r.notes = readString(file);
        
        ratingIndex.insert(r.overallRating, offset);
        priceIndex.insert(r.averagePrice, offset);
        idIndex.insert(r.restaurantId, offset);
        
        for (const auto& cuisine : r.cuisineTypes) {
            cuisineIndex.insert(cuisine, offset);
        }
        
        locationIndex.insert(r.location, offset);
        
        string idNum = r.restaurantId.substr(5);
        int num = stoi(idNum);
        if (num >= nextId) {
            nextId = num + 1;
        }
        
        count++;
    }
    
    file.close();
    
    //cout << "Indexing done" << endl;
}

string DiskDatabase::addRestaurant(const string& name, const string& location,const vector<string>& cuisineTypes, float rating,float avgPrice, const vector<Dish>& dishes,const string& notes) {
    string id = generateId();
    
    Restaurant restaurant;
    restaurant.restaurantId = id;
    restaurant.name = name;
    restaurant.location = location;
    restaurant.cuisineTypes = cuisineTypes;
    restaurant.overallRating = rating;
    restaurant.averagePrice = avgPrice;
    restaurant.dishes = dishes;
    restaurant.lastVisitDate = time(nullptr);
    restaurant.totalVisits = 1;
    restaurant.notes = notes;
    
    if (!dishes.empty()) {
        restaurant.updateAveragePrice();
    }
    
    FileOffset offset = writeRestaurantToDisk(restaurant);
    
    if (offset < 0) {
        cerr << "Failed to write to disk" << endl;
        return "";
    }
    
    ratingIndex.insert(rating, offset);
    priceIndex.insert(avgPrice, offset);
    idIndex.insert(id, offset);
    
    for (const auto& cuisine : cuisineTypes) {
        cuisineIndex.insert(cuisine, offset);
    }
    
    locationIndex.insert(location, offset);
    
    cout << "Restaurant added: " << id << endl;
    
    return id;
}

vector<Restaurant> DiskDatabase::searchByRatingRange(float minRating, float maxRating) 
{
    vector<FileOffset> offsets = ratingIndex.searchRange(minRating, maxRating);
    
    cout << "Found " << offsets.size() << " matches in index" << endl;
    cout << "Reading from disk." << endl;
    
    vector<Restaurant> results;
    for (const auto& offset : offsets) 
    {
        results.push_back(readRestaurantFromDisk(offset));
    }
    
    return results;
}

vector<Restaurant> DiskDatabase::searchByPriceRange(float minPrice, float maxPrice) 
{
    vector<FileOffset> offsets = priceIndex.searchRange(minPrice, maxPrice);
    
    cout << "Found " << offsets.size() << " matches in index" << endl;
    cout << "Reading from disk." << endl;
    
    vector<Restaurant> results;
    for (const auto& offset : offsets) {
        results.push_back(readRestaurantFromDisk(offset));
    }
    
    return results;
}

vector<Restaurant> DiskDatabase::searchByCuisine(const string& cuisine) {
    vector<FileOffset> offsets = cuisineIndex.get(cuisine);
    
    cout << "Found " << offsets.size() << " matches in index" << endl;
    cout << "Reading from disk." << endl;
    
    vector<Restaurant> results;
    for (const auto& offset : offsets) {
        results.push_back(readRestaurantFromDisk(offset));
    }
    
    return results;
}

vector<Restaurant> DiskDatabase::searchByLocation(const string& location) {
    vector<FileOffset> offsets = locationIndex.get(location);
    
    cout << "Found " << offsets.size() << " matches in index" << endl;
    cout << "Reading from disk." << endl;
    
    vector<Restaurant> results;
    for (const auto& offset : offsets) {
        results.push_back(readRestaurantFromDisk(offset));
    }
    
    return results;
}

Restaurant DiskDatabase::getRestaurant(const string& id) {
    FileOffset* offsetPtr = idIndex.get(id);
    
    if (!offsetPtr) {
        Restaurant empty;
        return empty;
    }
    
    return readRestaurantFromDisk(*offsetPtr);
}

void DiskDatabase::displayAll() {
    ifstream file(dataFilePath, ios::binary);
    if (!file) {
        cout << "\nNo restaurants in database." << endl;
        return;
    }
    
    int count = 0;
    
    cout << "  ALL RESTAURANTS:" << endl;
    
    while (file.good() && file.peek() != EOF) {
        FileOffset offset = file.tellg();
        file.close();
        
        Restaurant r = readRestaurantFromDisk(offset);
        if (r.restaurantId.empty()) break;
        
        r.display();
        count++;
        
        file.open(dataFilePath, ios::binary);
        file.seekg(offset);
        
        readString(file);
        readString(file);
        readString(file);
        readStringVector(file);
        float temp;
        file.read(reinterpret_cast<char*>(&temp), sizeof(temp));
        file.read(reinterpret_cast<char*>(&temp), sizeof(temp));
        readDishVector(file);
        time_t t;
        file.read(reinterpret_cast<char*>(&t), sizeof(t));
        int v;
        file.read(reinterpret_cast<char*>(&v), sizeof(v));
        readString(file);
    }
    
    file.close();
    
    cout << "\nTotal: " << count << " restaurants" << endl;
}

int DiskDatabase::getTotalRestaurants() const {
    return nextId - 1;
}
vector<Restaurant> DiskDatabase::getAllRestaurants() {
    vector<Restaurant> restaurants;
    
    ifstream file(dataFilePath, ios::binary);
    if (!file.good()) {
        cout << "  File not found: " << dataFilePath << endl;
        return restaurants;
    }
    
    cout << "  Reading from: " << dataFilePath << endl;
    
    while (file.good() && file.peek() != EOF) {
        FileOffset offset = file.tellg();
        
        Restaurant r;
        r.restaurantId = readString(file);
        if (r.restaurantId.empty()) break;
        
        r.name = readString(file);
        r.location = readString(file);
        r.cuisineTypes = readStringVector(file);
        
        file.read(reinterpret_cast<char*>(&r.overallRating), sizeof(r.overallRating));
        file.read(reinterpret_cast<char*>(&r.averagePrice), sizeof(r.averagePrice));
        
        r.dishes = readDishVector(file);
        
        file.read(reinterpret_cast<char*>(&r.lastVisitDate), sizeof(r.lastVisitDate));
        file.read(reinterpret_cast<char*>(&r.totalVisits), sizeof(r.totalVisits));
        
        r.notes = readString(file);
        
        restaurants.push_back(r);
        
        cout << "    Read: " << r.name << " (ID: " << r.restaurantId << ")" << endl;
    }
    
    file.close();
    cout << "  Total restaurants read: " << restaurants.size() << endl;
    return restaurants;
}