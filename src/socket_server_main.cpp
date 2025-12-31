#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "../include/disk_database.h"
#include "../include/user_manager.h"
#include "../include/alert_system.h"
#include "../include/recommendation_system.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

UserManager* userManager = nullptr;
AlertSystem* alertSystem = nullptr;
RecommendationSystem* recommendationSystem = nullptr;
map<string, DiskDatabase*> userDatabases;

string toJSON(const string& key, const string& value) {
    return "\"" + key + "\":\"" + value + "\"";
}

string toJSON(const string& key, int value) {
    return "\"" + key + "\":" + to_string(value);
}

string toJSON(const string& key, float value) {
    return "\"" + key + "\":" + to_string(value);
}

bool initWinsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cerr << "WSAStartup failed: " << result << endl;
        return false;
    }
    return true;
}

void cleanupWinsock() {
    WSACleanup();
}

string handleCommand(const string& command) {
    stringstream ss(command);
    string action;
    ss >> action;
    
    try {
        if (action == "REGISTER") {
            string username, email, password;
            ss >> username >> email >> password;
            
            bool success = userManager->registerUser(username, email, password);
            
            if (success) {
                return "{\"status\":\"success\",\"message\":\"User registered\"}";
            } else {
                return "{\"status\":\"error\",\"message\":\"Username already exists\"}";
            }
        }
        
        else if (action == "LOGIN") {
            string username, password;
            ss >> username >> password;
            
            User* user = userManager->login(username, password);
            
            if (user) {
                return "{\"status\":\"success\",\"userID\":\"" + user->userID + 
                       "\",\"username\":\"" + user->username + "\"}";
            } else {
                return "{\"status\":\"error\",\"message\":\"Invalid credentials\"}";
            }
        }
        
        else if (action == "ADD_RESTAURANT") {
            string userID, name, location, cuisine, notes;
            float rating, avgPrice;
            ss >> userID >> name >> location >> cuisine >> rating >> avgPrice >> notes;
            
            std::replace(name.begin(), name.end(), '_', ' ');
            std::replace(location.begin(), location.end(), '_', ' ');
            std::replace(cuisine.begin(), cuisine.end(), '_', ' ');
            std::replace(notes.begin(), notes.end(), '_', ' ');
            
            cout << "DEBUG ADD_RESTAURANT:" << endl;
            cout << "  User: " << userID << endl;
            cout << "  Name: " << name << endl;
            cout << "  Location: " << location << endl;
            cout << "  Cuisine: " << cuisine << endl;
            cout << "  Rating: " << rating << endl;
            cout << "  Price: " << avgPrice << endl;
            
            if (userDatabases.find(userID) == userDatabases.end()) {
                string username = userID.substr(5);
                string dbPath = "data/users/user_" + username + ".dat";
                
                #ifdef _WIN32
                    system("if not exist data mkdir data");
                    system("if not exist data\\users mkdir data\\users");
                #else
                    system("mkdir -p data/users");
                #endif
                
                cout << "  Creating database at: " << dbPath << endl;
                userDatabases[userID] = new DiskDatabase(dbPath);
            }
            
            vector<string> cuisines = {cuisine};
            vector<Dish> dishes;
            
            string restID = userDatabases[userID]->addRestaurant(
                name, location, cuisines, rating, avgPrice, dishes, notes
            );
            
            cout << "  Restaurant ID returned: " << restID << endl;
            
            if (restID.empty()) {
                cout << "  ERROR: Failed to add restaurant!" << endl;
                return "{\"status\":\"error\",\"message\":\"Failed to add restaurant\"}";
            }
            
            recommendationSystem->updatePreferences(userID, cuisines);
            
            vector<string> friends = userManager->getFriends(userID);
            User* user = userManager->getUser(userID);
            if (user && !friends.empty()) {
                alertSystem->notifyFriends(userID, user->username, restID, name, friends);
            }
            
            userManager->updateRestaurantCount(userID);
            
            cout << "  SUCCESS: Restaurant added!" << endl;
            
            return "{\"status\":\"success\",\"restaurantID\":\"" + restID + 
                "\",\"name\":\"" + name + "\"}";
        }
                
        else if (action == "GET_RESTAURANTS") 
        {
            string userID;
            ss >> userID;
            
            cout << "\nDEBUG GET_RESTAURANTS:" << endl;
            cout << "  User: " << userID << endl;
            
            if (userDatabases.find(userID) == userDatabases.end()) {
                string username = userID.substr(5);
                string dbPath = "data/users/user_" + username + ".dat";
                
                #ifdef _WIN32
                    system("if not exist data mkdir data");
                    system("if not exist data\\users mkdir data\\users");
                #else
                    system("mkdir -p data/users");
                #endif
                
                cout << "  Creating new database: " << dbPath << endl;
                userDatabases[userID] = new DiskDatabase(dbPath);
            }
            
            ifstream testFile("data/users/user_" + userID.substr(5) + ".dat");
            if (!testFile.good()) {
                cout << "  File doesn't exist yet" << endl;
                return "{\"status\":\"success\",\"restaurants\":[]}";
            }
            testFile.close();
            
            int count = userDatabases[userID]->getTotalRestaurants();
            cout << "  Database reports: " << count << " restaurants" << endl;
            
            try {
                vector<Restaurant> restaurants = userDatabases[userID]->getAllRestaurants();
                cout << "  Successfully read " << restaurants.size() << " restaurants" << endl;
                
                string json = "{\"status\":\"success\",\"restaurants\":[";
                for (size_t i = 0; i < restaurants.size(); i++) {
                    const auto& r = restaurants[i];
                    cout << "    Restaurant " << i << ": " << r.name << endl;
                    
                    json += "{";
                    json += "\"id\":\"" + r.restaurantId + "\",";
                    json += "\"name\":\"" + r.name + "\",";
                    json += "\"location\":\"" + r.location + "\",";
                    json += "\"cuisine\":\"" + (r.cuisineTypes.empty() ? "" : r.cuisineTypes[0]) + "\",";
                    json += "\"rating\":" + to_string(r.overallRating) + ",";
                    json += "\"price\":" + to_string(r.averagePrice);
                    json += "}";
                    if (i < restaurants.size() - 1) json += ",";
                }
                json += "]}";
                
                return json;
                
            } catch (const exception& e) {
                cout << "  ERROR reading restaurants: " << e.what() << endl;
                return "{\"status\":\"error\",\"message\":\"Error reading restaurants: " + string(e.what()) + "\"}";
            }
        }       
        
        else if (action == "GET_FRIENDS") 
        {
            string userID;
            ss >> userID;
            
            vector<User> friends = userManager->getFriendProfiles(userID);
            
            string json = "{\"status\":\"success\",\"friends\":[";
            for (size_t i = 0; i < friends.size(); i++) {
                json += "{\"userID\":\"" + friends[i].userID + 
                        "\",\"username\":\"" + friends[i].username + "\"}";
                if (i < friends.size() - 1) json += ",";
            }
            json += "]}";
            
            return json;
        }
        
        else if (action == "ADD_FRIEND") {
            string userID, friendUsername;
            ss >> userID >> friendUsername;
            
            auto allUsers = userManager->getAllUsers();
            for (const auto& user : allUsers) {
                if (user.username == friendUsername) {
                    bool success = userManager->addFriend(userID, user.userID);
                    if (success) {
                        return "{\"status\":\"success\",\"message\":\"Friend added\"}";
                    } else {
                        return "{\"status\":\"error\",\"message\":\"Already friends\"}";
                    }
                }
            }
            
            return "{\"status\":\"error\",\"message\":\"User not found\"}";
        }
        
        else if (action == "GET_ALERTS") 
        {
            string userID;
            ss >> userID;
            
            cout << "\n API: GET_ALERTS for user: " << userID << endl;
            
            vector<Alert> alerts = alertSystem->getAlerts(userID);
            
            if (alerts.empty()) {
                cout << "  No alerts to return" << endl;
                return "{\"status\":\"success\",\"alerts\":[],\"count\":0}";
            }
            
            string json = "{\"status\":\"success\",\"alerts\":[";
            for (size_t i = 0; i < alerts.size(); i++) {
                const auto& alert = alerts[i];
                
                char timeBuffer[100];
                struct tm* timeinfo = localtime(&alert.timestamp);
                strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M", timeinfo);
                string timeStr(timeBuffer);
                
                json += "{";
                json += "\"sender\":\"" + alert.senderUsername + "\",";
                json += "\"restaurant\":\"" + alert.restaurantName + "\",";
                json += "\"time\":\"" + timeStr + "\",";
                json += "\"read\":" + string(alert.isRead ? "true" : "false");
                json += "}";
                if (i < alerts.size() - 1) json += ",";
                
                cout << "  Alert " << i << ": " << alert.senderUsername 
                    << " -> " << alert.restaurantName << endl;
            }
            json += "],\"count\":" + to_string(alerts.size()) + "}";
            
            cout << "  Returning " << alerts.size() << " alerts" << endl;
            return json;
        }
        
        else if (action == "GET_RECOMMENDATIONS") 
        {
            string userID;
            ss >> userID;
            
            cout << "\n GET_RECOMMENDATIONS for user: " << userID << endl;
            
            auto recommendations = recommendationSystem->getRecommendationsFromFriends(userID, 10);
            
            if (recommendations.empty()) {
                cout << "  No friend recommendations. Trying general recommendations..." << endl;
                recommendations = recommendationSystem->getGeneralRecommendations(userID, 10);
            }
            
            if (recommendations.empty()) {
                cout << "  No recommendations found at all" << endl;
                return "{\"status\":\"success\",\"recommendations\":[]}";
            }
            
            cout << "  Found " << recommendations.size() << " recommendations" << endl;
            
            string json = "{\"status\":\"success\",\"recommendations\":[";
            for (size_t i = 0; i < recommendations.size(); i++) {
                const auto& rec = recommendations[i];
                const auto& r = rec.restaurant;
                
                cout << "  • " << r.name << " (" << rec.score << "%) - From: " 
                    << rec.ownerUsername << endl;
                
                json += "{";
                json += "\"name\":\"" + r.name + "\",";
                json += "\"location\":\"" + r.location + "\",";
                json += "\"cuisine\":\"" + (r.cuisineTypes.empty() ? "" : r.cuisineTypes[0]) + "\",";
                json += "\"rating\":" + to_string(r.overallRating) + ",";
                json += "\"price\":" + to_string(r.averagePrice) + ",";
                json += "\"score\":" + to_string(rec.score) + ",";
                json += "\"owner\":\"" + rec.ownerUsername + "\"";  
                json += "}";
                if (i < recommendations.size() - 1) json += ",";
            }
            json += "]}";
            
            return json;
        }

        else if (action == "MARK_ALERTS_READ") {
            string userID;
            ss >> userID;
            
            cout << "\n MARK_ALERTS_READ for user: " << userID << endl;
            
            alertSystem->markAlertsAsRead(userID);
            
            return "{\"status\":\"success\",\"message\":\"Alerts marked as read\"}";
        }


        else if (action == "SEARCH_CUISINE") {
            string userID, cuisine;
            ss >> userID >> cuisine;
            
            std::replace(cuisine.begin(), cuisine.end(), '_', ' ');
            
            cout << "\n SEARCH_CUISINE:" << endl;
            cout << "  User: " << userID << endl;
            cout << "  Cuisine: " << cuisine << endl;
            
            if (userDatabases.find(userID) == userDatabases.end()) {
                cout << "  ERROR: User database not found" << endl;
                return "{\"status\":\"error\",\"message\":\"User database not found\"}";
            }
            
            vector<Restaurant> results = userDatabases[userID]->searchByCuisine(cuisine);
            cout << "  Found " << results.size() << " restaurants" << endl;
            
            string json = "{\"status\":\"success\",\"restaurants\":[";
            for (size_t i = 0; i < results.size(); i++) {
                const auto& r = results[i];
                
                cout << "    • " << r.name << " (Rating: " << r.overallRating << ")" << endl;
                
                json += "{";
                json += "\"id\":\"" + r.restaurantId + "\",";
                json += "\"name\":\"" + r.name + "\",";
                json += "\"location\":\"" + r.location + "\",";
                json += "\"cuisine\":\"" + (r.cuisineTypes.empty() ? "" : r.cuisineTypes[0]) + "\",";
                json += "\"rating\":" + to_string(r.overallRating) + ",";
                json += "\"price\":" + to_string(r.averagePrice) + ",";
                json += "\"notes\":\"" + r.notes + "\"";
                json += "}";
                if (i < results.size() - 1) json += ",";
            }
            json += "],\"count\":" + to_string(results.size()) + "}";
            
            return json;
        }

        else if (action == "SEARCH_LOCATION") {
            string userID, location;
            ss >> userID >> location;
            
            std::replace(location.begin(), location.end(), '_', ' ');
            
            cout << "\n SEARCH_LOCATION:" << endl;
            cout << "  User: " << userID << endl;
            cout << "  Location: " << location << endl;
            
            if (userDatabases.find(userID) == userDatabases.end()) {
                cout << "  ERROR: User database not found" << endl;
                return "{\"status\":\"error\",\"message\":\"User database not found\"}";
            }
            
            vector<Restaurant> results = userDatabases[userID]->searchByLocation(location);
            cout << "  Found " << results.size() << " restaurants" << endl;
            
            string json = "{\"status\":\"success\",\"restaurants\":[";
            for (size_t i = 0; i < results.size(); i++) {
                const auto& r = results[i];
                
                cout << "    • " << r.name << " (Cuisine: " 
                    << (r.cuisineTypes.empty() ? "" : r.cuisineTypes[0]) << ")" << endl;
                
                json += "{";
                json += "\"id\":\"" + r.restaurantId + "\",";
                json += "\"name\":\"" + r.name + "\",";
                json += "\"location\":\"" + r.location + "\",";
                json += "\"cuisine\":\"" + (r.cuisineTypes.empty() ? "" : r.cuisineTypes[0]) + "\",";
                json += "\"rating\":" + to_string(r.overallRating) + ",";
                json += "\"price\":" + to_string(r.averagePrice) + ",";
                json += "\"notes\":\"" + r.notes + "\"";
                json += "}";
                if (i < results.size() - 1) json += ",";
            }
            json += "],\"count\":" + to_string(results.size()) + "}";
            
            return json;
        }

        else if (action == "SEARCH_RATING") {
            string userID;
            float minRating, maxRating;
            ss >> userID >> minRating >> maxRating;
            
            cout << "\n SEARCH_RATING:" << endl;
            cout << "  User: " << userID << endl;
            cout << "  Rating Range: " << minRating << " - " << maxRating << endl;
            
            if (userDatabases.find(userID) == userDatabases.end()) {
                cout << "  ERROR: User database not found" << endl;
                return "{\"status\":\"error\",\"message\":\"User database not found\"}";
            }
            
            vector<Restaurant> results = userDatabases[userID]->searchByRatingRange(minRating, maxRating);
            cout << "  Found " << results.size() << " restaurants" << endl;
            
            string json = "{\"status\":\"success\",\"restaurants\":[";
            for (size_t i = 0; i < results.size(); i++) {
                const auto& r = results[i];
                
                cout << "    • " << r.name << " (Rating: " << r.overallRating << ")" << endl;
                
                json += "{";
                json += "\"id\":\"" + r.restaurantId + "\",";
                json += "\"name\":\"" + r.name + "\",";
                json += "\"location\":\"" + r.location + "\",";
                json += "\"cuisine\":\"" + (r.cuisineTypes.empty() ? "" : r.cuisineTypes[0]) + "\",";
                json += "\"rating\":" + to_string(r.overallRating) + ",";
                json += "\"price\":" + to_string(r.averagePrice) + ",";
                json += "\"notes\":\"" + r.notes + "\"";
                json += "}";
                if (i < results.size() - 1) json += ",";
            }
            json += "],\"count\":" + to_string(results.size()) + "}";
            
            return json;
        }

        else if (action == "SEARCH_PRICE") {
            string userID;
            float minPrice, maxPrice;
            ss >> userID >> minPrice >> maxPrice;
            
            cout << "\n SEARCH_PRICE:" << endl;
            cout << "  User: " << userID << endl;
            cout << "  Price Range: Rs. " << minPrice << " - " << maxPrice << endl;
            
            if (userDatabases.find(userID) == userDatabases.end()) {
                cout << "  ERROR: User database not found" << endl;
                return "{\"status\":\"error\",\"message\":\"User database not found\"}";
            }
            
            vector<Restaurant> results = userDatabases[userID]->searchByPriceRange(minPrice, maxPrice);
            cout << "  Found " << results.size() << " restaurants" << endl;
            
            string json = "{\"status\":\"success\",\"restaurants\":[";
            for (size_t i = 0; i < results.size(); i++) {
                const auto& r = results[i];
                
                cout << "    • " << r.name << " (Price: Rs. " << r.averagePrice << ")" << endl;
                
                json += "{";
                json += "\"id\":\"" + r.restaurantId + "\",";
                json += "\"name\":\"" + r.name + "\",";
                json += "\"location\":\"" + r.location + "\",";
                json += "\"cuisine\":\"" + (r.cuisineTypes.empty() ? "" : r.cuisineTypes[0]) + "\",";
                json += "\"rating\":" + to_string(r.overallRating) + ",";
                json += "\"price\":" + to_string(r.averagePrice) + ",";
                json += "\"notes\":\"" + r.notes + "\"";
                json += "}";
                if (i < results.size() - 1) json += ",";
            }
            json += "],\"count\":" + to_string(results.size()) + "}";
            
            return json;
        }
        
        else if (action == "TEST") {
            return "{\"status\":\"success\",\"message\":\"Server is working!\"}";
        }
        
        else {
            return "{\"status\":\"error\",\"message\":\"Unknown command\"}";
        }
        
    } catch (const exception& e) {
        return "{\"status\":\"error\",\"message\":\"" + string(e.what()) + "\"}";
    }
}

