#include "dbscan.h"
#include <math.h>

using namespace std;

/********************************************
 *                                          *
 * 實作內容參考維基百科的 pseudo code         *
 * https://zh.wikipedia.org/zh-tw/DBSCAN    *
 *                                          *
*********************************************/

DBScan::DBScan(const float& radius, const float& min_samples) {
    // 數值初始化
    this->radius = radius; 
    this->min_samples = min_samples;
}

int DBScan::predict(const pair<float, float>& pair) {
    return data2cluster[pair];
}
int DBScan::get_n_clusters() {
    return this->n_clusters;
}

void DBScan::fit(const vector<pair<float, float>>& data) {

    this->n_clusters = 0; // 初始化 n_clusters
    for (auto point: data)
        data2cluster[point] = -1; // 初始化至-1

    for (auto point: data) {
        if (!visited.count(point)) {
            visited.insert(point);
            vector< pair<float, float> > neighbor_points;
            neighbor_points = region_query(point, data);

            if (neighbor_points.size() < min_samples) {
                // NOISE -> -2
                data2cluster[point] = -2;
            } else {
                // 遞增 n_clusters -> 擴展 cluster
                n_clusters++;
                expand_cluster(point, neighbor_points, data);
            }
        }
    }
    this->adjust_noises();

}

void DBScan::expand_cluster(
    const pair<float, float>& point, 
    vector< pair<float, float> >& neighbor_points,
    const vector< pair<float, float> >& data
    )
{
    data2cluster[point] = n_clusters;
    int neighbor_size = neighbor_points.size();
    for (int i = 0; i < neighbor_size; ++i) {
        if (!visited.count(neighbor_points[i])) {
            visited.insert(neighbor_points[i]);
            vector< pair<float, float> > new_neighbor_points;
            new_neighbor_points = region_query(neighbor_points[i], data);

            if (new_neighbor_points.size() >= min_samples) {
                // 擴增 neighbor_points
                for (auto point: new_neighbor_points) {
                    neighbor_points.push_back(point);
                }
                neighbor_size = neighbor_points.size();
            }

            if (data2cluster[neighbor_points[i]] == -1) {
                // not yet in other cluster
                data2cluster[neighbor_points[i]] = n_clusters;
            }
        }
    }
}

vector< pair<float, float> > DBScan::region_query(
    const pair<float, float>& curr_point, 
    const vector< pair<float, float> >& data) 
{
    vector< pair<float, float> > points;
    // 計算座標邊界
    float max_x, min_x, max_y, min_y;
    max_x = curr_point.first + radius;
    min_x = curr_point.first - radius;
    max_y = curr_point.second + radius;
    min_y = curr_point.second - radius;

    for (auto point: data) {

        if (point.first <= max_x && point.first >= min_x &&
            point.second <= max_y && point.second >= min_y   ) 

            points.push_back(point); // 收集符合邊界的點
    }

    return points;
}

void DBScan::adjust_noises() {

    // 計算cluster中心點
    cluster_centers.push_back(make_pair(0.0f, 0.0f)); // 忽略第一個位置(index從1開始)
    float mean_x[n_clusters+1], mean_y[n_clusters+1];
    int count_x[n_clusters+1], count_y[n_clusters+1];

    // 計算總和與數量
    for (int i = 1; i <= n_clusters; i++) {
        mean_x[i] = mean_y[i] = 0.0f;
        count_x[i] = count_y[i] = 0;
    }

    for (auto& point2cluster: data2cluster) {
        int cluster = point2cluster.second;
        mean_x[cluster] += point2cluster.first.first;
        mean_y[cluster] += point2cluster.first.second;
        count_x[cluster]++;
        count_y[cluster]++;
    }

    // 計算各群中心點：(mean_x, mean_y)
    for (int i = 1; i <= n_clusters; ++i) {
        cluster_centers.push_back(
            make_pair(
                mean_x[i] / count_x[i], 
                mean_y[i] / count_y[i]
            )
        );
    }

    for (auto point2cluster: data2cluster) {
        
        if (point2cluster.second == -2) {
            
            float x = point2cluster.first.first;    
            float y = point2cluster.first.second;
            int min_cluster = data2cluster.size()+1;
            float min_dist = 1000;

            for (int i = 1; i <= n_clusters; ++i) {
                float dist = sqrt(
                    (x - cluster_centers[i].first) * (x - cluster_centers[i].first) +
                    (y - cluster_centers[i].second) * (y - cluster_centers[i].second) 
                );
                if (dist < min_dist) {
                    min_dist = dist;
                    min_cluster = i;
                }
            }

            data2cluster[point2cluster.first] = min_cluster;

        }

    }
}