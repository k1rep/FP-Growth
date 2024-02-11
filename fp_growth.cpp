#include <fstream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <cmath>
#include <iostream>
#include <unordered_set>
#include "fp_tree.h"
using namespace std;

// 数据大小
int line = 0;

bool compare(ItemHead &a, ItemHead &b) {
    return a.count > b.count;
}

vector<string> read_file(const string& file_name) {
    vector<string> file;
    ifstream fin(file_name);
    try{
        string temp;
        // 按行读取文件
        // 8760 9735 11859 32868
        while(getline(fin, temp)){
            line++;
            file.push_back(temp);
        }
    }catch (ifstream::failure &e){
        cout << "Read file error:"<< e.what() << endl;
    }
    return file;
}

vector<ItemHead> get_item_head(vector<string> &file) {
    vector<ItemHead> head;
    // 对item计数
    unordered_map<string, int> count;

    // 第一次扫描数据库 对每个item进行计数
    for(const auto& it : file){
        istringstream iss(it);
        string item;
        while(iss >> item){
            count[item]++;
        }
    }

    int min_support = ceil(support * line);

    // 直接忽略小于最小支持度的项
    for(const auto& it : count){
        if(it.second >= min_support){
            ItemHead ih;
            ih.data = stoi(it.first);
            ih.count = it.second;
            ih.next = nullptr;
            head.push_back(ih);
        }
    }
    // 排序
    sort(head.begin(), head.end(), compare);

    // 生成项头表
    return head;
}

bool is_in(const string& temp, const string& str) {
    return temp.find(str) != string::npos;
}

vector<vector<int>> get_fp_file(const vector<string> &file, const vector<ItemHead> &head) {
    vector<vector<int>> fp_file;
    unordered_set<int> headData;
    for (const auto &h : head) {
        headData.insert(h.data);
    }

    for (const auto &i : file) {
        vector<int> fp_i;
        for (const auto &n : headData) {
            if (is_in(i, std::to_string(n))) {
                fp_i.push_back(n);
            }
        }
        if (!fp_i.empty()) {
            fp_file.push_back(std::move(fp_i));
        }
    }
    return fp_file;
}



void connect_list(FP_TreeNode* new_node, vector<ItemHead> &connect_list) {
    for (auto &m : connect_list) {
        if (m.data == new_node->data) {
            if (m.next == nullptr) {
                m.next = new_node;
            } else {
                FP_TreeNode* t1 = m.next;
                FP_TreeNode* t2 = m.next;
                while (t1) {
                    t2 = t1;
                    t1 = t1->next;
                }
                t2->next = new_node;
            }
            break; // 找到对应的item_head，退出循环
        }
    }
}


