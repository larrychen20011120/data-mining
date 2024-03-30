#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <math.h>
#include "dbscan.h"

using namespace std;

map<string, string> parse_args(int argc, char *argv[]);
vector<pair<float, float>> standarize(vector<pair<float, float>>& data);

int main (int argc, char *argv[]) {

    map<string, string> args = parse_args(argc, argv);

    // 座標為 float
    float x, y;
    // 資料用 vector 存每一個座標點
    vector<pair<float, float>> data;
    vector<pair<float, float>> standarized_data;


    // 開始讀資料
    ifstream ifs(args["filename"]);

    while (ifs >> x) {
        ifs >> y;
        data.push_back( make_pair(x, y) );
    }
    // 結束讀檔
    ifs.close();
    
    // 建立 DBSCAN 模型
    DBScan model(stof(args["radius"]), stof(args["min_samples"]));

    standarized_data = standarize(data); // 標準化資料
    model.fit(standarized_data);

    ofstream ofs(args["filename"] + ".out");

    cout << "radius: " << args["radius"] << ", min_samples: " << args["min_samples"] << endl;
    cout << "number of clusters: " << model.get_n_clusters() << endl;
    cout << "================================================================================\n";

    for (int i = 0; i < data.size(); ++i) {
        int cluster = model.predict(standarized_data[i]);
        //cout << data[i].first << " " << data[i].second << " " << cluster << endl;
        ofs << data[i].first << " " << data[i].second << " " << cluster << endl;
    }

    ofs.close();

    return 0;
}

vector<pair<float, float>> standarize(vector<pair<float, float>>& data) {
    
    float mean_x, mean_y, std_x, std_y;
    mean_x = mean_y = std_x = std_y = 0.0f;
    vector<pair<float, float>> standarized_data;

    // 計算平均值
    for (auto point: data) {
        mean_x += point.first;
        mean_y += point.second;
    }
    mean_x /= data.size(); mean_y /= data.size();
    
    // 計算標準差
    for (auto point: data) {
        std_x += ((point.first - mean_x) * (point.first - mean_x));
        std_y += ((point.second - mean_y) * (point.second - mean_y));
    }
    std_x = sqrt(std_x / data.size());
    std_y = sqrt(std_y / data.size());
    
    // 計算標準化資料
    for (auto point: data) { // 標準化
        standarized_data.push_back(
            make_pair(
                (point.first-mean_x) / std_x,
                (point.second-mean_y) / std_y
            )
        );
    }
    return standarized_data;
}

inline map<string, string> parse_args(int argc, char *argv[]) {
    map<string, string> args;
    
    for (int i = 0; i < argc; ++i) {
        string arg_name(argv[i]);
        if (arg_name == "--filename") {
            args["filename"] = string(argv[i+1]);
        } else if (arg_name == "--radius") {
            args["radius"] = string(argv[i+1]);
        } else if (arg_name == "--min_samples") {
            args["min_samples"] = string(argv[i+1]);
        }
    }

    return args;
}
