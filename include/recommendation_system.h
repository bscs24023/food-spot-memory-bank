#ifndef RECOMMENDATION_SYSTEM_H
#define RECOMMENDATION_SYSTEM_H

#include "food_spot_structures.h"
#include "user_manager.h"
#include <unordered_map>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

struct CuisinePreference {
    string cuisine;
    int count;
    
    CuisinePreference(string c, int cnt) : cuisine(c), count(cnt) {}
    
    bool operator<(const CuisinePreference& other) const {
        return count < other.count;
    }
};
struct ScoredRestaurant {
    Restaurant restaurant;
    float score;
    string ownerUsername;
    string ownerUserID;
    
    ScoredRestaurant() 
        : restaurant(), score(0.0), ownerUsername(""), ownerUserID("") {}
    
    ScoredRestaurant(Restaurant r, float s, string owner, string ownerID)
        : restaurant(r), score(s), ownerUsername(owner), ownerUserID(ownerID) {}
    
    bool operator<(const ScoredRestaurant& other) const {
        return score > other.score;
    }
};

class RecommendationSystem {
private:
    UserManager* userManager;
    
    unordered_map<string, unordered_map<string, int>> userCuisinePreferences;
    
    vector<string> getTopCuisines(const string& userID, int topN = 3);
    
    float calculateScore(const Restaurant& restaurant,
                        const vector<string>& preferredCuisines);
    
    vector<Restaurant> loadUserRestaurants(const string& userID);
    
    vector<Restaurant> loadFriendsRestaurants(const string& userID);
    
public:
    RecommendationSystem(UserManager* um);
    
    void updatePreferences(const string& userID, const vector<string>& cuisines);
    
    vector<ScoredRestaurant> getRecommendationsFromFriends(const string& userID,
                                                           int limit = 10);
    
    vector<ScoredRestaurant> getRecommendationsByCity(const string& userID,
                                                      const string& city,
                                                      int limit = 10);
    
    void displayUserPreferences(const string& userID);
};

#endif