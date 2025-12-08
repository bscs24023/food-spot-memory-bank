#include <iostream>
#include <limits>
#include "../include/disk_database.h"

using namespace std;

void displayMenu() {
    cout << "  FOOD SPOT MEMORY BANK " << endl;
    cout << "1. Add New Restaurant" << endl;
    cout << "2. Search by Rating Range" << endl;
    cout << "3. Search by Price Range" << endl;
    cout << "4. Search by Cuisine" << endl;
    cout << "5. Search by Location" << endl;
    cout << "6. Display All Restaurants" << endl;
    cout << "7. Exit" << endl;
    cout << endl;
    cout << "Choice: ";
}

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void addNewRestaurant(DiskDatabase& db) {
    cout << "Add New Restaurant" << endl;
    
    string name, location;
    float rating;
    int numCuisines, numDishes;
    
    clearInput();
    
    cout << "Restaurant Name: ";
    getline(cin, name);
    
    cout << "Location: ";
    getline(cin, location);
    
    cout << "Number of Cuisines: ";
    cin >> numCuisines;
    clearInput();
    
    vector<string> cuisines;
    for (int i = 0; i < numCuisines; i++) {
        string cuisine;
        cout << "  Cuisine " << (i+1) << ": ";
        getline(cin, cuisine);
        cuisines.push_back(cuisine);
    }
    
    cout << "Overall Rating (1-10): ";
    cin >> rating;
    
    cout << "Number of Dishes: ";
    cin >> numDishes;
    clearInput();
    
    vector<Dish> dishes;
    float totalPrice = 0;
    
    for (int i = 0; i < numDishes; i++) {
        string dishName;
        float dishRating, dishPrice;
        
        cout << endl;
        cout << "Dish " << (i+1) << " Name: ";
        getline(cin, dishName);
        
        cout << "  Rating (1-10): ";
        cin >> dishRating;
        
        cout << "  Price (Rs.): ";
        cin >> dishPrice;
        clearInput();
        
        dishes.push_back(Dish(dishName, dishRating, dishPrice));
        totalPrice += dishPrice;
    }
    
    float avgPrice = numDishes > 0 ? totalPrice / numDishes : 0;
    
    string notes;
    cout << endl;
    cout << "Notes (optional): ";
    getline(cin, notes);
    
    db.addRestaurant(name, location, cuisines, rating, avgPrice, dishes, notes);
}

int main() 
{ 
    DiskDatabase db("restaurants_data.dat");
    
    int choice;
    bool running = true;
    
    while (running) {
        displayMenu();
        
        if (!(cin >> choice)) 
        {
            clearInput();
            cout << "Invalid input." << endl;
            continue;
        }
        
        switch (choice) {
            case 1:
                addNewRestaurant(db);
                break;
                
            case 2: {
                float minRating, maxRating;
                cout << "\nMin Rating: ";
                cin >> minRating;
                cout << "Max Rating: ";
                cin >> maxRating;
                
                vector<Restaurant> results = db.searchByRatingRange(minRating, maxRating);
                cout << "\nFound " << results.size() << " restaurant(s):" << endl;
                for (const auto& r : results) {
                    r.display();
                }
                break;
            }
                
            case 3: {
                float minPrice, maxPrice;
                cout << "\nMin Price: ";
                cin >> minPrice;
                cout << "Max Price: ";
                cin >> maxPrice;
                
                vector<Restaurant> results = db.searchByPriceRange(minPrice, maxPrice);
                cout << "\nFound " << results.size() << " restaurant(s):" << endl;
                for (const auto& r : results) {
                    r.display();
                }
                break;
            }
                
            case 4: {
                string cuisine;
                clearInput();
                cout << "\nCuisine: ";
                getline(cin, cuisine);
                
                vector<Restaurant> results = db.searchByCuisine(cuisine);
                cout << "\nFound " << results.size() << " restaurant(s):" << endl;
                for (const auto& r : results) {
                    r.display();
                }
                break;
            }
                
            case 5: {
                string location;
                clearInput();
                cout << "\nLocation: ";
                getline(cin, location);
                
                vector<Restaurant> results = db.searchByLocation(location);
                cout << "\nFound " << results.size() << " restaurant(s):" << endl;
                for (const auto& r : results) {
                    r.display();
                }
                break;
            }
                
            case 6:
                db.displayAll();
                break;
                
            case 7:
                cout << "Exiting";
                running = false;
                break;
                
            default:
                cout << "Invalid choice" << endl;
        }
    }
    
    return 0;
}