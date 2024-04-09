#include <vector>
#include <map>
#include <string>
#include <set>

#ifndef DECISION_TREE_H_
#define DECISION_TREE_H_

class NaiveBayes {
public:
    NaiveBayes(double pseudo_count, bool need_prior);
    void fit(std::vector< std::map<std::string, int> > data, std::string label_name);
    int predict(std::map<std::string, int> datum);

    void show_prior();
    void show_likelihood_by_name(std::string name);

private:
    bool need_prior;
    int n_class;
    std::string label_name;
    double pseudo_count;
    double* prior_probs;

    std::vector<std::string> feature_names;
    std::map<std::string, std::set<int>> feature_unique_values;

    // age -> 6 x n_class 
    std::map<std::string, double**> feature_likelihood_probs;
};

#endif