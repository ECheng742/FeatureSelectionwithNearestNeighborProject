#include <iostream>
#include <vector>
#include <cstdlib>
#include <unordered_set>
#include <limits>
#include <cmath>
#include <fstream>
#include <sstream>
// use std::

double leaveOneOutCrossValidation(const std::vector<std::vector<double>> &copyData, const std::unordered_set<double> &currentSet, double featureToAdd) {
    std::vector<std::vector<double>> data = copyData;
    for (int i = 0; i < data.size(); i++) {
        for (int j = 1; j < data[0].size(); j++) { // FIXME: does indexing for featuretoadd and data columns match up?
            if (currentSet.find(j) == currentSet.end() && featureToAdd != j) {
                data[i][j] = 0;
            }
        }
    }

    double numberCorrectlyClassified = 0;

    for (int i = 0; i < data.size(); i++) {
        std::vector<double> objectToClassify = data[i]; // FIXME: 1st element removal
        double labelObjectToClassify = data[i][0];

        double nearestNeighborDistance = std::numeric_limits<double>::max();
        double nearestNeighborLocation = std::numeric_limits<double>::max();
        double nearestNeighborLabel = 0;

        for (int k = 0; k < data.size(); k++) {
            if(k != i) {
                // Calculate sum of squares
                double sumOfSquares = 0;
                for (int j = 1; j < data[0].size(); j++) {
                    sumOfSquares += pow(objectToClassify[j] - data[k][j], 2);
                }
                double distance = sqrt(sumOfSquares);
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
        // std::cout << "Object " << i << " is class " << labelObjectToClassify << std::endl; // FIXME
        // std::cout << "Its nearest neighbor is " << nearestNeighborLocation << " which is in class " << nearestNeighborLabel << std::endl; // FIXME
    }
    return numberCorrectlyClassified / data.size();
    // return rand() % 101;
}

void featureSearch(std::vector<std::vector<double>> data) {

    std::unordered_set<double> currentSetOfFeatures;
    std::vector<double> bestSet;
    int bestNumOfFeatures = 0;
    double previousLevelAccuracy = 0;

    for (int i = 1; i < data[0].size(); i++) { // FIXME: indexing diff than slides
        std::cout << "On the " << i << "th level of the search tree" << std::endl;
        double featureToAddAtThisLevel;
        double bestSoFarAccuracy = 0;

        for (int k = 1; k < data[0].size(); k++) {
            if (currentSetOfFeatures.find(k) == currentSetOfFeatures.end()) {
                double accuracy = leaveOneOutCrossValidation(data, currentSetOfFeatures, k); // FIXME: slides using k+1, but why doesn't match current feature being tested in line above
                std::cout << "--Considering adding the " << k << " feature (accuracy: " << accuracy;
                std::cout << ", set: ";
                for (unsigned i = 0; i < bestSet.size(); i++) std::cout << bestSet.at(i) << " ";
                std::cout << k << ")" << std::endl;

                if (accuracy > bestSoFarAccuracy) {
                    bestSoFarAccuracy = accuracy;
                    featureToAddAtThisLevel = k;
                }
            }   
        }
        // std::cout << "prev: " << previousLevelAccuracy << " best: " << bestSoFarAccuracy;
        if (previousLevelAccuracy < bestSoFarAccuracy) {
            bestNumOfFeatures++;
            previousLevelAccuracy = bestSoFarAccuracy;
        }
        // std::cout << " num: " << bestNumOfFeatures << std::endl;
        

        currentSetOfFeatures.insert(featureToAddAtThisLevel);
        bestSet.push_back(featureToAddAtThisLevel);
        std::cout << "On level " << i << " I added feature " << featureToAddAtThisLevel << " to current set" << std::endl;
        
        std::cout << "On level " << i << " accuracy is " << bestSoFarAccuracy << std::endl;
    }

    // std::cout << "Order of features added: ";
    // for (auto it = bestSet.begin(); it!= bestSet.end(); ++it) {
    //     std::cout << *it << ' ';
    // }
    // std::cout << std::endl;
    std::cout << "Best Set of Features: ";
    for (size_t i = 0; i < bestNumOfFeatures; i++) {
        std::cout << bestSet.at(i) << ' ';
    }
    std::cout << std::endl;
}

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
        while(ssin >> num) { // FIXME: precision gets truncated
            row.push_back(num);
        }
        data.push_back(row);
    }

    fileIn.close();
    return data;

}

int main() {
    std::vector<std::vector<double>> data;

    data = parseData("Ver_2_CS170_Fall_2021_Small_data__35.txt"); 
    
    // data = parseData("Ver_2_CS170_Fall_2021_LARGE_data__86.txt"); 
    // data = parseData("test.txt");

    std::vector<std::vector<double>> d;
    std::vector<double> v1 = {1, 7, 3, 2};
    std::vector<double> v2 = {2, 3, 1, 3};
    std::vector<double> v3 = {2, 2, 5, 4};
    std::vector<double> v4 = {1, 8, 5, 9};
    std::vector<double> v5 = {2, 2, 7, 1};
    std::vector<double> v6 = {1, 3, 6, 4};
    std::vector<double> v7 = {1, 8, 7, 6};
    std::vector<double> v8 = {2, 2, 1, 5};
    std::vector<double> v9 = {1, 9, 7, 3};
    std::vector<double> v10 = {1, 2, 4, 8};
    d.push_back(v1);
    d.push_back(v2);
    d.push_back(v3);
    d.push_back(v4);
    d.push_back(v5);
    d.push_back(v6);
    d.push_back(v7);
    d.push_back(v8);
    d.push_back(v9);
    // d.push_back(v10);
    // std::unordered_set<double> u;
    // u.insert(2);
    // leaveOneOutCrossValidation(d, u, 3);
    featureSearch(data);
    // std::cout << "Answer Key: " << 	"Eamonn Keogh - On this dataset 86, the accuracy can be 0.95 when using only features 3 8 2" << std::endl;

    // Scott's large 41: 21 (0.832), 42 (0.9775), 23 (0.9635) 

}