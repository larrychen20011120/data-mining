#include "mydatabase.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;


/*********************************************
 * 從檔案中建立 db                            *
 *********************************************/
Database::Database(const string& filename) 
{
    // 初始化 n_trans 和 n_items
    n_trans = n_items = 0;

    string line;
    int item;
    ifstream ifs(filename);
    
    if (!ifs.is_open()) {   // 確認讀檔成功
        std::cout << "Opening file error" << std::endl;
        exit(-1);
    }

    while ( getline(ifs, line) ) { // 讀檔案與建構DB

        vector <int> transaction;
        istringstream iss(line);

        while (iss >> item) {
            transaction.push_back(item);
            // 更新 itemset
            set<int> first_level_set{item};
            itemset.insert(first_level_set);
            // 更新 db
            transactions[first_level_set].insert(n_trans);
        }
        n_trans++; // 交易數量增加
    }

    ifs.close();

    // 記錄所有 item 總數
    n_items = itemset.size();
}


/********************************************************
 * 搜尋特定 pattern 出現次數，並為新的 pattern 更新db      *
 *      ex: pattern = {1, 2, 3, 5}                      *
 *          pattern[0] = {1}, pattern[1:] = {2, 3, 5}   *
 *          transactions[ pattern[0] ] = {10, 12, 15}   *
 *          transactions[ pattern[1:] ] = {10, 11}      *
 * ******************************************************
 *    取交集： {10} 新增到 transactions[pattern]         *
 ********************************************************/
int Database::search_by_pattern(const set<int> pattern)
{
    if (transactions.count(pattern) > 0) {
        // pattern 就在 transactions 裡面
        return transactions[pattern].size();
    } else {
        // 取交集
        auto second_ptr = pattern.begin();
        set<int> a_part(pattern.begin(), ++second_ptr);
        set<int> b_part(second_ptr, pattern.end());
        vector<int> intersect( min(transactions[a_part].size(), transactions[b_part].size()) );
        auto it = set_intersection(
            transactions[a_part].begin(), transactions[a_part].end(),
            transactions[b_part].begin(), transactions[b_part].end(),
            intersect.begin()
        );
        intersect.resize(it-intersect.begin());

        // 更新 transactions
        transactions[pattern] = set<int>(intersect.begin(), intersect.end());

        return intersect.size();
    }
}

/*********************************************
 * 用來刪除一開始不符合 min_sup 的一階產物      *
 *    ex: pattern = { {1}, {3}}              *
 * 直接從transactions中移除這些 key 值         *
 *********************************************/
void Database::delete_from_patterns(const std::set<std::set<int>> patterns) {

    for (auto& transaction: transactions) { // 刪除一階的對應db
        for (auto pattern: patterns) {
            transactions.erase(pattern);
        }        
    }

    for (auto pattern: patterns) { // 更新一階的itemset
        itemset.erase(pattern);
    }        
}

/**********************************************
 * 將資料庫視覺化 -> 可以用來debug              *
 *      (1) Database format:                  *  
 *      (2) 輸出目前所有的組合出現的集合        *
 *          ex: {1,}: {1, 2, 3, 6}            *
 *********************************************/
void Database::show() {
    cout << "Database format:" << endl;
    for (auto kv_pair: transactions) {
        cout << "{ ";
        for (auto index: kv_pair.first) {
            cout << index << ", ";
        }
        cout << "}: ";

        cout << "{ ";
        for (auto index: kv_pair.second) {
            cout << index << ", ";
        }
        cout << "}" << endl;
    }
}

/**   Getters   **/
int Database::get_item_count() {
    return n_items;
}
int Database::get_trans_count() {
    return n_trans;
}
set<set<int>>& Database::get_itemset() {
    return itemset;
}