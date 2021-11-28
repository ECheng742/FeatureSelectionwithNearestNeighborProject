#include <iostream>
#include <vector>
#include <cstdlib>
#include <unordered_set>
#include <limits>
#include <cmath>
#include <fstream>
#include <sstream>
// use std::

double leaveOneOutCrossValidation(std::vector<std::vector<double>> data, std::unordered_set<double> currentSet, double featureToAdd) {
    
    for (int i = 0; i < data.size(); i++) {
        for (int j = 1; j < data[0].size(); j++) { // FIXME: does indexing for featuretoadd and data columns match up?
            if (currentSet.find(j) == currentSet.end()) {
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
                for (int j = 1; j < data.size(); j++) {
                    sumOfSquares = pow(objectToClassify[j] - data[k][j], 2);
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
        std::cout << "Object " << i << " is class " << labelObjectToClassify << std::endl;
        std::cout << "Its nearest neighbor is " << nearestNeighborLocation << " which is in class " << nearestNeighborLabel << std::endl;
    }

    return numberCorrectlyClassified / data.size();
    // return rand() % 101;
}

void featureSearch(std::vector<std::vector<double>> data) {

    std::unordered_set<double> currentSetOfFeatures;

    for (int i = 1; i < data[0].size(); i++) { // FIXME: indexing diff than slides
        std::cout << "On the " << i << "th level of the search tree" << std::endl;
        double featureToAddAtThisLevel;
        double bestSoFarAccuracy = 0;

        for (int k = 1; k < data[0].size(); k++) {
            if (currentSetOfFeatures.find(k) == currentSetOfFeatures.end()) {
                std::cout << "--Considering adding the " << k << " feature" << std::endl;
                double accuracy = leaveOneOutCrossValidation(data, currentSetOfFeatures, k+1);
            
                if (accuracy > bestSoFarAccuracy) {
                    bestSoFarAccuracy = accuracy;
                    featureToAddAtThisLevel = k;
                }
            }   
        }

        currentSetOfFeatures.insert(featureToAddAtThisLevel);
        std::cout << "On level " << i << " I added feature " << featureToAddAtThisLevel << " to current set" << std::endl;
    }
}

std::vector<std::vector<double>> parseData(const std::string &filename) {
    
    return data;

}

int main() {

    parseData("test.txt");
    // parseData("Ver_2_CS170_Fall_2021_Small_data__35.txt");

    // std::vector<std::vector<double>> d;
    // std::vector<double> v1 = {1, 7, 3, 2, 3};
    // std::vector<double> v2 = {2, 3, 1, 3, 2};
    // std::vector<double> v3 = {2, 2, 3, 4, 2};
    // d.push_back(v1);
    // d.push_back(v2);
    // d.push_back(v3);
    // std::unordered_set<double> u;
    // leaveOneOutCrossValidation(d, u, 1);
    // featureSearch(d);

}