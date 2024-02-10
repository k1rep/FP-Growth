#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include <cmath>
#include <iostream>
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
    fin.exceptions(ifstream::failbit | ifstream::badbit);
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
    map<string, int> count;

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

vector<vector<int>> get_fp_file(vector<string> &file, vector<ItemHead> &head) {
    vector<vector<int>> fp_file;
    vector<int> fp_i;
    // 第二次扫描数据库，删除非频繁项进行过滤，根据项头表重排数据
    for (auto &i: file) {
        // 已经排过序的项头表
        for (auto &k: head) {
            string s;
            int n = k.data;
            stringstream ss;
            ss << n;
            ss >> s;
            if (is_in(i, s)) {
                fp_i.push_back(n);
            }
        }
        if (!fp_i.empty()) {
            fp_file.push_back(fp_i);
            fp_i.clear();
        }
    }
    file.clear();
    // 生成删除非频繁项和降序排列的数据集
    return fp_file;
}


void connect_list(FP_TreeNode* new_node, vector<ItemHead> &connect_list) {
    for(auto & m : connect_list) {
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
            break;
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
        while (q->father != head) {
            q = q->father;
            stringstream ss;
            string x;
            int n;
            n = q->data;

            ss << n;
            ss >> x;

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


vector<ResultNode> result;
void get_result(vector<ItemHead> &item_head, FP_TreeNode* &head, string &base ,vector<ResultNode> &result) {
    if(item_head.empty()) return;
    //遍历项头表依次挖掘，找到每项的条件模式基
    for(auto p = item_head.rbegin(); p != item_head.rend(); p++) {
        ResultNode temp;
        int n;
        n = p->data;//int to string
        stringstream ss;
        string x;
        ss << n;
        ss >> x;
        string xx = base.append(" ").append(x) ;
        temp.data = xx;
        temp.count = p->count;
        result.push_back(temp);
        // 开始递归
        // 产生条件模式基
        vector<string> file1 = get_frequent_items(*p, head);
        vector<ItemHead> headlist1 = get_item_head(file1);//getL1
        vector<vector<int> > rfile1 = get_fp_file(file1, headlist1);

        FP_TreeNode *Tree = create_fp_tree(rfile1, headlist1);

        string s = base.append(" ").append(x);
        //产生频繁项集结果
        get_result(headlist1, Tree, s, result);
    }
}

void Print() {
    ofstream outfile;
    outfile.open("./result.txt");
    for (const auto & p : result) {
        outfile << p.data << " " << "(" << p.count << ")" << endl;
    }
    outfile.close();
}

const string filename = "./chainstoreFIM.txt";
//const string filename = "./OnlineRetailZZ.txt";
int main(){
    vector<string> file = read_file(filename);
    vector<ItemHead> headlist = get_item_head(file);
    vector<vector<int>> fp_file = get_fp_file(file,headlist);
    FP_TreeNode* head = create_fp_tree(fp_file,headlist);
    string base=" ";
    get_result(headlist,head,base,result);
    Print();
    cout<<result.size();
    return 0;
}

