#include "../include/recommendation_system.h"
#include "../include/disk_database.h"
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

RecommendationSystem::RecommendationSystem(UserManager* um)
    : userManager(um) {
    cout << "Initializing Recommendation System..." << endl;
}

void RecommendationSystem::updatePreferences(const string& userID, 
                                             const vector<string>& cuisines) {
    for (const auto& cuisine : cuisines) {
        userCuisinePreferences[userID][cuisine]++;
    }
}

vector<string> RecommendationSystem::getTopCuisines(const string& userID, int topN) {
    auto& cuisineMap = userCuisinePreferences[userID];
    
    if (cuisineMap.empty()) {
        return vector<string>();
    }
    
    priority_queue<CuisinePreference> pq;
    
    for (const auto& pair : cuisineMap) {
        pq.push(CuisinePreference(pair.first, pair.second));
    }
    
    vector<string> topCuisines;
    for (int i = 0; i < topN && !pq.empty(); i++) {
        topCuisines.push_back(pq.top().cuisine);
        pq.pop();
    }
    
    return topCuisines;
}

float RecommendationSystem::calculateScore(const Restaurant& restaurant,
                                          const vector<string>& preferredCuisines) {
    float score = 0.0;
    
    score += restaurant.overallRating * 10;
    
    for (const auto& cuisine : restaurant.cuisineTypes) {
        for (size_t i = 0; i < preferredCuisines.size(); i++) {
            if (cuisine == preferredCuisines[i]) {
                score += (50 - i * 10);
                break;
            }
        }
    }
    
    return score;
}

vector<Restaurant> RecommendationSystem::loadUserRestaurants(const string& userID) {
    vector<Restaurant> restaurants;
    
    string username = userID.substr(5); 
    
    string dbPath = "data/users/user_" + username + ".dat";
    
    ifstream file(dbPath, ios::binary);
    if (!file.good()) {
        return restaurants;
    }

    while (file.good() && file.peek() != EOF) {
        Restaurant r;
        
        try {
            int len;
            file.read(reinterpret_cast<char*>(&len), sizeof(len));
            if (len <= 0 || len > 1000) break;
            r.restaurantId.resize(len);
            file.read(&r.restaurantId[0], len);
            
            file.read(reinterpret_cast<char*>(&len), sizeof(len));
            if (len <= 0 || len > 1000) break;
            r.name.resize(len);
            file.read(&r.name[0], len);
            
            file.read(reinterpret_cast<char*>(&len), sizeof(len));
            if (len <= 0 || len > 1000) break;
            r.location.resize(len);
            file.read(&r.location[0], len);
            
            int cuisineCount;
            file.read(reinterpret_cast<char*>(&cuisineCount), sizeof(cuisineCount));
            for (int i = 0; i < cuisineCount && i < 20; i++) {
                file.read(reinterpret_cast<char*>(&len), sizeof(len));
                if (len <= 0 || len > 1000) break;
                string cuisine(len, ' ');
                file.read(&cuisine[0], len);
                r.cuisineTypes.push_back(cuisine);
            }
            
            file.read(reinterpret_cast<char*>(&r.overallRating), sizeof(r.overallRating));
            file.read(reinterpret_cast<char*>(&r.averagePrice), sizeof(r.averagePrice));
            
            int dishCount;
            file.read(reinterpret_cast<char*>(&dishCount), sizeof(dishCount));
            for (int i = 0; i < dishCount && i < 50; i++) {
                Dish dish;
                
                file.read(reinterpret_cast<char*>(&len), sizeof(len));
                if (len <= 0 || len > 1000) break;
                dish.dishName.resize(len);
                file.read(&dish.dishName[0], len);
                
                file.read(reinterpret_cast<char*>(&dish.rating), sizeof(dish.rating));
                file.read(reinterpret_cast<char*>(&dish.price), sizeof(dish.price));
                
                r.dishes.push_back(dish);
            }
            
            file.read(reinterpret_cast<char*>(&r.lastVisitDate), sizeof(r.lastVisitDate));
            file.read(reinterpret_cast<char*>(&r.totalVisits), sizeof(r.totalVisits));
            
            file.read(reinterpret_cast<char*>(&len), sizeof(len));
            if (len > 0 && len < 1000) {
                r.notes.resize(len);
                file.read(&r.notes[0], len);
            }
            
            restaurants.push_back(r);
            
        } catch (...) {
            break;
        }
    }
    
    file.close();
    return restaurants;
}

