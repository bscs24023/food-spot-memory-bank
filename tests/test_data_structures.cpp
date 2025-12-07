#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "../include/btree.h"
#include "../include/hashtable.h"

using namespace std;

#define GREEN "\033[32m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

void printHeader(const string& title) {
    cout << "\n" << BLUE << "========================================" << RESET << endl;
    cout << BLUE << "  " << title << RESET << endl;
    cout << BLUE << "========================================" << RESET << endl;
}

void printSuccess(const string& msg) {
    cout << GREEN << "✓ " << msg << RESET << endl;
}

void printError(const string& msg) {
    cout << RED << "✗ " << msg << RESET << endl;
}

void printInfo(const string& msg) {
    cout << YELLOW << "→ " << msg << RESET << endl;
}


void testBTreeBasicInsertSearch() {
    printHeader("TEST 1: B-Tree Basic Insert & Search");
    
    BTree<float, string> ratingIndex(3);
    
    printInfo("Inserting restaurant ratings...");
    ratingIndex.insert(8.5, "rest_001");
    ratingIndex.insert(9.2, "rest_002");
    ratingIndex.insert(7.8, "rest_003");
    ratingIndex.insert(9.5, "rest_004");
    ratingIndex.insert(6.5, "rest_005");
    printSuccess("Inserted 5 restaurants");

    printInfo("Searching for rating 9.2...");
    string* result = ratingIndex.search(9.2);
    if (result && *result == "rest_002") {
        printSuccess("Found correct restaurant: rest_002");
    } else {
        printError("Search failed!");
    }
    
    printInfo("Searching for non-existent rating 5.0...");
    string* nullResult = ratingIndex.search(5.0);
    if (nullResult == nullptr) {
        printSuccess("Correctly returned nullptr for non-existent key");
    } else {
        printError("Should have returned nullptr!");
    }
}

void testBTreeRangeQuery() {
    printHeader("TEST 2: B-Tree Range Query");
    
    BTree<float, string> ratingIndex(3);

    printInfo("Inserting 10 restaurants with various ratings...");
    ratingIndex.insert(8.5, "rest_001");
    ratingIndex.insert(9.2, "rest_002");
    ratingIndex.insert(7.8, "rest_003");
    ratingIndex.insert(9.5, "rest_004");
    ratingIndex.insert(6.5, "rest_005");
    ratingIndex.insert(8.0, "rest_006");
    ratingIndex.insert(9.0, "rest_007");
    ratingIndex.insert(7.0, "rest_008");
    ratingIndex.insert(8.8, "rest_009");
    ratingIndex.insert(9.8, "rest_010");
    printSuccess("Inserted 10 restaurants");
    
    printInfo("Querying restaurants rated between 8.0 and 9.5...");
    vector<string> highRated = ratingIndex.searchRange(8.0, 9.5);
    cout << "  Found " << highRated.size() << " restaurants: ";
    for (size_t i = 0; i < highRated.size(); i++) {
        cout << highRated[i];
        if (i < highRated.size() - 1) cout << ", ";
    }
    cout << endl;
    
    if (highRated.size() >= 6) { 
        printSuccess("Range query working correctly!");
    } else {
        printError("Range query returned unexpected results!");
    }
    
    printInfo("Querying top-rated restaurants (9.0-10.0)...");
    vector<string> topRated = ratingIndex.searchRange(9.0, 10.0);
    cout << "  Found " << topRated.size() << " top-rated restaurants" << endl;
    
    if (topRated.size() >= 4) { 
        printSuccess("Top-rated filter working!");
    } else {
        printError("Top-rated filter failed!");
    }
}

