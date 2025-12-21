#include <iostream>
#include <limits>
#include "../include/disk_database.h"
#include "../include/user_manager.h"
#include "../include/alert_system.h"

using namespace std;

UserManager* userManager = nullptr;
AlertSystem* alertSystem = nullptr;
User* currentUser = nullptr;
DiskDatabase* currentDB = nullptr;

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void showMainMenu() {
    cout << endl;
    cout << "   FOOD SPOT MEMORY BANK - MULTI-USER" << endl;
    cout << endl;
    cout << "1. Register" << endl;
    cout << "2. Login" << endl;
    cout << "3. Exit" << endl;
    cout << endl;
    cout << "Choice: ";
}

void showUserMenu() {
    int unreadCount = alertSystem->getUnreadCount(currentUser->userID);
    
    cout << endl;
    cout << "   Welcome, " << currentUser->username << "!" << endl;
    if (unreadCount > 0) {
        cout << unreadCount << " new alert(s)" << endl;
    }
    cout << endl;
    cout << "RESTAURANTS:" << endl;
    cout << "  1. Add Restaurant" << endl;
    cout << "  2. Search by Rating Range" << endl;
    cout << "  3. Search by Price Range" << endl;
    cout << "  4. Search by Cuisine" << endl;
    cout << "  5. Search by Location" << endl;
    cout << "  6. Display All My Restaurants" << endl;
    cout << "\nFRIENDS:" << endl;
    cout << "  7. View My Friends" << endl;
    cout << "  8. Add Friend" << endl;
    cout << "  9. View Alerts" << endl;
    cout << "\nACCOUNT:" << endl;
    cout << "  10. View Profile" << endl;
    cout << "  11. Logout" << endl;
    cout << endl;
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
    
    string restID = currentDB->addRestaurant(name, location, cuisines,  rating, avgPrice, dishes, notes);
    

    userManager->updateRestaurantCount(currentUser->userID);
    currentUser->totalRestaurants++;
    
    vector<string> friendIDs = userManager->getFriends(currentUser->userID);
    alertSystem->notifyFriends(currentUser->userID, currentUser->username, restID, name, friendIDs);
    
    cout << "\nRestaurant added and friends notified!" << endl;
}

void searchByRating() {
    float minRating, maxRating;
    
    cout << "\n--- Search by Rating Range ---" << endl;
    cout << "Min Rating: ";
    cin >> minRating;
    cout << "Max Rating: ";
    cin >> maxRating;
    
    vector<Restaurant> results = currentDB->searchByRatingRange(minRating, maxRating);
    
    cout << "\nFound " << results.size() << " restaurant(s):" << endl;
    for (const auto& r : results) {
        r.display();
    }
}

void searchByPrice() {
    float minPrice, maxPrice;
    
    cout << "\n--- Search by Price Range ---" << endl;
    cout << "Min Price: ";
    cin >> minPrice;
    cout << "Max Price: ";
    cin >> maxPrice;
    
    vector<Restaurant> results = currentDB->searchByPriceRange(minPrice, maxPrice);
    
    cout << "\nFound " << results.size() << " restaurant(s):" << endl;
    for (const auto& r : results) {
        r.display();
    }
}

void searchByCuisine() {
    string cuisine;
    
    cout << "\n--- Search by Cuisine ---" << endl;
    clearInput();
    cout << "Cuisine: ";
    getline(cin, cuisine);
    
    vector<Restaurant> results = currentDB->searchByCuisine(cuisine);
    
    cout << "\nFound " << results.size() << " restaurant(s):" << endl;
    for (const auto& r : results) {
        r.display();
    }
}

void searchByLocation() {
    string location;
    
    cout << "\n--- Search by Location ---" << endl;
    clearInput();
    cout << "Location: ";
    getline(cin, location);
    
    vector<Restaurant> results = currentDB->searchByLocation(location);
    
    cout << "\nFound " << results.size() << " restaurant(s) in " << location << ":" << endl;
    for (const auto& r : results) {
        r.display();
    }
}

void viewFriends() {
    userManager->displayFriends(currentUser->userID);
}

void addFriend() {
    string friendUsername;
    
    clearInput();
    cout << "\n--- Add Friend ---" << endl;
    cout << "Enter friend's username: ";
    getline(cin, friendUsername);
    
    string friendID = "user_" + friendUsername;
    User* friendUser = userManager->getUser(friendID);
    
    if (!friendUser) {
        cout << "User not found!" << endl;
        return;
    }
    
    if (friendID == currentUser->userID) {
        cout << "Cannot add yourself as friend!" << endl;
        return;
    }
    
    userManager->addFriend(currentUser->userID, friendID);
}

void viewAlerts() {
    cout << endl;
    cout << "   YOUR ALERTS" << endl;
    cout<< endl;
    
    vector<Alert> alerts = alertSystem->getAlerts(currentUser->userID);
    
    if (alerts.empty()) {
        cout << "No new alerts." << endl;
        return;
    }
    
    for (const auto& alert : alerts) {
        alert.display();
    }
    
    cout << "\nTotal: " << alerts.size() << " alert(s)" << endl;
}

void viewProfile() {
    currentUser->display();
    
    int friendCount = userManager->getFriendCount(currentUser->userID);
    cout << "Friends: " << friendCount << endl;
}

void userSession() {
    bool sessionActive = true;
    
    while (sessionActive) {
        showUserMenu();
        
        int choice;
        if (!(cin >> choice)) {
            clearInput();
            cout << "Invalid input!" << endl;
            continue;
        }
        
        switch (choice) {
            case 1:
                addRestaurant();
                break;
            case 2:
                searchByRating();
                break;
            case 3:
                searchByPrice();
                break;
            case 4:
                searchByCuisine();
                break;
            case 5:
                searchByLocation();
                break;
            case 6:
                currentDB->displayAll();
                break;
            case 7:
                viewFriends();
                break;
            case 8:
                addFriend();
                break;
            case 9:
                viewAlerts();
                break;
            case 10:
                viewProfile();
                break;
            case 11:
                cout << "\nLogging out..." << endl;
                delete currentDB;
                currentDB = nullptr;
                currentUser = nullptr;
                sessionActive = false;
                break;
            default:
                cout << "Invalid choice!" << endl;
        }
    }
}

int main() {
    cout << "========================================" << endl;
    cout << "  FOOD SPOT MEMORY BANK" << endl;
    cout << "   MULTI-USER SYSTEM" << endl;
    cout << "========================================\n" << endl;
    
    userManager = new UserManager();
    alertSystem = new AlertSystem();
    
    bool running = true;
    
    while (running) {
        if (!currentUser) {
            showMainMenu();
            
            int choice;
            if (!(cin >> choice)) {
                clearInput();
                cout << "Invalid input!" << endl;
                continue;
            }
            
            switch (choice) {
                case 1:
                    registerUser();
                    break;
                case 2:
                    loginUser();
                    if (currentUser) {
                        userSession();
                    }
                    break;
                case 3:
                    cout << "\nExiting system..." << endl;
                    running = false;
                    break;
                default:
                    cout << "Invalid choice!" << endl;
            }
        }
    }
    
    delete userManager;
    delete alertSystem;
    if (currentDB) delete currentDB;
    
    cout << "Goodbye!" << endl;
    
    return 0;
}