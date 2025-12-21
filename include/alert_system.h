#ifndef ALERT_SYSTEM_H
#define ALERT_SYSTEM_H

#include <iostream>
#include "user.h"
#include <queue>
#include <unordered_map>
#include <vector>
#include <fstream>

using namespace std;

class AlertSystem 
{
private:
    unordered_map<string, queue<Alert>> userAlerts;
    
    string alertsFilePath;
    
    void loadAlerts();
    void saveAlerts();
    
public:
    AlertSystem(const string& alertsFile = "data/system/alerts.dat");
    ~AlertSystem();
    
    void createAlert(const string& recipientID, const string& senderID,const string& senderName, const string& restaurantID,const string& restaurantName);
    
    void notifyFriends(const string& userID, const string& username,const string& restaurantID, const string& restaurantName,const vector<string>& friendIDs);
    
    vector<Alert> getAlerts(const string& userID);
    int getUnreadCount(const string& userID);
    void markAllAsRead(const string& userID);
    void clearAlerts(const string& userID);
};

#endif