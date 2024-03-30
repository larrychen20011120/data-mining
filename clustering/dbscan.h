#include <vector>
#include <map>
#include <set>

#ifndef DBSCAN_H_
#define DBSCAN_H_

class DBScan {

public:
    DBScan(const float& radius, const float& min_samples);
    void fit(const std::vector< std::pair<float, float> >& data);
    int predict(const std::pair<float, float>& pair);
    int get_n_clusters();

private:
    std::map< std::pair<float, float>, int> data2cluster;
    int n_clusters;
    float radius, min_samples;
    std::vector<std::pair<float, float>> cluster_centers;
    std::set<std::pair<float, float>> visited;

    void expand_cluster(
        const std::pair<float, float>& point, 
        std::vector< std::pair<float, float> >& neightbor_points,
        const std::vector< std::pair<float, float> >& data
    );
    std::vector< std::pair<float, float> > region_query(
        const std::pair<float, float>& curr_point,
        const std::vector< std::pair<float, float> >& data
    );
    void adjust_noises();
};


#endif