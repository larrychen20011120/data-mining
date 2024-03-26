#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <bitset>
#include <algorithm>
#include <math.h>
#include "mydatabase.h"


using namespace std;

void run_apriori(Database& db, const float min_sup, const float min_conf);
set<set<int>> generate_candidates(vector<set<int>> freq_itemset, int level);
map<set<int>, int> prune_candidates(set<set<int>>& candidates, Database& db, int min_sup_count);
vector<pair<vector<int>, vector<int>>> generate_rules(set<int> freq_itemset);

int main () {

    float min_sup, min_conf;

    // 創建 DB
    string filename = "input.txt";
    Database db(filename);

    // 使用者輸入 min_sup 和 min_conf
    cout << "min_sup: ";  cin >> min_sup;
    cout << "min_conf: "; cin >> min_conf;
    
    // 跑 apriori 結果
    run_apriori(db, min_sup, min_conf); 
    return 0;
}

vector<pair<vector<int>, vector<int>>> generate_rules(set<int> freq_itemset)
{   
    // n_combinations -> 計算所有可能的組合 (2^N - 2)
    unsigned int n_combinations = ((unsigned int)1 << freq_itemset.size()) - 2;
    const int n = 32;
    vector<int> freq_items(freq_itemset.begin(), freq_itemset.end());
    vector<pair<vector<int>, vector<int>>> left_right_parts;

    for (unsigned int comb = 1; comb <= n_combinations; ++comb) {
        bitset<n> is_left(comb);  // bitset 來表示哪一個 item 放在左邊
        vector<int> left_part, right_part;
        
        for (int i = 0 ; i < freq_itemset.size(); ++i)
            if (is_left[i])  // bit = 1: 放左邊
                left_part.push_back(freq_items[i]);
            else 
                right_part.push_back(freq_items[i]);

        // 確保輸出結果有排序，不然集合會長很醜
        sort(left_part.begin(), left_part.end());
        sort(right_part.begin(), right_part.end());
        left_right_parts.push_back(make_pair(left_part, right_part));
    }
    
    return left_right_parts; // 回傳左右兩邊的結果
}

/*************************************************
 * 從 freq_itemset 建立下一個 level 的 candidates *
 *      (1) 倆倆產生下一個 candidate (union)      *
 *      (2) 確定 candidate 的數量是符合 level 的  *
 *************************************************/
set<set<int>> generate_candidates(vector<set<int>> freq_itemset, int level) 
{
    set<set<int>> candidates;

    // 利用 freq_itemset 交互生成 candidates
    for (int i = 0; i < freq_itemset.size(); ++i) {
    
        for (int j = 0; j < freq_itemset.size(); ++j) {
            
            if (i != j) { // 自己跟自己沒有意義
                vector<int> candidate(freq_itemset[i].size() + freq_itemset[j].size());
                vector<int>::iterator it = set_union(
                    freq_itemset[i].begin(), freq_itemset[i].end(),
                    freq_itemset[j].begin(), freq_itemset[j].end(),
                    candidate.begin()
                );
                candidate.resize(it-candidate.begin());  
                
                // 確認 candidate 符合 level 限制
                if (candidate.size() == level) {
                    candidates.insert(set<int> (candidate.begin(), candidate.end()));
                }
            }
        }
    }
    return candidates;
}
/*********************************************
 * 掃過 DB 並刪除沒有符合 support 的 candidate *
 *********************************************/
map<set<int>, int> prune_candidates(set<set<int>>& candidates, Database& db, int min_sup_count) 
{
    map<set<int>, int> table;
    
    for (auto candidate: candidates) {
        int freq = db.search_by_pattern(candidate);
        if (freq >= min_sup_count) 
            table[candidate] = freq;
    }

    return table; // 有符合 support 對應的次數
}

void run_apriori(Database& db, const float min_sup, const float min_conf) 
{    
    const int max_size = db.get_item_count();
    const int min_sup_count = ceil(db.get_trans_count() * min_sup);
    set<set<int>> first_level_candidates = db.get_itemset();
    /****************************************************
     *  freq_table: array of table                      *
     *      index -> level of freq_itemset              *
     *      table -> each level freq_itemset counts     *
     ****************************************************/
    vector< map<set<int>, int> > freq_table;


    // 產生一階freq_table
    map<set<int>, int> table;
    table = prune_candidates(first_level_candidates, db, min_sup_count);

    // 可以先從資料庫刪除一階就不符的
    set<set<int>> patterns;
    for (auto candidate: first_level_candidates) {
        if (!table.count(candidate))
            patterns.insert(candidate);
    }
    db.delete_from_patterns(patterns);

    // 將一階的所有itemset存到table
    freq_table.push_back(table); 


    for (int level = 2; level <= max_size; ++level) {
        // 從 (i-1) table 取出 freq_itemset
        vector<set<int>> freq_itemset;
        for (auto kv_pair: table) {
            freq_itemset.push_back(kv_pair.first);
        }
        // 根據(i-1) freq_itemset 建出所有可行的 candidates
        set<set<int>> candidates = generate_candidates(freq_itemset, level);
        // 由 candidate 產生下一階 freq_table
        table = prune_candidates(candidates, db, min_sup_count);
        freq_table.push_back(table);
    }

    ofstream ofs("output.txt");

    cout << "min_sup: " << min_sup << ", min_conf: " << min_conf << endl;  
    ofs << "min_sup: " << min_sup << ", min_conf: " << min_conf << endl; 

    for (int level = 1; level < freq_table.size(); level++) {
        for (auto kv_pair: freq_table[level]) {
            
            // 生成規則
            vector<pair<vector<int>, vector<int>>> rules = generate_rules(kv_pair.first);
            
            for (auto rule : rules) {
                
                set<int> left_itemset(rule.first.begin(), rule.first.end());
                float conf = ((float)kv_pair.second) / ((float)freq_table[left_itemset.size()-1][left_itemset]);
                
                if (conf >= min_conf) {

                    // 設定複雜的輸出格式
                    // 包括檔案與terminal輸出
                    
                    if (rule.first.size() == 1) {
                        cout << rule.first[0]; ofs << rule.first[0];
                    } else if (rule.first.size() > 1) {
                        cout << "{"; ofs << "{";
                        for (int i = 0; i < rule.first.size(); ++i) {
                            if (i != rule.first.size()-1) {
                                cout << rule.first[i] << ", ";
                                ofs << rule.first[i] << ", ";
                            } else {
                                cout << rule.first[i];
                                ofs << rule.first[i];
                            }
                        }
                        cout << "}"; ofs << "}";
                    }
                    cout << " -> "; ofs << " -> ";

                    if (rule.second.size() == 1) {
                        cout << rule.second[0]; ofs << rule.second[0];
                    } else if (rule.second.size() > 1) {
                        cout << "{"; ofs << "{";
                        for (int i = 0; i < rule.second.size(); ++i) {
                            if (i != rule.second.size()-1) {
                                cout << rule.second[i] << ", ";
                                ofs << rule.second[i] << ", ";
                            } else {
                                cout << rule.second[i];
                                ofs << rule.second[i] << ", ";
                            }
                        }
                        cout << "} "; ofs << "} ";
                    }
                    cout << "(" << conf << ")" << endl;
                    ofs << "(" << conf << ")" << endl;
                }
            }
        }
    
    }
    ofs.close();
}