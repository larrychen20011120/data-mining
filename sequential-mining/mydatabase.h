#include <string>
#include <vector>
#include <set>
#include <map>

#ifndef MYDATABASE_H_
#define MYDATABASE_H_

struct Column {
    int sid;
    int eid;
};

using Item = std::set<int>;
using Sequence = std::vector<Item>;
using Table = std::vector<struct Column>;

class Database {

public:
    // 透過資料檔來建構 DB
    Database (const std::string& filename);
    // 搜尋特定 pattern
    int search_by_pattern(const Sequence& pattern);
    int search_level_one_pattern(const Sequence& pattern);
    // 刪除 level=1 patterns
    void delete_by_patterns(const std::vector<Sequence>& patterns);
    // 獲取 item 總數
    int get_item_count();
    // 獲取 transaction 數量
    int get_seq_count();
    // 獲取現有的所有一階 items, ex: { {1}, {2}, {3} }
    std::set<Item>& get_itemset(); 
    void show();

private:
    // 總共有幾個 item, sequence
    int n_seqs;
    // itemset
    std::set<Item> itemset;
    // 把序列轉成 vertical db 形式
    // 每一個 sequence 對應到 SID 和 EID
    std::map<int, std::map<Sequence, Table> > level2tables;
    std::vector<Item> find_all_subsets(std::vector<int>& same_time_items);
};

#endif