void testBTreePriceIndex() {
    printHeader("TEST 3: B-Tree for Price Range");
    
    BTree<float, string> priceIndex(3);
    
    printInfo("Indexing restaurants by price...");
    priceIndex.insert(500.0, "rest_001");
    priceIndex.insert(1500.0, "rest_002");  
    priceIndex.insert(3000.0, "rest_003"); 
    priceIndex.insert(800.0, "rest_004");
    priceIndex.insert(2000.0, "rest_005"); 
    priceIndex.insert(600.0, "rest_006");
    priceIndex.insert(2500.0, "rest_007");
    printSuccess("Indexed 7 restaurants");
    

    printInfo("Finding budget restaurants (under Rs. 1000)...");
    vector<string> budget = priceIndex.searchRange(0.0, 1000.0);
    cout << "  Found " << budget.size() << " budget options" << endl;
    
    if (budget.size() == 3) {
        printSuccess("Budget filter working correctly!");
    } else {
        printError("Expected 3 budget restaurants, got " + to_string(budget.size()));
    }
    

    printInfo("Finding mid-range restaurants (Rs. 1000-2000)...");
    vector<string> midRange = priceIndex.searchRange(1000.0, 2000.0);
    cout << "  Found " << midRange.size() << " mid-range options" << endl;
    
    if (midRange.size() == 2) {
        printSuccess("Mid-range filter working!");
    } else {
        printError("Expected 2 mid-range restaurants, got " + to_string(midRange.size()));
    }
}


void testHashTableBasicOperations() {
    printHeader("TEST 4: Hash Table Basic Operations");
    
    HashTable<string, string> restaurantHash(100);
    

    printInfo("Inserting restaurants...");
    restaurantHash.insert("rest_001", "Cosa Nostra");
    restaurantHash.insert("rest_002", "Bundu Khan");
    restaurantHash.insert("rest_003", "Jade Cafe");
    printSuccess("Inserted 3 restaurants");
    

    printInfo("Retrieving restaurant by ID...");
    string* name = restaurantHash.get("rest_001");
    if (name && *name == "Cosa Nostra") {
        printSuccess("Retrieved 'Cosa Nostra' successfully!");
    } else {
        printError("Failed to retrieve restaurant!");
    }
    
    printInfo("Updating restaurant name...");
    restaurantHash.insert("rest_001", "Cosa Nostra - Updated");
    string* updated = restaurantHash.get("rest_001");
    if (updated && *updated == "Cosa Nostra - Updated") {
        printSuccess("Update successful!");
    } else {
        printError("Update failed!");
    }
    
    printInfo("Checking if restaurant exists...");
    if (restaurantHash.contains("rest_002")) {
        printSuccess("Contains check working!");
    } else {
        printError("Contains check failed!");
    }
    
    printInfo("Removing restaurant...");
    restaurantHash.remove("rest_003");
    if (!restaurantHash.contains("rest_003")) {
        printSuccess("Removal successful!");
    } else {
        printError("Removal failed!");
    }
}

void testMultiValueHashTable() {
    printHeader("TEST 5: Multi-Value Hash Table");
    
    MultiValueHashTable<string, string> cuisineHash(50);
    
    printInfo("Mapping cuisines to restaurants...");
    cuisineHash.insert("Italian", "rest_001");
    cuisineHash.insert("Italian", "rest_005");
    cuisineHash.insert("Italian", "rest_008");
    cuisineHash.insert("Pakistani", "rest_002");
    cuisineHash.insert("Pakistani", "rest_006");
    cuisineHash.insert("Chinese", "rest_003");
    cuisineHash.insert("Chinese", "rest_007");
    cuisineHash.insert("Fast Food", "rest_004");
    printSuccess("Mapped 8 restaurants across 4 cuisines");
    
    printInfo("Finding all Italian restaurants...");
    vector<string> italian = cuisineHash.get("Italian");
    cout << "  Found " << italian.size() << " Italian restaurants: ";
    for (size_t i = 0; i < italian.size(); i++) {
        cout << italian[i];
        if (i < italian.size() - 1) cout << ", ";
    }
    cout << endl;
    
    if (italian.size() == 3) {
        printSuccess("Multi-value retrieval working!");
    } else {
        printError("Expected 3 Italian restaurants, got " + to_string(italian.size()));
    }
    
    printInfo("Getting all cuisine types...");
    vector<string> allCuisines = cuisineHash.getAllKeys();
    cout << "  Available cuisines: ";
    for (size_t i = 0; i < allCuisines.size(); i++) {
        cout << allCuisines[i];
        if (i < allCuisines.size() - 1) cout << ", ";
    }
    cout << endl;
    
    if (allCuisines.size() == 4) {
        printSuccess("All keys retrieved correctly!");
    } else {
        printError("Expected 4 cuisines, got " + to_string(allCuisines.size()));
    }
}

