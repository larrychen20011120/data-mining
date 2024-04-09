#include <string>
#include <vector>
#include <map>

#ifndef DATASET_H_
#define DATASET_H_

class Dataset {

public:
    // construct dataset
    Dataset(const std::string& filename);
    // showing dataset
    void show();
    std::vector<std::map<std::string, int>> get_data();

private:
    
    const std::string feature_names[5] = {
        "marital_status", "num_children_at_home", "member_card", "age", "year_income"
    };

    std::vector<std::map<std::string, int>> data;

};

#endif