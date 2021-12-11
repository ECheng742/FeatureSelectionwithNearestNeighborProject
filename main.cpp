#include <iostream>
#include <vector>
#include <cstdlib>
#include <unordered_set>
#include <limits>
#include <cmath>
#include <fstream>
#include <sstream>

namespace std {
    std::ofstream outputLogFileOut;
    std::ofstream dataLogFileOut;
}

double leaveOneOutCrossValidation(const std::vector<std::vector<double>> &copyData, const std::unordered_set<double> &currentSet, double featureToConsider, int method) {
    // Create new 2D vector for dataset with irrelevant features set to 0
    std::vector<std::vector<double>> data = copyData;
    for (int i = 0; i < data.size(); i++) {
        for (int j = 1; j < data[0].size(); j++) { 
            if (method == 1) { // Forward selection
                if (currentSet.find(j) == currentSet.end() && featureToConsider != j) data[i][j] = 0;
            }
            else if (method == 2) { // Backward elimination
                if (currentSet.find(j) == currentSet.end() || featureToConsider == j) data[i][j] = 0;
            }
            else { // Error
                return -1;
            }
        }
    }

    // Calculate how many objects were correctly classified using k-fold cross validation
    double numberCorrectlyClassified = 0;
    for (int i = 0; i < data.size(); i++) {
        std::vector<double> objectToClassify = data[i];
        double labelObjectToClassify = data[i][0];

        double nearestNeighborDistance = std::numeric_limits<double>::max();
        double nearestNeighborLocation = std::numeric_limits<double>::max();
        double nearestNeighborLabel = 0;

        for (int k = 0; k < data.size(); k++) {
            if(k != i) {
                // Calculate Euclidean distance
                double sumOfSquares = 0;
                for (int j = 1; j < data[0].size(); j++) {
                    sumOfSquares += pow(objectToClassify[j] - data[k][j], 2);
                }
                double distance = sqrt(sumOfSquares);

                // Update nearest neighbor if distance is smaller than current nearest neighbor
                if (distance < nearestNeighborDistance) {
                    nearestNeighborDistance = distance;
                    nearestNeighborLocation = k;
                    nearestNeighborLabel = data[nearestNeighborLocation][0];
                }
            }
        }
        
        if (labelObjectToClassify == nearestNeighborLabel) {
            numberCorrectlyClassified++;
        }
        // std::cout << "Object " << i << " is class " << labelObjectToClassify << std::endl; 
        // std::cout << "Its nearest neighbor is " << nearestNeighborLocation << " which is in class " << nearestNeighborLabel << std::endl; 
    }
    return numberCorrectlyClassified / data.size();
    // return rand() % 101;
}