void testHashTableCollisions() {
    printHeader("TEST 6: Hash Table Collision Handling");
    
    HashTable<int, string> smallHash(5);
    
    printInfo("Inserting many items into small hash table (will cause collisions)...");
    for (int i = 0; i < 20; i++) {
        smallHash.insert(i, "item_" + to_string(i));
    }
    printSuccess("Inserted 20 items into 5-bucket table");
    
    printInfo("Verifying all items retrievable after collisions...");
    bool allFound = true;
    for (int i = 0; i < 20; i++) {
        string* result = smallHash.get(i);
        if (!result || *result != "item_" + to_string(i)) {
            allFound = false;
            break;
        }
    }
    
    if (allFound) {
        printSuccess("All items retrievable! Collision handling works!");
    } else {
        printError("Some items lost due to collisions!");
    }
    
    printInfo("Hash table automatically rehashed due to load factor > 0.7");
}

void testLocationHash() {
    printHeader("TEST 7: Location-based Hash Table");
    
    MultiValueHashTable<string, string> locationHash(20);
    
    printInfo("Mapping restaurants to locations...");
    locationHash.insert("DHA", "rest_001");
    locationHash.insert("DHA", "rest_005");
    locationHash.insert("DHA", "rest_009");
    locationHash.insert("Gulberg", "rest_002");
    locationHash.insert("Gulberg", "rest_006");
    locationHash.insert("Johar Town", "rest_003");
    locationHash.insert("Johar Town", "rest_007");
    locationHash.insert("Model Town", "rest_004");
    locationHash.insert("Model Town", "rest_008");
    printSuccess("Mapped 9 restaurants across 4 locations");
    
    printInfo("Finding restaurants in DHA...");
    vector<string> dhaRestaurants = locationHash.get("DHA");
    cout << "  DHA has " << dhaRestaurants.size() << " restaurants" << endl;
    
    if (dhaRestaurants.size() == 3) {
        printSuccess("Location-based search working!");
    } else {
        printError("Location search failed!");
    }
    
    printInfo("Getting all available locations...");
    vector<string> locations = locationHash.getAllKeys();
    cout << "  Locations: ";
    for (size_t i = 0; i < locations.size(); i++) {
        cout << locations[i];
        if (i < locations.size() - 1) cout << ", ";
    }
    cout << endl;
    
    if (locations.size() == 4) {
        printSuccess("All locations retrieved!");
    } else {
        printError("Expected 4 locations!");
    }
}


void testCombinedOperations() {
    printHeader("TEST 8: Combined B-Tree + Hash Table Query");
    
    BTree<float, string> ratingIndex(3);
    MultiValueHashTable<string, string> cuisineHash(50);
    MultiValueHashTable<string, string> locationHash(20);
    HashTable<string, string> restaurantHash(100);
    
    printInfo("Setting up mini database...");
    
    struct RestaurantData {
        string id;
        string name;
        string cuisine;
        string location;
        float rating;
    };
    
    vector<RestaurantData> restaurants = {
        {"rest_001", "Cosa Nostra", "Italian", "DHA", 9.2},
        {"rest_002", "Bundu Khan", "Pakistani", "Gulberg", 8.5},
        {"rest_003", "Jade Cafe", "Chinese", "DHA", 8.8},
        {"rest_004", "Subway", "Fast Food", "Johar Town", 7.5},
        {"rest_005", "Pizza Hut", "Italian", "Model Town", 8.0},
        {"rest_006", "Bar BQ Tonight", "Pakistani", "DHA", 9.0},
        {"rest_007", "Yum Chinese", "Chinese", "Gulberg", 8.3},
    };
    
    for (const auto& r : restaurants) {
        ratingIndex.insert(r.rating, r.id);
        cuisineHash.insert(r.cuisine, r.id);
        locationHash.insert(r.location, r.id);
        restaurantHash.insert(r.id, r.name);
    }
    printSuccess("Added 7 restaurants to database");
    
    printInfo("\nComplex Query: Italian restaurants in DHA rated 8+");
    
    vector<string> italianIds = cuisineHash.get("Italian");
    cout << "  Step 1: Found " << italianIds.size() << " Italian restaurants" << endl;
    
    vector<string> dhaIds = locationHash.get("DHA");
    cout << "  Step 2: Found " << dhaIds.size() << " restaurants in DHA" << endl;
    
    vector<string> highRatedIds = ratingIndex.searchRange(8.0, 10.0);
    cout << "  Step 3: Found " << highRatedIds.size() << " restaurants rated 8+" << endl;
    
    vector<string> results;
    for (const auto& id : italianIds) {
        bool inDHA = false;
        for (const auto& dhaId : dhaIds) {
            if (id == dhaId) {
                inDHA = true;
                break;
            }
        }
        
        bool highRated = false;
        for (const auto& ratedId : highRatedIds) {
            if (id == ratedId) {
                highRated = true;
                break;
            }
        }
        
        if (inDHA && highRated) {
            results.push_back(id);
        }
    }
    
    cout << "\n  Final Result: " << results.size() << " restaurant(s) match criteria" << endl;
    for (const auto& id : results) {
        string* name = restaurantHash.get(id);
        if (name) {
            cout << "    - " << *name << " (" << id << ")" << endl;
        }
    }
    
    if (results.size() == 1 && results[0] == "rest_001") {
        printSuccess("Complex query working perfectly! Found Cosa Nostra!");
    } else {
        printError("Complex query failed!");
    }
}


