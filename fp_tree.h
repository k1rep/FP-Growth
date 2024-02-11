//
// Created on 2022/4/4.
//
#include<iostream>

#ifndef FP_GROWTH_FP_TREE_H
#define FP_GROWTH_FP_TREE_H

#endif //FP_GROWTH_FP_TREE_H

// 最小支持度
#define support 0.005

/*
 * FP-Tree
 */
typedef struct FP_TreeNode {
    int data;
    int count;
    // 孩子兄弟链表法
    struct FP_TreeNode *father;
    struct FP_TreeNode *child;
    struct FP_TreeNode *brother;
    struct FP_TreeNode *next;
    FP_TreeNode() : data(0), count(0), father(nullptr), child(nullptr), brother(nullptr), next(nullptr) {}
} FP_TreeNode;

/*
 * 项头表
 */
typedef struct item_head {
    int data;
    int count;
    // 指向fp-tree的指针
    FP_TreeNode *next;
} ItemHead;

/*
 * 频繁项集结果节点
 */
typedef struct result_node {
    std::string data;
    int count;
    result_node() : count(0) {}
} ResultNode;

/**
 * 比较项目头表各节点
 * @param a
 * @param b
 * @return
 */
bool compare(ItemHead &a, ItemHead &b);

/**
 * 读取数据集
 * @param file_name 文件名
 * @return 数据行向量
 */
std::vector<std::string> read_file(const std::string& file_name);

/**
 * 第一次扫描数据库，得到项头表
 * @param file 数据行向量
 * @return 项头表
 */
std::vector<ItemHead> get_item_head(std::vector<std::string> &file);

/**
 * 第二次扫描数据库，对树删除非频繁项，过滤和重排数据
 * @param file 数据行向量
 * @param item_head 项头表
 * @return
 */
std::vector<std::vector<int>> get_fp_file(const std::vector<std::string> &file, const std::vector<ItemHead> &item_head);

/**
 * 连接项头表和fp-tree的辅助函数
 * @param new_node 新节点
 * @param item_heads 项头表
 * @return
 */
void connect_list(FP_TreeNode* new_node, std::vector<ItemHead> &item_heads);

/**
 * 创建FP-Tree
 * @param fp_file 过滤和重排后的数据
 * @param item_head 项头表
 * @return
 */
FP_TreeNode* create_fp_tree(std::vector<std::vector<int>> &fp_file, std::vector<ItemHead> &item_head);

/**
 * 生成条件模式基
 * 从项头表开始，沿着树的路径回溯到根节点，得到条件模式基
 * @param item_head
 * @param head
 * @return
 */
std::vector<std::string> get_frequent_items(ItemHead &item_head, FP_TreeNode* &head);

/**
 * 打印结果
 */
void Print(const std::vector<ResultNode> &result);