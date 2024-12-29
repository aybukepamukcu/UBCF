#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <cmath>
#include <algorithm>

using namespace std;

// Type alias for RatingsMap
using RatingsMap = unordered_map<int, unordered_map<int, double>>;

// Calculate cosine similarity between two vectors
double cosineSimilarity(const vector<double> &A, const vector<double> &B) {
    double dotProduct = 0, normA = 0, normB = 0;

    for (size_t i = 0; i < A.size(); ++i) {
        dotProduct += A[i] * B[i];
        normA += A[i] * A[i];
        normB += B[i] * B[i];
    }

    return (normA == 0 || normB == 0) ? 0 : dotProduct / (sqrt(normA) * sqrt(normB));
}

// Calculate the average rating of a user
double calculateUserAverage(const unordered_map<int, double> &ratings) {
    double sum = 0;
    for (const auto &pair : ratings) {
        sum += pair.second;
    }
    return (ratings.empty()) ? 0 : sum / ratings.size();
}

// Predict rating for a given user and movie using UBCF
double predictRatingUBCF(int userID, int movieID, int k, const RatingsMap &user_movie_ratings, double globalAverage) {
    vector<pair<double, int>> similarUsers;

    if (user_movie_ratings.find(userID) == user_movie_ratings.end()) {
        return globalAverage;
    }

    const auto &targetUserRatings = user_movie_ratings.at(userID);
    double targetUserAverage = calculateUserAverage(targetUserRatings);

    for (auto it = user_movie_ratings.begin(); it != user_movie_ratings.end(); ++it) {
        int otherUserID = it->first;
        const auto &otherUserRatings = it->second;

        if (otherUserID == userID || otherUserRatings.find(movieID) == otherUserRatings.end()) {
            continue;
        }

        vector<double> targetRatings, otherRatings;

        for (const auto &pair : targetUserRatings) {
            int commonMovieID = pair.first;
            if (otherUserRatings.find(commonMovieID) != otherUserRatings.end()) {
                targetRatings.push_back(pair.second - targetUserAverage);
                otherRatings.push_back(otherUserRatings.at(commonMovieID) - calculateUserAverage(otherUserRatings));
            }
        }

        if (targetRatings.size() >= 2 && otherRatings.size() >= 2) { 
            double similarity = cosineSimilarity(targetRatings, otherRatings);
            if (similarity > 0.05) { 
                similarUsers.push_back(make_pair(similarity, otherUserID));
            }
        }
    }

    sort(similarUsers.begin(), similarUsers.end(), greater<pair<double, int>>());
    if (similarUsers.size() > static_cast<size_t>(k)) {
        similarUsers.resize(static_cast<size_t>(k));
    }

    double numerator = 0, denominator = 0;
    for (const auto &pair : similarUsers) {
        double similarity = pair.first;
        int otherUserID = pair.second;
        const auto &otherUserRatings = user_movie_ratings.at(otherUserID);
        double otherUserAverage = calculateUserAverage(otherUserRatings);
        numerator += similarity * (otherUserRatings.at(movieID) - otherUserAverage);
        denominator += abs(similarity);
    }

    return (denominator > 0) ? (targetUserAverage + numerator / denominator) : targetUserRatings.empty() ? globalAverage : targetUserAverage;
}

// Main function
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    RatingsMap user_movie_ratings;
    vector<pair<int, int>> testPairs;
    double totalRating = 0;
    int ratingCount = 0;

    string line;
    bool isTest = false;

    while (getline(cin, line)) {
        if (line == "train dataset") {
            isTest = false;
            continue;
        } else if (line == "test dataset") {
            isTest = true;
            continue;
        }

        stringstream ss(line);
        int userID, movieID;
        double rating;

        if (isTest) {
            ss >> userID >> movieID;
            testPairs.push_back(make_pair(userID, movieID));
        } else {
            ss >> userID >> movieID >> rating;
            user_movie_ratings[userID][movieID] = rating;
            totalRating += rating;
            ratingCount++;
        }
    }

    double globalAverage = (ratingCount > 0) ? totalRating / ratingCount : 0.0;

    for (size_t i = 0; i < testPairs.size(); ++i) {
        int userID = testPairs[i].first;
        int movieID = testPairs[i].second;

        double prediction = predictRatingUBCF(userID, movieID, 30, user_movie_ratings, globalAverage); 
        cout << fixed << setprecision(1) << prediction << endl;
    }

    return 0;
}