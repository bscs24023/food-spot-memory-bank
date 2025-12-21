#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include "user.h"
#include "hashtable.h"
#include <unordered_map>
#include <vector>
#include <queue>
#include <fstream>
#include <iostream>

using namespace std;

class UserManager 
{
private:
    unordered_map<string, User> users;
    
    unordered_map<string, vector<string>> friendGraph;
    
    string usersFilePath;
    string friendshipsFilePath;
    
    string hashPassword(const string& password);
    void loadUsers();
    void saveUsers();
    void loadFriendships();
    void saveFriendships();
    
public:
    UserManager(const string& usersFile = "data/system/users.dat",const string& friendshipsFile = "data/system/friendships.dat");
    ~UserManager();
    
    bool registerUser(const string& username, const string& email, const string& password);
    User* login(const string& username, const string& password);
    User* getUser(const string& userID);
    void updateRestaurantCount(const string& userID);
    vector<User> getAllUsers();
    
    bool addFriend(const string& userID, const string& friendID);
    bool removeFriend(const string& userID, const string& friendID);
    vector<string> getFriends(const string& userID);
    vector<User> getFriendProfiles(const string& userID);
    bool areFriends(const string& user1, const string& user2);
    int getFriendCount(const string& userID);
    
    void displayFriends(const string& userID);
};

#endif