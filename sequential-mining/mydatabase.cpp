#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "mydatabase.h"

using namespace std;

/*********************************************
 * 從檔案中建立 db                            *
 *********************************************/
Database::Database(const string& filename) 
{
    // 初始化 n_items
    n_seqs = 0;

    string line;
    int seq_id, timestamp, item;

    ifstream ifs(filename);
    
    if (!ifs.is_open()) {   // 確認讀檔成功
        std::cout << "Opening file error" << std::endl;
        exit(-1);
    }

    std::map<Sequence, Table> tables;

    while ( getline(ifs, line) ) { // 讀檔案與建構DB

        istringstream iss(line);

        iss >> seq_id;

        int curr_timestamp = -1000;
        vector<int> same_time_items;

        while (iss >> timestamp) {
            iss >> item;

            if (curr_timestamp == timestamp) {
                // 同一個 timestamp 繼續存下來
                same_time_items.push_back(item);
            } else {
                // 加入所有 subsets
                vector<Item> subsets = find_all_subsets(same_time_items);
                for (auto subset: subsets) {
                    itemset.insert(subset);
                    Sequence seq;
                    seq.push_back(subset);

                    struct Column c = {.sid=seq_id, .eid=curr_timestamp};
                    tables[seq].push_back(c);
                }
                same_time_items.clear();
                same_time_items.push_back(item);
                curr_timestamp = timestamp;
            }

        }
        n_seqs++; // 序列數量增加

        // 序列結尾也要加入所有 subsets
        vector<Item> subsets = find_all_subsets(same_time_items);
        for (auto subset: subsets) {
            itemset.insert(subset);
            Sequence seq;
            seq.push_back(subset);

            struct Column c = {.sid=seq_id, .eid=curr_timestamp};
            tables[seq].push_back(c);
        }
    }

    level2tables[1] = tables; // 建好 level=1 的表

    ifs.close();
}

int Database::search_level_one_pattern(const Sequence& pattern) {
    if (pattern.size() != 1) {
        cout << "Error! It's not level 1 pattern.\n";
        return 1;
    }
    Table table = level2tables[1][pattern];
    auto column = table.begin();
    int prev_seq_id = -1000, hit = 0;

    while (column != table.end()) {
        if (column->sid != prev_seq_id) {
            hit++;
            prev_seq_id = column->sid;
        }
        column++;
    }
    return hit;
}

int Database::search_by_pattern(const Sequence& pattern) {
    set<int> hit_seq_ids;
    auto ptr = pattern.begin();
    vector<set<int>> a_part(pattern.begin(), ++ptr);
    vector<set<int>> b_part(ptr, pattern.end()); 

    // 尋找方法：[1, 2, 3, 5]: [1] 後面跟著 [2, 3, 5]
    if (!level2tables[a_part.size()].count(a_part) || !level2tables[b_part.size()].count(b_part)) {
        // table 不存在任意一個
        return 0;
    }
    Table first_table = level2tables[a_part.size()][a_part];
    Table last_table  = level2tables[b_part.size()][b_part];
    Table target_table;

    auto first_column = first_table.begin();
    auto last_column  = last_table.begin();

    while (first_column != first_table.end() && last_column != last_table.end()) {
        
        if (first_column->sid > last_column->sid) {
            // last column 往下找
            last_column++;
        } else if (first_column->sid < last_column->sid) {
            first_column++; // first column 往下找
        } else {
            int seq_id = first_column->sid;
            Table::iterator first_start = first_column;
            Table::iterator last_start = last_column;
            Table::iterator first_end, last_end;

            for (;first_column != first_table.end() && first_column->sid == seq_id; first_column++) 
                ; // do nothing
            first_end = first_column;
            for (;last_column != last_table.end() && last_column->sid == seq_id; last_column++) 
                ; // do nothing
            last_end = last_column;

            // sequence id 一樣，找 eid 是否合適
            for (first_column = first_start; first_column != first_end; first_column++) {
                for (last_column = last_start; last_column != last_end; last_column++) {
                    if (first_column->eid < last_column->eid) {
                        hit_seq_ids.insert(first_column->sid);
                        struct Column c = {.sid=first_column->sid, .eid=first_column->eid};
                        target_table.push_back(c);
                    }
                }
            }

        }

    }

    if (target_table.size() >= 1) // 有東西再存
        level2tables[pattern.size()][pattern] = target_table;

    return hit_seq_ids.size();
}

void Database::delete_by_patterns(const vector<Sequence>& patterns) {
    for (auto pattern : patterns) {
        level2tables[pattern.size()].erase(pattern);
        if (pattern.size() == 1)
            itemset.erase(pattern[0]);
    }
}

void Database::show() {
    for (auto level_table_pair: level2tables) {
        cout << "=================== level: " << level_table_pair.first << " ======================" << endl;
        for (auto seq_table_pair: level_table_pair.second) {
            
            for (auto item : seq_table_pair.first) {
                cout << "\t{";
                for (auto el: item) {
                    cout << el << ", ";
                } 
                cout << "}";
                cout << " || \n";
            } cout << "-------------------------" << endl;
            cout << "\tSID\tEID\t\n";
            
            for (auto col: seq_table_pair.second) {
                cout << "\t" << col.sid << "\t" << col.eid << endl;
            }

            cout << "-------------------------" << endl;
        }
    }
}

vector<Item> Database::find_all_subsets(vector<int>& same_time_items) {
    int n = same_time_items.size();
    vector<Item> all_items;

    for (int i = 0; i < (1 << n); i++) {
        Item item;
        // LOOP 所有可能
        for (int j = 0; j < n; j++) {
             // 第 j 個 bit subset
            if ((i & (1 << j)) != 0) {
                item.insert(same_time_items[j]);
            }
        }

        if (item.size() != 0) // 過濾空集合
            all_items.push_back(item);
    }
    return all_items;
}

int Database::get_seq_count() {
    return n_seqs;
}
int Database::get_item_count() {
    return itemset.size();
}
set<Item>& Database::get_itemset() {
    return itemset;
}