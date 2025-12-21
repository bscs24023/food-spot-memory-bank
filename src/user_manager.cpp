#include "../include/user_manager.h"
#include <iostream>
#include <algorithm>
#include <sys/stat.h>

using namespace std;

UserManager::UserManager(const string& usersFile, const string& friendshipsFile)
    : usersFilePath(usersFile), friendshipsFilePath(friendshipsFile) {
    
    cout << "\nInitializing User Manager" << endl;
    
    system("mkdir -p data/system");
    system("mkdir -p data/users");
    
    loadUsers();
    loadFriendships();
    
    cout << "Loaded " << users.size() << " users" << endl;
}

UserManager::~UserManager() {
    saveUsers();
    saveFriendships();
}

string UserManager::hashPassword(const string& password) {
    hash<string> hasher;
    return to_string(hasher(password));
}

bool UserManager::registerUser(const string& username, const string& email,const string& password) {
    for (const auto& pair : users) {
        if (pair.second.username == username) {
            cout << "Username already taken!" << endl;
            return false;
        }
    }
    
    string userID = "user_" + username;
    string passHash = hashPassword(password);
    
    User newUser(userID, username, email, passHash);
    users[userID] = newUser;
    
    friendGraph[userID] = vector<string>();
    
    saveUsers();
    
    cout << "User registered: " << username << endl;
    return true;
}

User* UserManager::login(const string& username, const string& password) {
    string passHash = hashPassword(password);
    
    for (auto& pair : users) {
        if (pair.second.username == username && 
            pair.second.passwordHash == passHash) {
            cout << "Login successful: " << username << endl;
            return &pair.second;
        }
    }

    cout << "Invalid username or password!" << endl;
    return nullptr;
}

User* UserManager::getUser(const string& userID) {
    auto it = users.find(userID);
    return (it != users.end()) ? &it->second : nullptr;
}

void UserManager::updateRestaurantCount(const string& userID) {
    auto it = users.find(userID);
    if (it != users.end()) {
        it->second.totalRestaurants++;
        saveUsers();
    }
}

vector<User> UserManager::getAllUsers() {
    vector<User> allUsers;
    for (const auto& pair : users) {
        allUsers.push_back(pair.second);
    }
    return allUsers;
}

bool UserManager::addFriend(const string& userID, const string& friendID) {
    if (users.find(userID) == users.end() || 
        users.find(friendID) == users.end()) {
        cout << "User not found!" << endl;
        return false;
    }
    
    if (areFriends(userID, friendID)) {
        cout << "⚠️  Already friends!" << endl;
        return false;
    }
    
    friendGraph[userID].push_back(friendID);
    friendGraph[friendID].push_back(userID);
    
    saveFriendships();

    cout << "Friendship added!" << endl;
    return true;
}

bool UserManager::removeFriend(const string& userID, const string& friendID) {
    auto& userFriends = friendGraph[userID];
    auto& friendFriends = friendGraph[friendID];
    
    userFriends.erase(remove(userFriends.begin(), userFriends.end(), friendID),userFriends.end());
    friendFriends.erase(remove(friendFriends.begin(), friendFriends.end(), userID),friendFriends.end());
    
    saveFriendships();

    cout << "Friendship removed" << endl;
    return true;
}

vector<string> UserManager::getFriends(const string& userID) {
    auto it = friendGraph.find(userID);
    return (it != friendGraph.end()) ? it->second : vector<string>();
}

vector<User> UserManager::getFriendProfiles(const string& userID) {
    vector<User> friendProfiles;
    auto friendIDs = getFriends(userID);
    
    for (const auto& friendID : friendIDs) {
        User* user = getUser(friendID);
        if (user) {
            friendProfiles.push_back(*user);
        }
    }
    
    return friendProfiles;
}

bool UserManager::areFriends(const string& user1, const string& user2) {
    auto it = friendGraph.find(user1);
    if (it == friendGraph.end()) return false;
    
    return find(it->second.begin(), it->second.end(), user2) != it->second.end();
}