vector<Restaurant> RecommendationSystem::loadFriendsRestaurants(const string& userID) {
    vector<Restaurant> allRestaurants;
    
    vector<string> friendIDs = userManager->getFriends(userID);
    
    if (friendIDs.empty()) {
        return allRestaurants;
    }
    
    cout << "Loading restaurants from " << friendIDs.size() << " friend(s)..." << endl;
    
    for (const auto& friendID : friendIDs) {
        vector<Restaurant> friendRestaurants = loadUserRestaurants(friendID);
        
        if (!friendRestaurants.empty()) {
            cout << "   • " << friendID.substr(5) << ": " 
                 << friendRestaurants.size() << " restaurants" << endl;
        }
        
        allRestaurants.insert(allRestaurants.end(), 
                             friendRestaurants.begin(), 
                             friendRestaurants.end());
    }
    
    return allRestaurants;
}

vector<ScoredRestaurant> RecommendationSystem::getRecommendationsFromFriends(
    const string& userID, int limit) {
    
    cout << "\nGenerating personalized recommendations..." << endl;
    
    vector<string> topCuisines = getTopCuisines(userID, 3);
    
    if (topCuisines.empty()) {
        cout << "You haven't logged any restaurants yet!" << endl;
        cout << "   Add some restaurants to get personalized recommendations." << endl;
        return vector<ScoredRestaurant>();
    }
    
    cout << "Your top cuisines: ";
    for (size_t i = 0; i < topCuisines.size(); i++) {
        cout << topCuisines[i];
        if (i < topCuisines.size() - 1) cout << ", ";
    }
    cout << endl;
    
    vector<Restaurant> friendsRestaurants = loadFriendsRestaurants(userID);
    
    if (friendsRestaurants.empty()) {
        cout << "  Your friends haven't logged any restaurants yet!" << endl;
        return vector<ScoredRestaurant>();
    }
    
    cout << "Total restaurants from friends: " << friendsRestaurants.size() << endl;
    
    vector<ScoredRestaurant> scoredRestaurants;
    
    for (const auto& restaurant : friendsRestaurants) {
        float score = calculateScore(restaurant, topCuisines);
        
        scoredRestaurants.push_back(
            ScoredRestaurant(restaurant, score, "Friend", "")
        );
    }
    
    sort(scoredRestaurants.begin(), scoredRestaurants.end());
    
    if (scoredRestaurants.size() > (size_t)limit) {
        scoredRestaurants.resize(limit);
    }
    
    return scoredRestaurants;
}

vector<ScoredRestaurant> RecommendationSystem::getRecommendationsByCity(
    const string& userID, const string& city, int limit) {
    
    cout << "\n Finding top restaurants in " << city << "..." << endl;
    
    vector<string> topCuisines = getTopCuisines(userID, 3);
    
    vector<User> allUsers = userManager->getAllUsers();
    
    vector<Restaurant> cityRestaurants;
    
    for (const auto& user : allUsers) {
        if (user.userID == userID) continue; 
        
        vector<Restaurant> userRestaurants = loadUserRestaurants(user.userID);
        
        for (const auto& restaurant : userRestaurants) {
            string lowerLocation = restaurant.location;
            string lowerCity = city;
            transform(lowerLocation.begin(), lowerLocation.end(), 
                     lowerLocation.begin(), ::tolower);
            transform(lowerCity.begin(), lowerCity.end(), 
                     lowerCity.begin(), ::tolower);
            
            if (lowerLocation.find(lowerCity) != string::npos) {
                cityRestaurants.push_back(restaurant);
            }
        }
    }
    
    if (cityRestaurants.empty()) {
        cout << "  No restaurants found in " << city << endl;
        return vector<ScoredRestaurant>();
    }
    
    cout << " Found " << cityRestaurants.size() << " restaurants in " << city << endl;
    
    vector<ScoredRestaurant> scoredRestaurants;
    
    for (const auto& restaurant : cityRestaurants) {
        float score;
        
        if (!topCuisines.empty()) {
            score = calculateScore(restaurant, topCuisines);
        } else {
            score = restaurant.overallRating * 10;
        }
        
        scoredRestaurants.push_back(
            ScoredRestaurant(restaurant, score, "User", "")
        );
    }
    
    sort(scoredRestaurants.begin(), scoredRestaurants.end());
    
    if (scoredRestaurants.size() > (size_t)limit) {
        scoredRestaurants.resize(limit);
    }
    
    return scoredRestaurants;
}

void RecommendationSystem::displayUserPreferences(const string& userID) {
    auto& cuisineMap = userCuisinePreferences[userID];
    
    if (cuisineMap.empty()) {
        cout << "\nNo preferences yet - add some restaurants!" << endl;
        return;
    }
    
    cout << "\n========================================" << endl;
    cout << "   YOUR CUISINE PREFERENCES" << endl;
    cout << "========================================" << endl;
    
    vector<pair<string, int>> prefs(cuisineMap.begin(), cuisineMap.end());
    sort(prefs.begin(), prefs.end(), 
         [](const pair<string, int>& a, const pair<string, int>& b) {
             return a.second > b.second;
         });
    
    for (const auto& pair : prefs) {
        cout << "  " << pair.first << ": " << pair.second << " time(s)" << endl;
    }
    cout << "========================================" << endl;
}