int c = 0;
FP_TreeNode* create_fp_tree(vector<vector<int>> &fp_file, vector<ItemHead> &head) {
    auto* root = new FP_TreeNode();
    auto* p = root;
    auto* q = root;
    int flag = 0;
    // 读取过滤和排序的原始数据
    for(int i=0;i<fp_file.size();i++) {
        p = root;
        int j = 0;
        while (j < fp_file[i].size()) {
            flag = 0;
            // 按照顺序插入到树中
            if (i == 0) {//第一条
                auto *newNode = new FP_TreeNode();
                c++;
                newNode->count = 1;
                newNode->data = fp_file[i][j];
                newNode->father = p;
                p->child = newNode;
                p = p->child;
                j++;
                for (auto &m: head) {
                    if (m.data == newNode->data) {
                        m.next = newNode;
                        break;
                    }
                }
            } else {
                // 如果有公共的祖先 在对应祖先节点加1
                p = p->child;
                while (p && j < fp_file[i].size()) {
                    if (p->data == fp_file[i][j]) {
                        p->count++;
                        q = p;//q->chilren=p;
                        p = p->child;
                        j++;
                        flag = 1;
                    } else {//
                        q = p;//q->brother=p;
                        p = p->brother;
                        flag = 2;
                    }
                }
                // 新节点出现 连接项表头到树新节点
                if (flag == 1) {
                    while (j < fp_file[i].size()) {
                        auto *newNode = new FP_TreeNode();
                        c++;
                        newNode->count = 1;
                        newNode->father = q;
                        q->child = newNode;
                        newNode->data = fp_file[i][j];
                        q = q->child;
                        j++;
                        //Linknext();
                        connect_list(newNode, head);
                    }

                } else if (flag == 2) {
                    auto *newNode = new FP_TreeNode();
                    c++;
                    newNode->count = 1;
                    newNode->data = fp_file[i][j];
                    q->brother = newNode;
                    newNode->father = q->father;
                    q = q->brother;
                    j++;
                    connect_list(newNode, head);
                    while (j < fp_file[i].size()) {
                        auto *newNode = new FP_TreeNode();
                        c++;
                        newNode->count = 1;
                        newNode->father = q;
                        q->child = newNode;
                        newNode->data = fp_file[i][j];
                        q = q->child;
                        j++;
                        //Linknext();
                        connect_list(newNode, head);
                    }
                }
            }
        }
    }
    return root;
}

vector<string> get_frequent_items(ItemHead &item_head, FP_TreeNode* &head) {
    FP_TreeNode *p = item_head.next;
    vector<string> rfile;
    while (p) {
        FP_TreeNode *q = p;
        vector<string> temp;
        // 确保不会遍历到根节点的父节点（即 nullptr）
        while (q != nullptr && q->father != nullptr && q->father != head) {
            q = q->father;
            stringstream ss;
            string x;
            // 直接使用 to_string 函数转换整数为字符串
            x = std::to_string(q->data);

            // 添加一个空格作为项之间的分隔符，如果需要
            temp.push_back(x + " ");
        }
        reverse(temp.begin(), temp.end());
        string s;

        for (auto &j: temp) {
            s += j;
        }
        for (int i = 0; i < p->count; i++) {
            if (!s.empty())
                rfile.push_back(s);
        }
        s.clear();
        p = p->next;
    }
    return rfile;
}

void get_result(vector<ItemHead> &item_head, FP_TreeNode* &head, string &base, vector<ResultNode> &result) {
    if(item_head.empty()) {
        return;
    }
    for (auto p = item_head.rbegin(); p != item_head.rend(); ++p) {
        string itemString = std::to_string(p->data);
        string newBase = base + " " + itemString;

        ResultNode temp;
        temp.data = newBase;
        temp.count = p->count;
        result.push_back(temp);

        // 产生条件模式基
        vector<string> conditionalPatternBase = get_frequent_items(*p, head);
        vector<ItemHead> newHeadList = get_item_head(conditionalPatternBase);
        vector<vector<int>> newFpFile = get_fp_file(conditionalPatternBase, newHeadList);

        FP_TreeNode* newTree = create_fp_tree(newFpFile, newHeadList);
        string s = base + " " + itemString;
        // 递归产生频繁项集结果
        get_result(newHeadList, newTree, s, result);
    }
}


void Print(const vector<ResultNode> &result) {
    ofstream outfile(OUT_FILE);
    if(!outfile.is_open()) {
        cout << "Open file error!" << endl;
        return;
    }

    for (const auto & p : result) {
        outfile << std::left << std::setw(20) << p.data << " " << "(" << p.count << ")" << endl;
    }
}


int main(){
    vector<ResultNode> result;
    vector<string> file = read_file(DATA_FILE);
    vector<ItemHead> headlist = get_item_head(file);
    vector<vector<int>> fp_file = get_fp_file(file,headlist);
    FP_TreeNode* head = create_fp_tree(fp_file,headlist);
    string base=" ";
    get_result(headlist,head,base,result);
    Print(result);
    cout<<result.size();
    return 0;
}

