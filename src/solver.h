#pragma once

#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>
#include <omp.h>

using Lines = std::vector<std::vector<double>>;
using Line = std::vector<double>;

extern double alpha;
extern double betta;

inline double g_loss(int horisontal, int vertical, int h_stationar, int v_stationar)
{
    return (std::abs(vertical-v_stationar) + std::abs(horisontal - h_stationar)) * betta;
}

double find_min_for_vertex(const Lines& lines, int h_pos, int v_pos)
{
    double min_way = std::numeric_limits<double>::max();

    for (int v=0; v<lines.size(); ++v)
    {
        for (int h=0; h<lines[0].size(); ++h)
        {
            double tmp = alpha*lines[v][h] + g_loss(h, v, h_pos, v_pos);
            if(tmp<min_way){min_way=tmp;}
        }
    }
    return min_way;
}

void find_path(const Lines& lines_l, Lines& lines_r)
{
    for (size_t v=0; v<lines_l.size(); ++v)
    {
        for (size_t h=0; h<lines_l[0].size(); ++h)
        {
            double min = find_min_for_vertex(lines_l, h, v);
            lines_r[v][h] += min;
        }
    }
}

void solve(std::vector<Lines>& pic)
{
    for (size_t pix=1; pix<pic.size(); ++pix)
    {
        find_path(pic[pix-1], pic[pix]);
    }
}

void find_min(const Lines& pic, int& r_v, int& r_h){
    double _min = std::numeric_limits<double>::max();
    int _v=0,_h=0;
    for(int v=0; v< pic.size(); v++){
        for(int h=0; h< pic[0].size(); h++){
            double min = pic[v][h] + g_loss(h, v, r_h, r_v);
            if(min < _min) {_min = min; _v=v; _h=h;}
        }
    }
    r_v = _v; r_h = _h;
}

auto back_prop(const std::vector<Lines>& pic)
{
    int width = pic.size();
    std::vector<std::vector<double>> shifts(width, std::vector<double>(3));
    auto& l_col = pic[pic.size()-1];
    double _min = std::numeric_limits<double>::max();
    int _v=0,_h=0;
    for(int v=0; v< l_col.size(); v++){
        for(int h=0; h< l_col[0].size(); h++){
            double min = l_col[v][h];
            if(min < _min) {_min = min; _v=v; _h=h;}
        }
    }
    shifts[width-1][0] = _v;
    shifts[width-1][1] = _h;
    shifts[width-1][2] = 0;
    for(int pos = width-2; pos>=0; pos--){
        auto& l_col = pic[pos];
        find_min(l_col, _v, _h);
        shifts[pos][0] = _v;
        shifts[pos][1] = _h;
        shifts[pos][2] = 0;
    }
    return shifts;  
}
