#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "dataset.h"
#include "bayesian.h"

using namespace std;

float calculate_accuracy(vector<int> y_true, vector<int> y_pred);

int main() {

    // 標籤定義
    const string label_names[] = {"Basic", "Normal", "Silver", "Gold"};
    // 超參數 > pseudo count, need_prior
    const double pseudo_count = 1.0;
    const bool need_prior = true;

    // 訓練與測試檔案名稱
    const string train_filename = "training.txt";
    const string test_filename = "test.txt";

    // 讀取檔案為 dataste 格式
    Dataset train_dataset(train_filename);
    Dataset test_dataset(test_filename);

    NaiveBayes model(pseudo_count, need_prior);
    // learning
    model.fit(train_dataset.get_data(), "member_card");

    vector<int> y_pred, y_true;
    // inference
    for (auto datum : test_dataset.get_data()) {
        for (auto item: datum) {
            int pred = model.predict(datum);
            y_pred.push_back(pred);
            y_true.push_back(datum["member_card"]);
        }
    }

    /** 輸出預測結果 **/
    ofstream ofs("output.txt");
    ifstream ifs("test.txt");

    // 輸出每筆預測資料
    int count = 0;
    string line;

    while (getline(ifs, line)) {
        cout << line << " member_card = " << label_names[y_pred[count]] << endl;
        ofs  << line << " member_card = " << label_names[y_pred[count]] << endl;
        count++;
    }

    // 輸出正確率
    cout << "=====================\n";     ofs << "=====================\n";
    cout << "| Accuracy: " << calculate_accuracy(y_true, y_pred) << "|" << endl;    
    ofs  << "| Accuracy: " << calculate_accuracy(y_true, y_pred) << "|" << endl;
    cout << "=====================\n";     ofs << "=====================\n";

    return 0;
}


float calculate_accuracy(vector<int> y_true, vector<int> y_pred) {
    if (y_pred.size() != y_true.size()) {
        cout << "Input sizes are not match!!\n";
        exit(1);
    }
    float count = 1.0f;
    for (int i = 0; i < y_pred.size(); ++i)
        if (y_true[i] == y_pred[i])
            count++;
    return count/y_pred.size();
}