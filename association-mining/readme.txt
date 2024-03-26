1. 編譯與執行
    g++ -o main .\main.cpp .\mydatabase.cpp
    .\main

2. 程式執行後可以分別輸入 min_sup 和 min_conf

* 目前輸出結果 "output.txt" 為 "input.txt" 設定 min_sup=0.02, min_conf=0.8的結果
* 實作演算法為 apriori 但使用反向索引的 db，且從db刪除第一次就不符合的item減少運算量。