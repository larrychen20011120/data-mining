實作演算法 dbscan
    * 資料會根據x, y兩軸分別標準化，所以radius取值為小數
    * 先做分群並辨識出 NOISE
    * 根據現有的群算出群中心後，幫NOISE分到這些群裡(依照與群中心的距離)
    * 參數影響非常大

編譯可執行檔
    g++ -o main main.cpp dbscan.cpp

執行單一檔案 (參數包括 filename, radius, min_samples)
.\main.exe --filename Clustering_testdata\Clustering_test5 --radius 0.12 --min_samples 14

一次執行多個檔案
./run.sh

看分群結果 -> 執行 plot.ipynb (需有matplotlib)

輸出檔名為輸入"檔名.out"