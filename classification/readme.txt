根據資料分析(eda.ipynb)的分析結果可以發現以下特點：
    
    * marital_status含有缺失值，其中只有'M'出現在資料當中，所以直接猜測'S'是缺失值的值
    * num_children_at_home，我直觀的認為填補為0
    * member_card含有缺失值，作業說明一律視為 Basic
    * age, year_income 沒有缺失值，但需要類別化好實作模型
        * age分布從20-90，同時分布圖中有少部分的柱狀數量<=4，我將這些視為分界點，並排除過於接近的點
            * 27, 38, 46, 62, 74 是分界點，共分出 6 類
        * year_income 只有 20000 - 160000 且間隔為20000遞增。根據分布可以看出明顯的三群並定義:
            * 7w以下為低收入
            * 7w-11w為中收入
            * 高收入則是高過於11w

編譯程式
    g++ -o main .\main.cpp .\dataset.cpp .\bayesian.cpp

執行程式
    main.exe

輸出至 terminal 和 output.txt
    * 每筆資料和其預測結果
    * 最後輸出預測的準確度 ~ 0.655

結論
    * 資料特徵數少以及切分方法不好，導致likelihood機率差異不大，而預測結果多受prior影響只輸出Basic和Gold
    * 若不考慮 prior 又會導致準確度更低