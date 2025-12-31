#include "../include/alert_system.h"
#include <iostream>
#include <chrono>

using namespace std;

AlertSystem::AlertSystem(const string& alertsFile): alertsFilePath(alertsFile) 
{

    #ifdef _WIN32
        system("if not exist data mkdir data");
        system("if not exist data\\system mkdir data\\system");
    #else
        system("mkdir -p data/system");
    #endif
    
    loadAlerts();
    cout << "AlertSystem initialized. Data file: " << alertsFilePath << endl;
}

AlertSystem::~AlertSystem() {
    saveAlerts();
}

void AlertSystem::createAlert(const string& recipientID, const string& senderID,
                             const string& senderName, const string& restaurantID,
                             const string& restaurantName) {
    

    auto now = chrono::system_clock::now();
    auto timestamp = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
    string alertID = "alert_" + to_string(timestamp) + "_" + to_string(rand() % 1000);
    
    Alert alert;
    alert.alertID = alertID;
    alert.recipientUserID = recipientID;
    alert.senderUserID = senderID;
    alert.senderUsername = senderName;
    alert.restaurantID = restaurantID;
    alert.restaurantName = restaurantName;
    alert.timestamp = time(nullptr);
    alert.isRead = false;
    
    cout << "📢 Creating alert:" << endl;
    cout << "  For: " << recipientID << endl;
    cout << "  From: " << senderName << endl;
    cout << "  Restaurant: " << restaurantName << endl;
    
    userAlerts[recipientID].push(alert);
}

void AlertSystem::notifyFriends(const string& userID, const string& username,
                               const string& restaurantID, const string& restaurantName,
                               const vector<string>& friendIDs) {
    
    cout << "\n🔔 NOTIFYING FRIENDS:" << endl;
    cout << "  User: " << username << " added: " << restaurantName << endl;
    cout << "  Friends to notify: " << friendIDs.size() << endl;
    
    for (const auto& friendID : friendIDs) {
        createAlert(friendID, userID, username, restaurantID, restaurantName);
    }
    
    if (!friendIDs.empty()) {
        cout << "  ✓ Notified " << friendIDs.size() << " friend(s)" << endl;
        saveAlerts();
    }
}

vector<Alert> AlertSystem::getAlerts(const string& userID) {
    cout << "\n📨 GET_ALERTS for user: " << userID << endl;
    
    vector<Alert> alerts;
    auto it = userAlerts.find(userID);
    
    if (it == userAlerts.end()) {
        cout << "  No alerts found for this user" << endl;
        return alerts;
    }

    queue<Alert> tempQueue = it->second;
    
    cout << "  Queue size: " << tempQueue.size() << endl;
    
    int count = 0;
    while (!tempQueue.empty()) {
        Alert alert = tempQueue.front();
        tempQueue.pop();
        
        char timeStr[100];
        struct tm* timeinfo = localtime(&alert.timestamp);
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
        
        cout << "  Alert " << ++count << ":" << endl;
        cout << "    From: " << alert.senderUsername << endl;
        cout << "    Restaurant: " << alert.restaurantName << endl;
        cout << "    Time: " << timeStr << endl;
        cout << "    Read: " << (alert.isRead ? "Yes" : "No") << endl;
        
        alerts.push_back(alert);
    }
    
    cout << "  Returning " << alerts.size() << " alerts" << endl;
    return alerts;
}

int AlertSystem::getUnreadCount(const string& userID) {
    auto it = userAlerts.find(userID);
    if (it == userAlerts.end()) return 0;
    
    int count = it->second.size();
    cout << "Unread alerts for " << userID << ": " << count << endl;
    return count;
}

void AlertSystem::markAllAsRead(const string& userID) {
    auto it = userAlerts.find(userID);
    if (it != userAlerts.end()) {
        queue<Alert> newQueue;
        while (!it->second.empty()) {
            Alert alert = it->second.front();
            it->second.pop();
            alert.isRead = true;
            newQueue.push(alert);
        }
        it->second = newQueue;
        saveAlerts();
    }
}

void AlertSystem::clearAlerts(const string& userID) {
    auto it = userAlerts.find(userID);
    if (it != userAlerts.end()) {
        cout << "Clearing " << it->second.size() << " alerts for user " << userID << endl;
        queue<Alert> emptyQueue;
        it->second.swap(emptyQueue);
        saveAlerts();
    }
}

