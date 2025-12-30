#include <iostream>
#include <limits>
#include "../include/disk_database.h"
#include "../include/user_manager.h"
#include "../include/alert_system.h"
#include "../include/recommendation_system.h"

using namespace std;

UserManager* userManager = nullptr;
AlertSystem* alertSystem = nullptr;
RecommendationSystem* recommendationSystem = nullptr;
User* currentUser = nullptr;
DiskDatabase* currentDB = nullptr;

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void showMainMenu() {
    cout << "\n========================================" << endl;
    cout << "   FOOD SPOT MEMORY BANK - MULTI-USER" << endl;
    cout << "========================================" << endl;
    cout << "1. Register" << endl;
    cout << "2. Login" << endl;
    cout << "3. Exit" << endl;
    cout << "========================================" << endl;
    cout << "Choice: ";
}

void showUserMenu() {
    int unreadCount = alertSystem->getUnreadCount(currentUser->userID);
    
    cout << "\n========================================" << endl;
    cout << "   Welcome, " << currentUser->username << "!" << endl;
    if (unreadCount > 0) {
        cout << unreadCount << " new alert(s)" << endl;
    }
    cout << "========================================" << endl;
    cout << "RESTAURANTS:" << endl;
    cout << "  1. Add Restaurant" << endl;
    cout << "  2. Search by Rating Range" << endl;
    cout << "  3. Search by Price Range" << endl;
    cout << "  4. Search by Cuisine" << endl;
    cout << "  5. Search by Location" << endl;
    cout << "  6. Display All My Restaurants" << endl;
    cout << "\nRECOMMENDATIONS:" << endl;
    cout << "  7. Get Recommendations from Friends" << endl;
    cout << "  8. Discover Restaurants in City" << endl;
    cout << "  9. View My Preferences" << endl;
    cout << "\nFRIENDS:" << endl;
    cout << "  10. View My Friends" << endl;
    cout << "  11. Add Friend" << endl;
    cout << "  12. View Alerts" << endl;
    cout << "\nACCOUNT:" << endl;
    cout << "  13. View Profile" << endl;
    cout << "  14. Logout" << endl;
    cout << "========================================" << endl;
    cout << "Choice: ";
}

void registerUser() {
    string username, email, password;
    
    clearInput();
    cout << "\n--- Register New User ---" << endl;
    cout << "Username: ";
    getline(cin, username);
    cout << "Email: ";
    getline(cin, email);
    cout << "Password: ";
    getline(cin, password);
    
    userManager->registerUser(username, email, password);
}

void loginUser() {
    string username, password;
    
    clearInput();
    cout << "\n--- Login ---" << endl;
    cout << "Username: ";
    getline(cin, username);
    cout << "Password: ";
    getline(cin, password);
    
    currentUser = userManager->login(username, password);
    
    if (currentUser) {
        string dbPath = "data/users/user_" + currentUser->username + ".dat";
        currentDB = new DiskDatabase(dbPath);
        
        cout << "Logged in successfully!" << endl;
    }
}

void addRestaurant() {
    cout << "\n--- Add New Restaurant ---" << endl;
    
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
        
        cout << "\n  Dish " << (i+1) << " Name: ";
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
    cout << "\nNotes (optional): ";
    getline(cin, notes);
    
    string restID = currentDB->addRestaurant(name, location, cuisines, 
                                              rating, avgPrice, dishes, notes);
    
    userManager->updateRestaurantCount(currentUser->userID);
    currentUser->totalRestaurants++;
    
    recommendationSystem->updatePreferences(currentUser->userID, cuisines);
    
    vector<string> friends = userManager->getFriends(currentUser->userID);
    alertSystem->notifyFriends(currentUser->userID, currentUser->username,
                               restID, name, friends);
}

void viewFriends() {
    userManager->displayFriends(currentUser->userID);
}

void addFriend() {
    cout << "\n--- Add Friend ---" << endl;
    
    auto allUsers = userManager->getAllUsers();
    cout << "\nAvailable users:" << endl;
    
    int index = 1;
    for (const auto& user : allUsers) {
        if (user.userID != currentUser->userID) {
            cout << index++ << ". " << user.username 
                 << " (" << user.totalRestaurants << " restaurants)" << endl;
        }
    }
    
    clearInput();
    string friendUsername;
    cout << "\nEnter username to add: ";
    getline(cin, friendUsername);
    
    for (const auto& user : allUsers) {
        if (user.username == friendUsername) {
            userManager->addFriend(currentUser->userID, user.userID);
            return;
        }
    }
    
    cout << "User not found!" << endl;
}

void viewAlerts() {
    vector<Alert> alerts = alertSystem->getAlerts(currentUser->userID);
    
    if (alerts.empty()) {
        cout << "\n📭 No new alerts" << endl;
        return;
    }
    
    cout << "\n========================================" << endl;
    cout << "   YOUR ALERTS (" << alerts.size() << ")" << endl;
    cout << "========================================" << endl;
    
    for (const auto& alert : alerts) {
        alert.display();
    }
}

void viewProfile() {
    currentUser->display();
    cout << "Friends: " << userManager->getFriendCount(currentUser->userID) << endl;
}