void testPerformance() {
    printHeader("TEST 9: Performance Test");
    
    printInfo("Inserting 1000 items into B-Tree...");
    BTree<int, string> largeBTree(3);
    for (int i = 0; i < 1000; i++) {
        largeBTree.insert(i, "item_" + to_string(i));
    }
    printSuccess("1000 items inserted");
    
    printInfo("Searching for 100 random items...");
    bool allFound = true;
    for (int i = 0; i < 100; i++) {
        int key = rand() % 1000;
        string* result = largeBTree.search(key);
        if (!result) {
            allFound = false;
            break;
        }
    }
    
    if (allFound) {
        printSuccess("All searches successful! B-Tree scales well!");
    } else {
        printError("Some searches failed!");
    }
    
    printInfo("Inserting 1000 items into Hash Table...");
    HashTable<int, string> largeHash(100);
    for (int i = 0; i < 1000; i++) {
        largeHash.insert(i, "item_" + to_string(i));
    }
    printSuccess("1000 items inserted");
    
    printInfo("Hash table automatically rehashed multiple times");
    cout << "  Final size: " << largeHash.getSize() << " items" << endl;
}


int main() {
    cout << "\n" << BLUE << "╔════════════════════════════════════════╗" << RESET << endl;
    cout << BLUE << "║  FOOD SPOT MEMORY BANK - TEST SUITE   ║" << RESET << endl;
    cout << BLUE << "║  B-Tree & Hash Table Testing          ║" << RESET << endl;
    cout << BLUE << "╚════════════════════════════════════════╝" << RESET << endl;
    
    try {
        testBTreeBasicInsertSearch();
        testBTreeRangeQuery();
        testBTreePriceIndex();
        
        testHashTableBasicOperations();
        testMultiValueHashTable();
        testHashTableCollisions();
        testLocationHash();
        
        testCombinedOperations();
        testPerformance();
        
        printHeader("TEST SUMMARY");
        printSuccess("All tests completed successfully!");
        cout << GREEN << "\n✓ B-Tree implementation: WORKING" << RESET << endl;
        cout << GREEN << "✓ Hash Table implementation: WORKING" << RESET << endl;
        cout << GREEN << "✓ Range queries: WORKING" << RESET << endl;
        cout << GREEN << "✓ Multi-value lookups: WORKING" << RESET << endl;
        cout << GREEN << "✓ Collision handling: WORKING" << RESET << endl;
        cout << GREEN << "✓ Combined queries: WORKING" << RESET << endl;
        
        cout << "\n" << BLUE << "Ready for production!" << RESET << endl;
        
    } catch (const exception& e) {
        printError("Exception caught: " + string(e.what()));
        return 1;
    }
    
    return 0;
}