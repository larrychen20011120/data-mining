#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>
#include <set>
#include <map>
#include "mydatabase.h"

using namespace std;

set<Sequence> construct_sequence_candidates(const set<Item>& itemset, const set<Sequence>& sequence_set);

int main() {
    float min_sup;
    int min_sup_count;

    cout << "min_sup: ";  cin >> min_sup;

    string filename = "seqdata.dat";
    Database db(filename); // 建立 Database

    map<Sequence, int> freq_sequence_table;
    set<Sequence> sequence_candidates;
    set<Item> itemset = db.get_itemset();
    
    min_sup_count = ceil(db.get_seq_count() * min_sup); // 計算 min_sup_count
    vector<Sequence> removed_patterns;

    for (auto item: itemset) {   
        Sequence seq = vector<Item>({item});
        int hit = db.search_level_one_pattern(seq);

        if (hit < min_sup_count) {
            // removed_patterns 新增不滿足的 seq
            removed_patterns.push_back(seq); 
        }
    }

    db.delete_by_patterns(removed_patterns);
    // 取得新的 itemset 作為(level=1)的itemset
    itemset = db.get_itemset(); 

    for (auto item: itemset) { 
        Sequence seq = vector<Item>({item});
        sequence_candidates.insert(seq);
    }

    // 迭代直到沒有 candidates
    while (sequence_candidates.size() != 0) {

        // 產生下一個 level 的 candidates
        sequence_candidates = construct_sequence_candidates(
            itemset, sequence_candidates
        );
        // 刪除不符合 min_sup 的 candidates
        vector<Sequence> to_be_removed;
        for (auto seq: sequence_candidates) {   
            int hit = db.search_by_pattern(seq);

            if (hit < min_sup_count) {
                to_be_removed.push_back(seq);
            } else {
                // 更新 table
                freq_sequence_table[seq] = hit;
            }
        }
        // 刪除不滿足 min_sup 的 candidates
        for (auto removed: to_be_removed)
            sequence_candidates.erase(removed);
        // 一併刪除不滿足的tables
        db.delete_by_patterns(to_be_removed);
    }
    
    // 輸出 freq_sequnce_set
    // 複雜的 format
    ofstream ofs("output.dat");
    ofs << "min_sup: " << min_sup << endl;
    for (auto seq_count_pair: freq_sequence_table) {
        for (int i = 0; i < seq_count_pair.first.size(); ++i) {
            for (auto el : seq_count_pair.first[i]) {
                cout << el << " ";
                ofs << el << " ";
            }
            cout << "|| ";
            ofs << "|| ";
        } 
        cout << "SUP: " << seq_count_pair.second << endl;
        ofs << "SUP: " << seq_count_pair.second << endl;
    }
    ofs.close();

    return 0;
}

/***************************************************
 * 把 level=1 的 item 插入任一個序列中               *
 *   ex: item: {1, 2}, seq: [ {1}, {3}, {4} ]      *
 *       -> [ {1, 2}, {1}, {3}, {4} ]              *
 *       -> [ {1}, {1, 2}, {3}, {4} ]              *
 *       -> [ {1}, {3}, {1, 2}, {4} ]              *
 *       -> [ {1}, {3}, {4}, {1, 2} ]              *
 * ************************************************/
set<Sequence> construct_sequence_candidates(
    const set<Item>& itemset, const set<Sequence>& sequence_set) 
{
    set<Sequence> sequence_candidates;

    for (auto item: itemset) {
        for (auto seq: sequence_set) {
            // <= seq.size() => 最後一個位置也可以插入
            for (int i = 0; i <= seq.size(); ++i) {
                Sequence new_seq;
                new_seq.assign(seq.begin(), seq.end());
                // i 是 Insert index
                new_seq.insert(new_seq.begin() + i, item);
                sequence_candidates.insert(new_seq);
            }
        }
    }
    return sequence_candidates;
}