void getRecommendationsFromFriends() {
    auto recommendations = recommendationSystem->getRecommendationsFromFriends(
        currentUser->userID, 10
    );
    
    if (recommendations.empty()) {
        return;
    }
    
    cout << "\n========================================" << endl;
    cout << "   RECOMMENDED BY YOUR FRIENDS (" << recommendations.size() << ")" << endl;
    cout << "========================================" << endl;
    
    int rank = 1;
    for (const auto& scored : recommendations) { 
        cout << " #" << rank << " - " << scored.restaurant.name << endl;
        cout << scored.restaurant.location << endl;
        for (size_t i = 0; i < scored.restaurant.cuisineTypes.size(); i++) {
            cout << scored.restaurant.cuisineTypes[i];
            if (i < scored.restaurant.cuisineTypes.size() - 1) cout << ", ";
        }
        cout << endl;
        cout << scored.restaurant.overallRating << "/10" << endl;
        cout << "   Rs. " << scored.restaurant.averagePrice << " avg" << endl;
        cout << "   Match Score: " << scored.score << endl;
        rank++;
    }
    cout << "========================================" << endl;
}

void discoverRestaurantsByCity() {
    string city;
    clearInput();
    cout << "\nEnter city name (e.g., Lahore, Karachi, Islamabad): ";
    getline(cin, city);
    
    auto recommendations = recommendationSystem->getRecommendationsByCity(
        currentUser->userID, city, 10
    );
    
    if (recommendations.empty()) {
        return;
    }
    
    cout << "\n========================================" << endl;
    cout << "   TOP RESTAURANTS IN " << city << " (" << recommendations.size() << ")" << endl;
    cout << "========================================" << endl;
    
    int rank = 1;
    for (const auto& scored : recommendations) {
        cout << " #" << rank << " - " << scored.restaurant.name << endl;
        cout << scored.restaurant.location << endl;
        for (size_t i = 0; i < scored.restaurant.cuisineTypes.size(); i++) {
            cout << scored.restaurant.cuisineTypes[i];
            if (i < scored.restaurant.cuisineTypes.size() - 1) cout << ", ";
        }
        cout << endl;
        cout << scored.restaurant.overallRating << "/10" << endl;
        cout << "   Rs. " << scored.restaurant.averagePrice << " avg" << endl;
        rank++;
    }
    cout << "========================================" << endl;
}

void viewMyPreferences() {
    recommendationSystem->displayUserPreferences(currentUser->userID);
}

int main() {
    cout << "========================================" << endl;
    cout << "  FOOD SPOT MEMORY BANK - MULTI-USER" << endl;
    cout << "  Cloud-Based Architecture" << endl;
    cout << "========================================\n" << endl;
    
    userManager = new UserManager();
    alertSystem = new AlertSystem();
    recommendationSystem = new RecommendationSystem(userManager);
    
    bool running = true;
    
    while (running) {
        if (!currentUser) {
            showMainMenu();
            
            int choice;
            if (!(cin >> choice)) {
                clearInput();
                continue;
            }
            
            switch (choice) {
                case 1:
                    registerUser();
                    break;
                case 2:
                    loginUser();
                    break;
                case 3:
                    running = false;
                    break;
                default:
                    cout << "Invalid choice!" << endl;
            }
        } else {
            showUserMenu();
            
            int choice;
            if (!(cin >> choice)) {
                clearInput();
                continue;
            }
            
            switch (choice) {
                case 1:
                    addRestaurant();
                    break;
                case 2: {
                    float minRating, maxRating;
                    cout << "\nMin Rating: ";
                    cin >> minRating;
                    cout << "Max Rating: ";
                    cin >> maxRating;
                    
                    auto results = currentDB->searchByRatingRange(minRating, maxRating);
                    cout << "\nFound " << results.size() << " restaurant(s)" << endl;
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
                    
                    auto results = currentDB->searchByPriceRange(minPrice, maxPrice);
                    cout << "\nFound " << results.size() << " restaurant(s)" << endl;
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
                    
                    auto results = currentDB->searchByCuisine(cuisine);
                    cout << "\nFound " << results.size() << " restaurant(s)" << endl;
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
                    
                    auto results = currentDB->searchByLocation(location);
                    cout << "\nFound " << results.size() << " restaurant(s)" << endl;
                    for (const auto& r : results) {
                        r.display();
                    }
                    break;
                }
                case 6:
                    currentDB->displayAll();
                    break;
                case 7:
                    getRecommendationsFromFriends();
                    break;
                case 8:
                    discoverRestaurantsByCity();
                    break;
                case 9:
                    viewMyPreferences();
                    break;
                case 10:
                    viewFriends();
                    break;
                case 11:
                    addFriend();
                    break;
                case 12:
                    viewAlerts();
                    break;
                case 13:
                    viewProfile();
                    break;
                case 14:
                    cout << "\n✓ Logging out..." << endl;
                    delete currentDB;
                    currentDB = nullptr;
                    currentUser = nullptr;
                    break;
                default:
                    cout << "Invalid choice!" << endl;
            }
        }
    }
    
    if (currentDB) delete currentDB;
    delete userManager;
    delete alertSystem;
    delete recommendationSystem;
    
    cout << "\n✓ Goodbye!" << endl;
    return 0;
}