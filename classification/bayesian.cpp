#include "bayesian.h"
#include <iostream>

using namespace std;

NaiveBayes::NaiveBayes(double pseudo_count, bool need_prior) {
    this->pseudo_count = pseudo_count;
    this->need_prior = need_prior;
}

void NaiveBayes::fit(vector<map<string, int>> data, string label_name) {
    
    this->label_name = label_name;
    // 紀錄所有特徵名稱
    for (auto item : data[0]) {
        if (item.first != label_name)
            feature_names.push_back(item.first);
    }
    // 紀錄特徵與標籤的 unique 值
    for (auto datum : data) {
        for (auto item : datum)
        feature_unique_values[item.first].insert(item.second);
    }

    n_class = feature_unique_values[label_name].size(); // 取得 n_class

    // allocate memory
    prior_probs = new double[n_class];
    for (int i = 0; i < n_class; ++i)
        prior_probs[i] = pseudo_count; 

    for (auto name: feature_names) {
        // allocate memory
        feature_likelihood_probs[name] = new double* [feature_unique_values[name].size()];
        for (int i = 0; i < feature_unique_values[name].size(); ++i) 
            feature_likelihood_probs[name][i] = new double [n_class];

        // 初始化
        for (int i = 0; i < feature_unique_values[name].size(); ++i) 
            for (int j = 0; j < n_class; ++j)
                feature_likelihood_probs[name][i][j] = pseudo_count;
    }

    // learning by counting
    for (int i = 0; i < data.size(); ++i) {
        for (auto item : data[i]) {
            if (item.first != label_name) {

                string name = item.first;
                int category = item.second;
                
                feature_likelihood_probs[name][category][data[i][label_name]]++;

            } else {
                prior_probs[item.second]++;
            }
        }
    }
    // 轉換 count 成 prob
    for (auto name: feature_names) {
        for (int i = 0; i < feature_unique_values[name].size(); ++i) {
            for (int c = 0; c < n_class; ++c) {
                feature_likelihood_probs[name][i][c] /= prior_probs[c];
            }
        }
    }
    int sum = 0;
    for (int i = 0; i < n_class; ++i)
        sum += prior_probs[i];

    for (int i = 0; i < n_class; ++i)
        prior_probs[i] /= sum;
}

int NaiveBayes::predict(map<string, int> datum) {
    
    // posterior 機率預設為 prior 或 1
    double posterior[n_class];

    if (need_prior) {
        for (int i = 0; i < n_class; ++i)   
            posterior[i] = prior_probs[i];
    } else {
        for (int i = 0; i < n_class; ++i)   
            posterior[i] = 1.0f;
    }

    for (auto item: datum) {
        if (item.first != label_name) {
            
            string name = item.first;
            int category = item.second;

            for (int c = 0; c < n_class; ++c) {
                posterior[c] *= feature_likelihood_probs[name][category][c];
            }

        }
    }

    double max_value = -1.0f;
    int max_index = -1;

    for (int i = 0; i < n_class; ++i){
        if (posterior[i] > max_value){
            max_value = posterior[i];
            max_index = i;
        }
    }
    return max_index;
}

void NaiveBayes::show_prior() {
    cout << "[ ";
    for (int c = 0; c < n_class; ++c)
        cout << prior_probs[c] << ", ";
    cout << "]\n";
}

void NaiveBayes::show_likelihood_by_name(string name) {
    cout << "[ " << name << " ]\n";
    for (int i = 0; i < feature_unique_values[name].size(); ++i) {
        cout << i << ": ";
        for (int c = 0; c < n_class; ++c) {
            cout << feature_likelihood_probs[name][i][c] << " ";
        }
        cout << endl;
    }
}