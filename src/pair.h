#pragma once

#include <vector>
#include "CImg/CImg.hpp"
#include <string>
#include <limits>
#include <iostream>
#include <assert.h>
#include <fstream>

using namespace cimg_library;
using Lines = std::vector<std::vector<double>>;

extern int min_v_shift;
extern int min_h_shift;
extern int max_v_shift;
extern int max_h_shift;
extern double alpha;
extern double betta;

struct pixel{
int r,g,b;

double operator-(pixel& pix)
{
    return std::abs(r - pix.r) + std::abs(g - pix.g) + std::abs(b - pix.b);
}
};

using Img = std::vector<std::vector<pixel>>;

struct Pair
{
    Pair(std::string left_name, std::string right_name){
        CImg<unsigned char> left_i(left_name.c_str());
        CImg<unsigned char> right_i(right_name.c_str());
        assert(left_i.height() == right_i.height() && left_i.width() == right_i.width());

        std::cout << "height "<<left_i.height() << " width "<<left_i.width()<<"\n";
        height = left_i.height() + std::max(0, -min_v_shift) + std::max(0, max_v_shift);
        width = left_i.width() + std::max(0, -min_h_shift) + std::max(0, max_h_shift);
        std::cout << "height "<<height << " width "<<width<<"\n";

        img_begin_v = std::max(0, -min_v_shift);
        img_end_v = height - std::max(0, max_v_shift);
        img_begin_h = std::max(0, -min_h_shift);
        img_end_h = width - std::max(0, max_h_shift);

        left.resize(height);
        for (auto& row : left){
            row.resize(width);
        }
        right.resize(height);
        for (auto& row : right){
            row.resize(width);
        }
        depth.resize(height);
        for (auto& row : depth){
            row.resize(width);
        }

        for (int i=0; i<height; i++){
            for (int j=0; j<width; j++){
                if(i<img_begin_v || i>img_end_v || j<img_begin_h || j>img_end_h){
                    left[i][j].r = std::numeric_limits<int>::max();
                    left[i][j].g = std::numeric_limits<int>::max();
                    left[i][j].b = std::numeric_limits<int>::max();

                    right[i][j].r = 0;
                    right[i][j].g = 0;
                    right[i][j].b = 0;
                }
                else {
                    left[i][j].r = (int) left_i.atXY(j-img_begin_h, i-img_begin_v, 0, 0);
                    left[i][j].g = (int) left_i.atXY(j-img_begin_h, i-img_begin_v, 0, 1);
                    left[i][j].b = (int) left_i.atXY(j-img_begin_h, i-img_begin_v, 0, 2);

                    right[i][j].r = (int) right_i.atXY(j-img_begin_h, i-img_begin_v, 0, 0);
                    right[i][j].g = (int) right_i.atXY(j-img_begin_h, i-img_begin_v, 0, 1);
                    right[i][j].b = (int) right_i.atXY(j-img_begin_h, i-img_begin_v, 0, 2);
                }
            }
        }
    }

    void save_img(Img& img){
        int start_v = std::max(0, -min_v_shift);
        int start_h = std::max(0, -min_h_shift);
        int end_v = height - std::max(0, max_v_shift);
        int end_h = width - std::max(0, max_h_shift);
        std::cout << "start_V "<< start_v << " start_h "<< start_h << " end_v " << end_v << " end_h "<<end_h<<"\n";
         CImg<unsigned char> image(end_h-start_h, end_v-start_v, 1, 3, 0);
         auto file = std::ofstream("result.txt");
         for(int i=start_h; i< end_h-start_h; i++){
             for(int j=start_v; j< end_v-start_v; j++){
                 img[j][i].r += min_v_shift;
                 img[j][i].g += min_h_shift;
                     image.atXY(i-start_h, j-start_v, 0, 0) = abs(img[j][i].r)*5.; //(img[j][i].r - min_h_shift) * (255. / (max_h_shift - min_h_shift));
                     image.atXY(i-start_h, j-start_v, 0, 1) = abs(img[j][i].g)*5.; //(img[j][i].g - min_v_shift) * (255. / (max_v_shift - min_v_shift));
                     image.atXY(i-start_h, j-start_v, 0, 2) = 0.;

                 file << img[j][i].r; //(img[j][i].r - min_h_shift) * (255. / (max_h_shift - min_h_shift)); vert
                 file << " ";
                 file << img[j][i].g; //(img[j][i].g - min_v_shift) * (255. / (max_v_shift - min_v_shift)); horiz
                 file << " ";
             }
             file << "\n";
         }
        CImgDisplay main_disp(image,"Click a point");
        while (!main_disp.is_closed()) {
            main_disp.wait();
        }
    }

    std::vector<pixel>& row(Img& img,int i){
        return img[i];
    } 

int height, width;
int img_begin_v, img_begin_h, img_end_v, img_end_h;
Img left;
Img right;
Img depth;
};

inline double diff(const pixel& l, const pixel& r)
{
    return ( std::abs(l.r-r.r) + std::abs(l.g-r.g) + std::abs(l.b-r.b) );
}

auto get_diff(const pixel& ref, const Pair& pair, int row, int col){
    std::vector<std::vector<double>> _diff (-min_v_shift + max_v_shift, std::vector<double>(-min_h_shift + max_h_shift));
    for (int i =min_v_shift; i < max_v_shift; i++)
    {
//#pragma omp simd
        for (int j =min_h_shift; j < max_h_shift; j++)
        {
                _diff[i-min_v_shift][j-min_h_shift] = diff(ref, pair.right[i+row][j+col]);
        }
    }
    return _diff;
}

auto get_row_diff(const Pair& pair, int row)
{
    std::vector<std::vector<std::vector<double>>> mat;
    for(int i=pair.img_begin_h; i<pair.img_end_h; ++i)
    {
        mat.push_back(get_diff(pair.left[row][i], pair, row, i));
    }
    return mat;
}

void write_row(Img& depth, std::vector<std::vector<double>>& diff, int row)
{
    for(int i=0; i < diff.size(); i++){
        depth[row][i].r = diff[i][0];
        depth[row][i].g = diff[i][1];
        depth[row][i].b = diff[i][2];
    }
}
