演算法實作
    * Vertical DB 的形式
    * 可以判斷同一個物品重複出現的 pattern (ex: 123 || 123 || 123)

編譯程式
    g++ -o main main.cpp mydatabase.cpp

執行程式
    main.exe
    輸入 min_sup (小數值)

output.dat
    seqdata.dat 以 min_sup=0.005 生成的結果
    輸出格式:
        第一行     -> min_sup: <min_sup user 輸入的值>
        第二行以後 -> < sequential pattern (用 || 隔開) + SUP:<出現次數> > 