void featureSearchForwardSelection(std::vector<std::vector<double>> data) {
    std::cout << "Feature Selection: Forward Selection Algorithm" << std::endl;
    std::cout << "Dataset Information: " << data[0].size()-1 << " features with " << data.size() << " instances" << std::endl << std::endl;
    
    // Default Rate
    std::outputLogFileOut << "On the 0th level of the search tree" << std::endl;
    std::cout << "On the 0th level of the search tree" << std::endl;
    std::vector<double> classSizes(2, 0);
    double mostCommonClass = 0;
    for (int i = 0; i < data.size(); i++) {
        int className = data[i][0];
        classSizes.at(className-1)++;
    }
    if (classSizes.at(0) >= classSizes.at(1)) mostCommonClass = 1;
    else mostCommonClass = 2;
    std::outputLogFileOut << "--Default Rate: " << classSizes.at(mostCommonClass-1)/data.size() * 100 << "%" << std::endl;
    std::cout << "--Default Rate: " << classSizes.at(mostCommonClass-1)/data.size() * 100 << "%" << std::endl;
    std::dataLogFileOut << "0 {} " << classSizes.at(mostCommonClass-1)/data.size() << std::endl;

    // On each level, add the feature that results in highest accuracy
    std::unordered_set<double> currentSetOfFeatures;
    std::vector<double> bestSet;
    int bestNumOfFeatures = 0;
    double previousLevelAccuracy = 0;

    for (int i = 1; i < data[0].size(); i++) { 
        std::outputLogFileOut << "On the " << i << "th level of the search tree" << std::endl;
        std::cout << "On the " << i << "th level of the search tree" << std::endl;
        double featureToAddAtThisLevel;
        double bestSoFarAccuracy = 0;

        for (int k = 1; k < data[0].size(); k++) {
            if (currentSetOfFeatures.find(k) == currentSetOfFeatures.end()) {
                double accuracy = leaveOneOutCrossValidation(data, currentSetOfFeatures, k, 1); 
                std::outputLogFileOut << "--Considering adding the " << k << " feature (Accuracy: " << accuracy * 100 << "%";
                std::outputLogFileOut << ", Best set: ";
                std::cout << "--Considering adding the " << k << " feature (Accuracy: " << accuracy * 100 << "%";
                std::cout << ", Best set: ";
                for (unsigned i = 0; i < bestSet.size(); i++) {
                    std::outputLogFileOut << bestSet.at(i) << " ";
                    std::cout << bestSet.at(i) << " ";
                }
                std::outputLogFileOut << k << ")" << std::endl;
                std::cout << k << ")" << std::endl;
                
                if (accuracy > bestSoFarAccuracy) {
                    bestSoFarAccuracy = accuracy;
                    featureToAddAtThisLevel = k;
                }
            }   
        }
        if (previousLevelAccuracy < bestSoFarAccuracy) {
            bestNumOfFeatures++;
            previousLevelAccuracy = bestSoFarAccuracy;
        }

        currentSetOfFeatures.insert(featureToAddAtThisLevel);
        bestSet.push_back(featureToAddAtThisLevel);
        std::outputLogFileOut << "On level " << i << " I added feature " << featureToAddAtThisLevel << " to current set" << std::endl;
        std::outputLogFileOut << "On level " << i << " accuracy is " << bestSoFarAccuracy * 100 << "%" << std::endl;
        std::cout << "On level " << i << " I added feature " << featureToAddAtThisLevel << " to current set" << std::endl;
        std::cout << "On level " << i << " accuracy is " << bestSoFarAccuracy * 100 << "%" << std::endl;

        std::dataLogFileOut << i << " {";
        for (size_t i = 0; i+1< bestSet.size(); i++) std::dataLogFileOut << bestSet.at(i) << ",";
        std::dataLogFileOut << bestSet.at(bestSet.size()-1) << "} " << bestSoFarAccuracy << std::endl;
    }

    std::outputLogFileOut << "Best Set of Features with accuracy " << previousLevelAccuracy * 100 << "%: {";
    std::cout << std::endl << "Best Set of Features with accuracy " << previousLevelAccuracy * 100 << "%: ";
    for (size_t i = 0; i+1 < bestNumOfFeatures; i++) {
        std::outputLogFileOut << bestSet.at(i) << ',';
        std::cout << bestSet.at(i) << ' ';
    }
    if (bestNumOfFeatures > 0) {
        std::outputLogFileOut << bestSet.at(bestNumOfFeatures-1);
        std::cout << bestSet.at(bestNumOfFeatures-1);
    }
    std::outputLogFileOut << "}" << std::endl;
    std::cout << std::endl;

    std::dataLogFileOut << "Best Set of Features: ";
    for (size_t i = 0; i < bestNumOfFeatures; i++) {
        std::dataLogFileOut << bestSet.at(i) << ' ';
    }
    std::dataLogFileOut << std::endl;
}

