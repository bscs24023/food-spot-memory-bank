#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../include/user_manager.h"
#include "../include/alert_system.h"
#include "../include/recommendation_system.h"
#include "../include/disk_database.h"

namespace py = pybind11;

PYBIND11_MODULE(foodspot, m) {
    m.doc() = "Python wrapper for FoodSpotMemoryBank C++ multiuser system";

    py::class_<DiskDatabase>(m, "DiskDatabase")
        .def(py::init<const std::string&>())
        .def("addRestaurant", &DiskDatabase::addRestaurant)
        .def("searchByRatingRange", &DiskDatabase::searchByRatingRange)
        .def("searchByPriceRange", &DiskDatabase::searchByPriceRange)
        .def("searchByCuisine", &DiskDatabase::searchByCuisine)
        .def("searchByLocation", &DiskDatabase::searchByLocation)
        .def("getRestaurant", &DiskDatabase::getRestaurant)
        .def("displayAll", &DiskDatabase::displayAll)
        .def("getTotalRestaurants", &DiskDatabase::getTotalRestaurants);

    py::class_<AlertSystem>(m, "AlertSystem")
        .def(py::init<const std::string&>())
        .def("createAlert", &AlertSystem::createAlert)
        .def("notifyFriends", &AlertSystem::notifyFriends)
        .def("getAlerts", &AlertSystem::getAlerts)
        .def("getUnreadCount", &AlertSystem::getUnreadCount)
        .def("markAllAsRead", &AlertSystem::markAllAsRead)
        .def("clearAlerts", &AlertSystem::clearAlerts);

    py::class_<UserManager>(m, "UserManager")
        .def(py::init<>())
        .def("addUser", &UserManager::addUser)
        .def("getFriends", &UserManager::getFriends)
        .def("addFriend", &UserManager::addFriend)
        .def("removeFriend", &UserManager::removeFriend)
        .def("getAllUsers", &UserManager::getAllUsers);

    py::class_<RecommendationSystem>(m, "RecommendationSystem")
        .def(py::init<UserManager*>())
        .def("updatePreferences", &RecommendationSystem::updatePreferences)
        .def("getTopCuisines", &RecommendationSystem::getTopCuisines)
        .def("getRecommendationsFromFriends", &RecommendationSystem::getRecommendationsFromFriends)
        .def("getRecommendationsByCity", &RecommendationSystem::getRecommendationsByCity)
        .def("displayUserPreferences", &RecommendationSystem::displayUserPreferences);
}