void AlertSystem::loadAlerts() {
    ifstream file(alertsFilePath, ios::binary);
    if (!file.good()) {
        cout << "No existing alerts file found. Starting fresh." << endl;
        return;
    }
    
    cout << "Loading alerts from: " << alertsFilePath << endl;
    
    int userCount;
    file.read(reinterpret_cast<char*>(&userCount), sizeof(userCount));
    cout << "Users with alerts: " << userCount << endl;
    
    for (int i = 0; i < userCount; i++) {
        int len;
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        string userID(len, ' ');
        file.read(&userID[0], len);
        
        int alertCount;
        file.read(reinterpret_cast<char*>(&alertCount), sizeof(alertCount));
        
        cout << "User " << userID << " has " << alertCount << " alerts" << endl;
        
        queue<Alert> alerts;
        for (int j = 0; j < alertCount; j++) {
            Alert alert;
            
            file.read(reinterpret_cast<char*>(&len), sizeof(len));
            alert.alertID.resize(len);
            file.read(&alert.alertID[0], len);
            
            file.read(reinterpret_cast<char*>(&len), sizeof(len));
            alert.recipientUserID.resize(len);
            file.read(&alert.recipientUserID[0], len);
            
            file.read(reinterpret_cast<char*>(&len), sizeof(len));
            alert.senderUserID.resize(len);
            file.read(&alert.senderUserID[0], len);
            
            file.read(reinterpret_cast<char*>(&len), sizeof(len));
            alert.senderUsername.resize(len);
            file.read(&alert.senderUsername[0], len);
            
            file.read(reinterpret_cast<char*>(&len), sizeof(len));
            alert.restaurantID.resize(len);
            file.read(&alert.restaurantID[0], len);
            
            file.read(reinterpret_cast<char*>(&len), sizeof(len));
            alert.restaurantName.resize(len);
            file.read(&alert.restaurantName[0], len);
            
            file.read(reinterpret_cast<char*>(&alert.timestamp), 
                      sizeof(alert.timestamp));
            file.read(reinterpret_cast<char*>(&alert.isRead), 
                      sizeof(alert.isRead));
            
            alerts.push(alert);
        }
        
        userAlerts[userID] = alerts;
    }
    
    file.close();
    cout << "Alerts loaded successfully!" << endl;
}

void AlertSystem::saveAlerts() {
    ofstream file(alertsFilePath, ios::binary);
    
    cout << "Saving alerts to: " << alertsFilePath << endl;
    
    int userCount = userAlerts.size();
    file.write(reinterpret_cast<const char*>(&userCount), sizeof(userCount));
    
    cout << "Saving alerts for " << userCount << " users" << endl;
    
    for (auto& pair : userAlerts) {
        const string& userID = pair.first;
        queue<Alert>& alerts = pair.second;
        
        int len = userID.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(userID.c_str(), len);
        
        int alertCount = alerts.size();
        file.write(reinterpret_cast<const char*>(&alertCount), sizeof(alertCount));
        
        cout << "  User " << userID << ": " << alertCount << " alerts" << endl;
        
        queue<Alert> tempQueue = alerts;
        while (!tempQueue.empty()) {
            Alert alert = tempQueue.front();
            tempQueue.pop();
            
            len = alert.alertID.length();
            file.write(reinterpret_cast<const char*>(&len), sizeof(len));
            file.write(alert.alertID.c_str(), len);
            
            len = alert.recipientUserID.length();
            file.write(reinterpret_cast<const char*>(&len), sizeof(len));
            file.write(alert.recipientUserID.c_str(), len);
            
            len = alert.senderUserID.length();
            file.write(reinterpret_cast<const char*>(&len), sizeof(len));
            file.write(alert.senderUserID.c_str(), len);
            
            len = alert.senderUsername.length();
            file.write(reinterpret_cast<const char*>(&len), sizeof(len));
            file.write(alert.senderUsername.c_str(), len);
            
            len = alert.restaurantID.length();
            file.write(reinterpret_cast<const char*>(&len), sizeof(len));
            file.write(alert.restaurantID.c_str(), len);
            
            len = alert.restaurantName.length();
            file.write(reinterpret_cast<const char*>(&len), sizeof(len));
            file.write(alert.restaurantName.c_str(), len);
            
            file.write(reinterpret_cast<const char*>(&alert.timestamp),
                       sizeof(alert.timestamp));
            file.write(reinterpret_cast<const char*>(&alert.isRead),
                       sizeof(alert.isRead));
        }
    }
    
    file.close();
    cout << "Alerts saved successfully!" << endl;
}

void AlertSystem::markAlertsAsRead(const string& userID) {
    auto it = userAlerts.find(userID);
    if (it == userAlerts.end()) {
        return;
    }
    
    cout << "📭 Marking alerts as read for user: " << userID << endl;
    cout << "  Before: " << it->second.size() << " unread alerts" << endl;
    
    queue<Alert> newQueue;
    queue<Alert> oldQueue = it->second;
    
    while (!oldQueue.empty()) {
        Alert alert = oldQueue.front();
        oldQueue.pop();
        
        if (!alert.isRead) {
            alert.isRead = true;
            cout << "  Marking alert as read: " << alert.restaurantName << endl;
        }
        
        newQueue.push(alert);
    }
    
    it->second = newQueue;
    saveAlerts();
    
    cout << "  After: " << it->second.size() << " alerts (all read)" << endl;
}