#include <string>
#include <vector>
#include <set>
#include <map>

#ifndef MYDATABASE_H_
#define MYDATABASE_H_

class Database {

public:
    // 透過資料檔來建構 transactions
    Database (const std::string& filename);
    // 搜尋特定 pattern
    int search_by_pattern(const std::set<int> pattern);
    // 刪除特定 patterns
    void delete_from_patterns(const std::set<std::set<int>> patterns);
    // 獲取 item 總數
    int get_item_count();
    // 獲取 transaction 數量
    int get_trans_count();
    // 獲取現有的所有一階 items, ex: { {1}, {2}, {3} }
    std::set<std::set<int>>& get_itemset(); 
    // print out db
    void show();

private:
    // 總共有幾個 item, transaction
    int n_items, n_trans;
    // 收集所有的 item
    std::set<std::set<int>> itemset;

    // 將 database 從每筆交易轉為 => item 出現在那些交易的儲存方式
    // map[組合] = { 哪幾筆交易 }
    // EX:  (1) map[{A}] = {1, 2, 3, 8}
    //      (2) map[{A, B, C}] = {1, 2}
    std::map < std::set<int>, std::set<int> > transactions;
};

#endif