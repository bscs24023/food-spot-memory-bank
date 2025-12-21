#include "../include/alert_system.h"
#include <iostream>

using namespace std;

AlertSystem::AlertSystem(const string& alertsFile): alertsFilePath(alertsFile) 
{
    loadAlerts();
}

AlertSystem::~AlertSystem() {
    saveAlerts();
}

void AlertSystem::createAlert(const string& recipientID, const string& senderID,const string& senderName, const string& restaurantID,const string& restaurantName) {
    Alert alert(recipientID, senderID, senderName, restaurantID, restaurantName);
    userAlerts[recipientID].push(alert);
}

void AlertSystem::notifyFriends(const string& userID, const string& username,const string& restaurantID, const string& restaurantName,const vector<string>& friendIDs) {
    for (const auto& friendID : friendIDs) {
        createAlert(friendID, userID, username, restaurantID, restaurantName);
    }
    
    if (!friendIDs.empty()) {
        cout << "Notified " << friendIDs.size() << " friend(s)" << endl;
    }
}

vector<Alert> AlertSystem::getAlerts(const string& userID) {
    vector<Alert> alerts;
    queue<Alert>& q = userAlerts[userID];
    
    while (!q.empty()) {
        alerts.push_back(q.front());
        q.pop();
    }
    
    return alerts;
}

int AlertSystem::getUnreadCount(const string& userID) {
    return userAlerts[userID].size();
}

void AlertSystem::markAllAsRead(const string& userID) {
    while (!userAlerts[userID].empty()) {
        userAlerts[userID].pop();
    }
}

void AlertSystem::clearAlerts(const string& userID) {
    while (!userAlerts[userID].empty()) {
        userAlerts[userID].pop();
    }
    saveAlerts();
}

void AlertSystem::loadAlerts() {
    ifstream file(alertsFilePath, ios::binary);
    if (!file.good()) return;
    
    int userCount;
    file.read(reinterpret_cast<char*>(&userCount), sizeof(userCount));
    
    for (int i = 0; i < userCount; i++) {
        int len;
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        string userID(len, ' ');
        file.read(&userID[0], len);
        
        int alertCount;
        file.read(reinterpret_cast<char*>(&alertCount), sizeof(alertCount));
        
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
}

void AlertSystem::saveAlerts() {
    ofstream file(alertsFilePath, ios::binary);
    
    int userCount = userAlerts.size();
    file.write(reinterpret_cast<const char*>(&userCount), sizeof(userCount));
    
    for (auto& pair : userAlerts) {
        int len = pair.first.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(pair.first.c_str(), len);
        
        int alertCount = pair.second.size();
        file.write(reinterpret_cast<const char*>(&alertCount), sizeof(alertCount));
        
        queue<Alert> tempQueue = pair.second;
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
}