void featureSearchBackwardElimination(std::vector<std::vector<double>> data) {
    std::cout << "Feature Selection: Backward Elimination Algorithm" << std::endl;
    std::cout << "Dataset Information: " << data[0].size()-1 << " features with " << data.size() << " instances" << std::endl << std::endl;
    // Using all features
    std::outputLogFileOut << "On the 0th level of the search tree" << std::endl;
    std::cout << "On the 0th level of the search tree" << std::endl;
    std::unordered_set<double> currentSetOfFeatures;
    for (int i = 1; i < data[0].size(); i++) {
        currentSetOfFeatures.insert(i);
    }
    double accuracy = leaveOneOutCrossValidation(data, currentSetOfFeatures, -1, 2); 
    std::outputLogFileOut << "--Considering all features (accuracy: " << accuracy * 100 << "%";
    std::outputLogFileOut << ", set:";
    std::cout << "--Considering all features (accuracy: " << accuracy * 100 << "%";
    std::cout << ", set:";
    std::dataLogFileOut << "0 {";
    if (currentSetOfFeatures.size() <= 1) {
        std::outputLogFileOut << ' ';
        std::cout << ' ';
    }
    int count = 1;
    for (auto it = currentSetOfFeatures.begin(); it != currentSetOfFeatures.end(); ++it, ++count) {
        std::outputLogFileOut << ' ' << *it; 
        std::cout << ' ' << *it; 
        std::dataLogFileOut << *it;
        if (count < data[0].size()-1) std::dataLogFileOut << ',';
    }
    std::outputLogFileOut << ")" << std::endl;
    std::cout << ")" << std::endl;
    std::dataLogFileOut << "} " << accuracy << std::endl;

    std::vector<double> bestSet;
    int bestNumOfFeatures = data[0].size() - 1;
    double previousLevelAccuracy = 0;

    // On each level, remove the feature that results in highest accuracy
    for (int i = 1; i < data[0].size(); i++) { 
        std::outputLogFileOut << "On the " << i << "th level of the search tree" << std::endl;
        std::cout << "On the " << i << "th level of the search tree" << std::endl;
        double featureToRemoveAtThisLevel;
        double bestSoFarAccuracy = 0;

        for (int k = 1; k < data[0].size(); k++) {
            bool foundk = false;
            for (unsigned j = 0; j < bestSet.size(); j++) {
                if (bestSet.at(j) == k) {
                    foundk = true;
                    break;
                }
            }
            if (!foundk) {
                double accuracy = leaveOneOutCrossValidation(data, currentSetOfFeatures, k, 2); 
                std::outputLogFileOut << "--Considering removing the " << k << " feature (accuracy: " << accuracy * 100 << "%";
                std::outputLogFileOut << ", set:";
                std::cout << "--Considering removing the " << k << " feature (accuracy: " << accuracy * 100 << "%";
                std::cout << ", set:";
                if (currentSetOfFeatures.size() <= 1) {
                    std::outputLogFileOut << ' ';
                    std::cout << ' ';
                }
                for (auto it = currentSetOfFeatures.begin(); it != currentSetOfFeatures.end(); ++it) {
                    if (*it != k) { 
                        std::outputLogFileOut << ' ' << *it; 
                        std::cout << ' ' << *it; 
                    }
                }
                std::outputLogFileOut << ")" << std::endl;
                std::cout << ")" << std::endl;

                if (accuracy > bestSoFarAccuracy) {
                    bestSoFarAccuracy = accuracy;
                    featureToRemoveAtThisLevel = k;
                }
            }   
        }
        if (previousLevelAccuracy < bestSoFarAccuracy) {
            bestNumOfFeatures--;
            previousLevelAccuracy = bestSoFarAccuracy;
        }        

        currentSetOfFeatures.erase(featureToRemoveAtThisLevel);
        bestSet.push_back(featureToRemoveAtThisLevel);
        std::outputLogFileOut << "On level " << i << " I removed feature " << featureToRemoveAtThisLevel << " from current set" << std::endl;
        std::outputLogFileOut << "On level " << i << " accuracy is " << bestSoFarAccuracy  * 100 << "%" << std::endl;
        std::cout << "On level " << i << " I removed feature " << featureToRemoveAtThisLevel << " from current set" << std::endl;
        std::cout << "On level " << i << " accuracy is " << bestSoFarAccuracy * 100 << "%" << std::endl;

        std::vector<double> temp;
        std::dataLogFileOut << i << " {";
        for (auto it = currentSetOfFeatures.begin(); it != currentSetOfFeatures.end(); ++it) {
            temp.push_back(*it);
        }
        if (temp.size() >= 1) {
            for (size_t i = 0; i+1< temp.size(); i++)
                std::dataLogFileOut << temp.at(i) << ",";
            std::dataLogFileOut << temp.at(temp.size()-1);    
        }
        std::dataLogFileOut << "} " << bestSoFarAccuracy << std::endl;
    }

    std::outputLogFileOut << "Best Set of Features with accuracy " << previousLevelAccuracy * 100 << "%: ";
    std::cout << std::endl << "Best Set of Features with accuracy " << previousLevelAccuracy * 100 << "%: ";
    for (size_t i = 0; i < bestNumOfFeatures; i++) {
        std::outputLogFileOut << bestSet.at(bestSet.size()-i-1) << ' ';
        std::cout << bestSet.at(bestSet.size()-i-1) << ' ';
    }
    std::outputLogFileOut << std::endl;
    std::cout << std::endl;

    std::dataLogFileOut << "Best Set of Features: ";
    for (size_t i = 0; i < bestNumOfFeatures; i++) {
        std::dataLogFileOut << bestSet.at(bestSet.size()-i-1) << ' ';
    }
    std::dataLogFileOut << std::endl;
}

// Parse dataset from file into 2D vector
std::vector<std::vector<double>> parseData(const std::string &filename) {
    std::vector<std::vector<double>> data;
    std::string line;
    double num;

    std::ifstream fileIn;
    fileIn.open(filename);
    if (!fileIn.is_open()) {
        std::cout << "Error opening file: " << filename << std::endl;
        return data;
    }
    while(getline(fileIn, line)) {
        std::istringstream ssin;
        ssin.str(line);
        std::vector<double> row;
        while(ssin >> num) { 
            row.push_back(num);
        }
        data.push_back(row);
    }

    fileIn.close();
    return data;
}

int main() {
    std::vector<std::vector<double>> data;

    std::outputLogFileOut.open("output_log.txt");
    std::dataLogFileOut.open("data_log.csv");

    data = parseData("Ver_2_CS170_Fall_2021_Small_data__35.txt"); 
    std::cout << "File: Ver_2_CS170_Fall_2021_Small_data__35.txt" << std::endl;
    // data = parseData("Ver_2_CS170_Fall_2021_LARGE_data__86.txt"); 
    // std::cout << "File: Ver_2_CS170_Fall_2021_LARGE_data__86.txt" << std::endl;

    // featureSearchForwardSelection(data);
    featureSearchBackwardElimination(data);

    std::outputLogFileOut.close();
    std::dataLogFileOut.close();
}