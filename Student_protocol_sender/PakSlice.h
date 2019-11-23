//
// Created by junk_chuan on 2019/10/31.
//

#ifndef NETWORK_PROJ_PAKSLICE_H
#define NETWORK_PROJ_PAKSLICE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SLICELEN 1000U

char **PakSlice(char * srcdata, unsigned int * datasize, unsigned int * piece, unsigned int * left)
{
    *piece = *datasize / SLICELEN; // 求商
    *left = *datasize % SLICELEN;  // 求余数
    if (*left)
        (*piece)++;                // 若存在余数，则需要的分组数=(商+1)

    char **SliceData = (char **)malloc(sizeof(char *) * (*piece)); // 根据分组数量动态分配空间
    for (unsigned int i = 0; i < *piece-1; ++i)
        // 先处理前(piece - 1)组数据(最后一组数据长度不一)
    {
        SliceData[i] = (char *)malloc(sizeof(char) * (SLICELEN + 3));
        SliceData[i][0] = (char)(i+1);    // 第一位存放当前分组序号
        SliceData[i][1] = (char)(*piece); // 第二位存放分组的总数

        for (int j = 0; j < 1000; ++j)
        {
            SliceData[i][j+2] = srcdata[i*SLICELEN + j];
        }
        SliceData[i][1002] = '\0'; // 每个分组最后加上结尾符
    }

    // 最后一组数据的处理
    (*left) += 2;
    SliceData[*piece-1] = (char *)malloc(sizeof(char) * (*left));
    SliceData[*piece-1][0] = (char)(*piece);
    SliceData[*piece-1][1] = (char)(*piece);
    for (int i = 0, j = 1000*(*piece-1); i < *left; ++i, ++j)
    {
        SliceData[*piece-1][i+2] = srcdata[j];
    }

    // 释放内存并且指针指向空
    free(srcdata);
    srcdata = NULL;
    return SliceData;

}

#endif //NETWORK_PROJ_PAKSLICE_H