int UserManager::getFriendCount(const string& userID) {
    auto it = friendGraph.find(userID);
    return (it != friendGraph.end()) ? it->second.size() : 0;
}

void UserManager::displayFriends(const string& userID) {
    auto friendProfiles = getFriendProfiles(userID);
    
    if (friendProfiles.empty()) {
        cout << "\nNo friends yet." << endl;
        return;
    }
    
    cout << endl;
    cout << "YOUR FRIENDS (" << friendProfiles.size() << ")" << endl;
    cout << endl;
    
    for (const auto& friendUser : friendProfiles) {
        cout <<  friendUser.username << " (" << friendUser.totalRestaurants << " restaurants)" << endl;
    }
}

void UserManager::loadUsers() {
    ifstream file(usersFilePath, ios::binary);
    if (!file.good()) return;
    
    int count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    
    for (int i = 0; i < count; i++) {
        User user;
        
        int len;
        
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        user.userID.resize(len);
        file.read(&user.userID[0], len);
        
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        user.username.resize(len);
        file.read(&user.username[0], len);
        
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        user.email.resize(len);
        file.read(&user.email[0], len);
        
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        user.passwordHash.resize(len);
        file.read(&user.passwordHash[0], len);
        
        file.read(reinterpret_cast<char*>(&user.createdAt), sizeof(user.createdAt));
        file.read(reinterpret_cast<char*>(&user.totalRestaurants), 
                  sizeof(user.totalRestaurants));
        
        users[user.userID] = user;
    }
    
    file.close();
}

void UserManager::saveUsers() {
    ofstream file(usersFilePath, ios::binary);
    
    int count = users.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    
    for (const auto& pair : users) {
        const User& user = pair.second;
        
        int len;
        
        len = user.userID.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(user.userID.c_str(), len);
        
        len = user.username.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(user.username.c_str(), len);
        
        len = user.email.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(user.email.c_str(), len);
        
        len = user.passwordHash.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(user.passwordHash.c_str(), len);
        
        file.write(reinterpret_cast<const char*>(&user.createdAt), 
                   sizeof(user.createdAt));
        file.write(reinterpret_cast<const char*>(&user.totalRestaurants),
                   sizeof(user.totalRestaurants));
    }
    
    file.close();
}

void UserManager::loadFriendships() {
    ifstream file(friendshipsFilePath, ios::binary);
    if (!file.good()) return;
    
    int userCount;
    file.read(reinterpret_cast<char*>(&userCount), sizeof(userCount));
    
    for (int i = 0; i < userCount; i++) {
        int len;
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        string userID(len, ' ');
        file.read(&userID[0], len);
        
        int friendCount;
        file.read(reinterpret_cast<char*>(&friendCount), sizeof(friendCount));
        
        vector<string> friends;
        for (int j = 0; j < friendCount; j++) {
            file.read(reinterpret_cast<char*>(&len), sizeof(len));
            string friendID(len, ' ');
            file.read(&friendID[0], len);
            friends.push_back(friendID);
        }
        
        friendGraph[userID] = friends;
    }
    
    file.close();
}

void UserManager::saveFriendships() {
    ofstream file(friendshipsFilePath, ios::binary);
    
    int userCount = friendGraph.size();
    file.write(reinterpret_cast<const char*>(&userCount), sizeof(userCount));
    
    for (const auto& pair : friendGraph) {
        int len = pair.first.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(pair.first.c_str(), len);
        
        int friendCount = pair.second.size();
        file.write(reinterpret_cast<const char*>(&friendCount), sizeof(friendCount));
        
        for (const auto& friendID : pair.second) {
            len = friendID.length();
            file.write(reinterpret_cast<const char*>(&len), sizeof(len));
            file.write(friendID.c_str(), len);
        }
    }
    
    file.close();
}