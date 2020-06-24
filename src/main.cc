
#include "solver.h"

#include "pair.h"

#include <iostream>
#include <vector>
/*
 int min_v_shift = 0;
int min_h_shift = -25;
int max_v_shift = 25;
int max_h_shift = 0;
 */
int min_v_shift = 0;
int min_h_shift = 0;
int max_v_shift = 30;
int max_h_shift = 45;
double alpha = 1.;
double betta = 40.;

int main()
{
    Pair pair("../img/mine/l_s.png","../img/mine/r_s.png");
    std::vector<std::vector<pixel>>& depth = pair.depth;

   /* for(int i=0; i< pair.height; i++){
        for(int j=0; j<pair.width; j++){
            std::cout<<pair.left[i][j].r+pair.left[i][j].g+pair.left[i][j].b<<"\t";
        }
        std::cout << "\n";
    }
    for(int i=0; i< pair.height; i++){
        for(int j=0; j<pair.width; j++){
            std::cout<<pair.right[i][j].r+pair.right[i][j].g+pair.right[i][j].b<<"\t";
        }
        std::cout << "\n";
    }

    auto mat = get_row_diff(pair, 0);
    int v = max_v_shift-min_v_shift;
    int h = max_h_shift-min_h_shift;
    for (auto& k : mat)
    {
        for (auto& v : k)
        {
            for (auto h : v)
            {
                std::cout << h <<" ";
            }
            std::cout << "\n";
        }
        std::cout << "\n\n";
    }*/

  #pragma omp parallel for
    for(int row=pair.img_begin_v; row<pair.img_end_v; ++row)
    {
        auto mat = get_row_diff(pair, row);

        solve(mat);
        auto shifts = back_prop(mat);
//        std::cout << "size "<<shifts.size()<<" "<< shifts[0].size() <<"\n";
        
        write_row(depth, shifts , row);
        std::cout<<"row "<<row<<"\n";
    }
    pair.save_img(depth);

    return 0;
}