void handleClient(SOCKET clientSocket) {
    char buffer[4096];
    
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        
        if (bytesReceived <= 0) {
            cout << "Client disconnected" << endl;
            break;
        }
        
        string command(buffer, bytesReceived);
        cout << "Received: " << command << endl;
        
        string response = handleCommand(command);
        cout << "Sending: " << response << endl;
        
        send(clientSocket, response.c_str(), response.length(), 0);
    }
    
    closesocket(clientSocket);
}

int main() {
    cout << "  FOOD SPOT SOCKET SERVER (WINDOWS)" << endl;
    
    if (!initWinsock()) {
        cerr << "Failed to initialize Winsock" << endl;
        return 1;
    }
    
    userManager = new UserManager();
    alertSystem = new AlertSystem();
    recommendationSystem = new RecommendationSystem(userManager);
    
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Failed to create socket: " << WSAGetLastError() << endl;
        cleanupWinsock();
        return 1;
    }
    
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);
    
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Failed to bind to port 8080: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        cleanupWinsock();
        return 1;
    }
    
    if (listen(serverSocket, 10) == SOCKET_ERROR) {
        cerr << "Failed to listen: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        cleanupWinsock();
        return 1;
    }
    
    cout << " Socket server started successfully!" << endl;
    cout << " Waiting for connections...\n" << endl;
    
    while (true) {
        sockaddr_in clientAddr;
        int clientSize = sizeof(clientAddr);
        
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
        
        if (clientSocket == INVALID_SOCKET) {
            cerr << "Failed to accept connection: " << WSAGetLastError() << endl;
            continue;
        }
        
        cout << " New client connected" << endl;
        
        thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }
    
    closesocket(serverSocket);
    cleanupWinsock();
    delete userManager;
    delete alertSystem;
    delete recommendationSystem;
    
    return 0;
}