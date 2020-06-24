#pragma once

#include <vector>
#include "CImg/CImg.hpp"
#include <string>
#include <limits>
#include <iostream>

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

struct Img
{
    Img(std::string name){
        CImg<unsigned char> image(name.c_str()); 
        height = image.height();
        width = image.width();
        img.resize(height);
        for (auto& row : img){
            row.resize(width);
        }
        for (int i=0; i<height; i++){
            for (int j=0; j<width; j++){
                img[i][j].r = (int)image.atXY(j, i, 0, 0);
                img[i][j].g = (int)image.atXY(j, i, 0, 1);
                img[i][j].b = (int)image.atXY(j, i, 0, 2);
            }
        }
    }

    Img(int _width, int _height)
    {
        width = _width;
        height = _height;

        img.resize(height);
        for (auto& row : img){
            row.resize(width);
        }
    }

    void save_img(){
         CImg<unsigned char> image(width, height, 1, 3, 0);
         for(int i=0; i< width; i++){
             for(int j=0; j< height; j++){
                     image.atXY(i, j, 0, 0) = (img[j][i].r - min_h_shift) * (255. / (max_h_shift - min_h_shift));
                     image.atXY(i, j, 0, 1) = (img[j][i].g - min_v_shift) * (255. / (max_v_shift - min_v_shift));
                     image.atXY(i, j, 0, 2) = img[j][i].b;
             }
         }
	image.save("rez.bmp");
        CImgDisplay main_disp(image,"Click a point");
        while (!main_disp.is_closed()) {
            main_disp.wait();
        }
    } 

    std::vector<pixel>& row(int i){
        return img[i];
    } 

int height, width;
std::vector<std::vector<pixel>> img;
};

double diff(pixel& l, pixel& r)
{
    return ( std::abs(l.r-r.r) + std::abs(l.g-r.g) + std::abs(l.b-r.b) );
}

auto get_diff(pixel& ref, Img& image, int row, int col){
    int height = image.height;
    int width = image.width;
    std::vector<std::vector<double>> _diff (-min_v_shift + max_v_shift, std::vector<double>(-min_h_shift + max_h_shift));
    for (int i =min_v_shift; i < max_v_shift; i++)
    {
        for (int j =min_h_shift; j < max_h_shift; j++)
        {
            if (i+row>=0 && i+row<=height && j+col>=0 && j+col<=width){
                _diff[i-min_v_shift][j-min_h_shift] = diff(ref, image.img[i+row][j+col]);
            } else {
                _diff[i-min_v_shift][j-min_h_shift] = std::numeric_limits<double>::max();
            }
        }
    }
    return _diff;
}

auto get_row_diff(Img& left, Img& right, int row)
{
    int height = left.height;
    int width = left.width;
    std::vector<std::vector<std::vector<double>>> mat;
    for(int i=0; i<width; ++i)
    {
        mat.push_back(get_diff(left.img[row][i], right, row, i));
    }
    return mat;
}

void write_row(Img& depth, std::vector<std::vector<double>>& diff, int row)
{
    int height = depth.height;
    int width = depth.width;
    for(int i=0; i< width; i++){
        depth.img[row][i].r = diff[i][0];
        depth.img[row][i].g = diff[i][1];
        depth.img[row][i].b = diff[i][2];
    }

}
