#ifndef FOOD_SPOT_STRUCTURES_H
#define FOOD_SPOT_STRUCTURES_H

#include <string>
#include <vector>
#include <ctime>
#include <iostream>

using namespace std;

struct Dish {
    string dishName;
    float rating;
    float price;
    
    Dish() : dishName(""), rating(0.0), price(0.0) {}
    
    Dish(string name, float r, float p) : dishName(name), rating(r), price(p) {}
    
    void display() const {
        cout << "  - " << dishName 
             << " (Rating: " << rating 
             << "/10, Price: Rs." << price << ")" << endl;
    }
};

struct Restaurant {
    string restaurantId;
    string name;
    string location;
    vector<string> cuisineTypes;
    
    float overallRating;
    float averagePrice;
    
    vector<Dish> dishes;
    time_t lastVisitDate;
    int totalVisits;
    string notes;
    
    Restaurant() : restaurantId(""), name(""), location(""), overallRating(0.0), averagePrice(0.0),lastVisitDate(0), totalVisits(0), notes("") {}
    
    Restaurant(string id, string n, string loc, vector<string> cuisines,float rating, float avgPrice): restaurantId(id), name(n), location(loc), cuisineTypes(cuisines),overallRating(rating), averagePrice(avgPrice),lastVisitDate(time(nullptr)), totalVisits(1), notes("") {}
    
    void addDish(const Dish& dish) {
        dishes.push_back(dish);
        updateAveragePrice();
    }
    
    void updateAveragePrice() {
        if (dishes.empty()) {
            averagePrice = 0.0;
            return;
        }
        float total = 0.0;
        for (const auto& dish : dishes) {
            total += dish.price;
        }
        averagePrice = total / dishes.size();
    }
    
    void display() const {
        cout << endl;
        cout << "Restaurant: " << name << " (ID: " << restaurantId << ")" << endl;
        cout << "Location: " << location << endl;
        cout << "Cuisine: ";
        for (size_t i = 0; i < cuisineTypes.size(); i++) {
            cout << cuisineTypes[i];
            if (i < cuisineTypes.size() - 1) cout << ", ";
        }
        cout << endl;
        cout << "Rating: " << overallRating << "/10" << endl;
        cout << "Average Price: Rs." << averagePrice << endl;
        cout << "Total Visits: " << totalVisits << endl;
        
        if (!dishes.empty()) {
            cout << "\nDishes Tried:" << endl;
            for (const auto& dish : dishes) {
                dish.display();
            }
        }
        
        if (!notes.empty()) {
            cout << "\nNotes: " << notes << endl;
        }
    }
};

#endif