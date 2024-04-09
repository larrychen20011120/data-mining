#include "dataset.h"
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

Dataset::Dataset(const string& filename) {

    ifstream ifs(filename);
    string line, block;
    int feature_index, feature_category;
    string feature_value;

    if (!ifs.is_open()) {   // 確認讀檔成功
        cout << "Opening file error" << endl;
        exit(-1);
    }
    while (getline(ifs, line)) {

        // marital_status 預設 'S', num_children_at_home 預設為 0, member_card=0 表示 Basic
        map<string, int> datum;
        for (auto feature_name: feature_names) 
            datum[feature_name] = 0;
        
        // 切除頭尾的 { }
        line = line.substr(1, line.size()-1);
        istringstream iss_1(line);

        while (getline(iss_1, block, ',')) {
            
            float age, income;

            istringstream iss_2(block);
            iss_2 >> feature_index;
            iss_2.ignore();
            iss_2 >> feature_value;

            // 讀取 datum
            switch (feature_index) {
                case 0:
                    if (feature_value == "M")
                        feature_category = 1;
                    else
                        feature_category = 0;
                    break;
                case 1:
                    feature_category = stoi(feature_value);
                    break;
                case 2:
                    if (feature_value == "Basic") {
                        feature_category = 0;
                    } else if (feature_value == "Normal") {
                        feature_category = 1;
                    } else if (feature_value == "Silver") {
                        feature_category = 2;
                    } else {
                        feature_category = 3;
                    }
                    break;
                case 3:
                    age = stof(feature_value);
                    if (age <= 27.0) 
                        feature_category = 0;
                    else if (age > 27.0 && age <= 38.0) 
                        feature_category = 1;
                    else if (age > 38.0 && age <= 46.0) 
                        feature_category = 2;
                    else if (age > 46.0 && age <= 62.0) 
                        feature_category = 3;
                    else if (age > 62.0 && age <= 74.0) 
                        feature_category = 4;
                    else if (age > 74.0)
                        feature_category = 5;
                    break;
                case 4:
                    income = stof(feature_value);
                    if (income <= 70000.0) 
                        feature_category = 0;
                    else if (income > 70000.0 && income <= 110000.0) 
                        feature_category = 1;
                    else if (income > 110000.0)
                        feature_category = 2; 
                    break;
            }
            datum[ feature_names[feature_index] ] = feature_category;

        }
        // 更新到 data
        data.push_back(datum);
    }
    ifs.close();
}

void Dataset::show() {
    for (int i = 0; i < data.size(); ++i) {
        auto datum = data[i];
        cout << "(" << i+1 << ")   ";
        for (auto item: datum) {
            cout << item.first << ": " << item.second << " ; ";
        }
        cout << endl;
    }
}

vector<map<string, int>> Dataset::get_data() {
    return data;
}