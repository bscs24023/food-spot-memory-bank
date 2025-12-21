#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <ctime>
#include <iostream>
using namespace std;

struct User {
    string userID;
    string username; 
    string email;
    string passwordHash;  
    time_t createdAt;
    int totalRestaurants;  
    
    User() : userID(""), username(""), email(""), passwordHash(""), createdAt(0), totalRestaurants(0) {}
    
    User(string id, string name, string mail, string pass): userID(id), username(name), email(mail), passwordHash(pass), createdAt(time(nullptr)), totalRestaurants(0) {}
    
    void display() const 
    {
        cout << endl;
        cout << "User: " << username << " (" << userID << ")" << endl;
        cout << "Email: " << email << endl;
        cout << "Restaurants Logged: " << totalRestaurants << endl;
        cout << "Member Since: " << ctime(&createdAt);
        cout << endl;
    }
};

struct Alert {
    string alertID;
    string recipientUserID;  
    string senderUserID;        
    string senderUsername;     
    string restaurantID;
    string restaurantName;
    time_t timestamp;
    bool isRead;
    
    Alert() : alertID(""), recipientUserID(""), senderUserID(""),senderUsername(""), restaurantID(""), restaurantName(""),timestamp(0), isRead(false) {}
    
    Alert(string recipient, string sender, string senderName,string restID, string restName): recipientUserID(recipient), senderUserID(sender),senderUsername(senderName), restaurantID(restID),restaurantName(restName), timestamp(time(nullptr)),isRead(false) {alertID = "alert_" + to_string(timestamp);
    }
    
    void display() const 
    {
        cout << senderUsername << " logged: " << restaurantName << endl;
        cout << "     Time: " << ctime(&timestamp);
    }
};

#endif