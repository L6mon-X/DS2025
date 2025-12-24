#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

// 先定义Rank类型（全局或类外）
typedef int Rank;

// 位图类（BitMap）- 修复后
class Bitmap {
private:
    unsigned char* M;
    Rank N;  // 字节数（成员变量）
    Rank _sz; // 有效位的个数（成员变量）

    // 扩容操作：当访问的位超出当前容量时调用
    void expand(Rank k) {
        if (k < 8 * N) return; // 未出界，无需扩容
        
        Rank oldN = N;
        unsigned char* oldM = M;
        N = 2 * ((k + 7) / 8); // 重新计算扩容后的字节数
        M = new unsigned char[N](); // 初始化新空间为0
        memcpy(M, oldM, oldN); // 复制原数据
        delete[] oldM; // 释放原空间
    }

public:
    // 构造函数：指定初始容量（默认8位）
    Bitmap(Rank n = 8) : N((n + 7) / 8), _sz(0) {
        M = new unsigned char[N](); // 初始化内存
    }

    // 构造函数：从文件读取位图
    Bitmap(const char* file, Rank n = 8) : N((n + 7) / 8), _sz(0) {
        M = new unsigned char[N](); // 初始化内存
        FILE* fp = fopen(file, "rb"); // 二进制读
        if (fp) {
            fread(M, sizeof(unsigned char), N, fp);
            fclose(fp);
        }
        // 重新计算有效位个数
        _sz = 0;
        for (Rank k = 0; k < n; k++) {
            if (test(k)) _sz++;
        }
    }

    // 析构函数
    ~Bitmap() {
        delete[] M;
        M = nullptr;
        N = 0;
        _sz = 0;
    }

    // 初始化位图
    void init(Rank n) {
        N = (n + 7) / 8; // 计算需要的字节数
        M = new unsigned char[N](); // 初始化为0
        _sz = 0;
    }

    // 返回有效位的个数
    Rank size() const {
        return _sz;
    }

    // 设置第k位为1
    void set(Rank k) {
        expand(k);
        M[k >> 3] |= (0x80 >> (k & 0x07)); // k>>3 = k/8，k&0x07 = k%8
        _sz++;
    }

    // 清除第k位（设为0）
    void clear(Rank k) {
        expand(k);
        if (test(k)) { // 只有当前位为1时才减少计数
            M[k >> 3] &= ~(0x80 >> (k & 0x07));
            _sz--;
        }
    }

    // 测试第k位是否为1
    bool test(Rank k) const {
        if (k >= 8 * N) return false; // 超出范围返回false
        return (M[k >> 3] & (0x80 >> (k & 0x07))) != 0;
    }

    // 将位图导出到文件
    void dump(const char* file) const {
        FILE* fp = fopen(file, "wb"); // 二进制写
        if (fp) {
            fwrite(M, sizeof(unsigned char), N, fp);
            fclose(fp);
        }
    }

    // 将前n位转换为字符串（0/1序列）
    char* bits2string(Rank n) {
        expand(n - 1); // 确保访问范围有效
        char* s = new char[n + 1];
        s[n] = '\0';
        for (Rank i = 0; i < n; i++) {
            s[i] = test(i) ? '1' : '0';
        }
        return s;
    }
};

// 二叉树节点类（用于构建Huffman树）
template <typename T>
struct BinNode {
    T data;          // 节点数据（Huffman树中存储字符及其频率）
    BinNode<T>* left; // 左子节点
    BinNode<T>* right;// 右子节点

    // 构造函数
    BinNode(T d = T(), BinNode<T>* l = nullptr, BinNode<T>* r = nullptr)
        : data(d), left(l), right(r) {}
};

// 二叉树类（BinTree）
template <typename T>
class BinTree {
protected:
    BinNode<T>* root; // 根节点

    // 递归销毁二叉树
    void destroy(BinNode<T>* node) {
        if (node) {
            destroy(node->left);
            destroy(node->right);
            delete node;
        }
    }

public:
    // 构造函数
    BinTree() : root(nullptr) {}

    // 析构函数
    ~BinTree() {
        destroy(root);
    }

    // 获取根节点
    BinNode<T>* getRoot() const {
        return root;
    }

    // 设置根节点
    void setRoot(BinNode<T>* node) {
        root = node;
    }

    // 判断是否为空树
    bool isEmpty() const {
        return root == nullptr;
    }
};

// Huffman树节点的数据类型（字符+频率）
struct HuffNodeData {
    char ch;      // 字符（'#'表示合并节点）
    int frequency;// 频率
    Bitmap code;  // 该字符对应的Huffman编码（位图存储）

    // 构造函数
    HuffNodeData(char c = '#', int freq = 0) : ch(c), frequency(freq) {}

    // 重载比较运算符（用于优先队列）
    bool operator<(const HuffNodeData& other) const {
        return frequency > other.frequency; // 小顶堆：频率小的优先
    }
};

// Huffman树类（继承自BinTree）
class HuffTree : public BinTree<HuffNodeData> {
private:
    // 统计《I Have a Dream》原文中26个字母的频率
    void countFrequencies(const string& text, vector<int>& freq) {
        freq.assign(26, 0); // 初始化26个字母的频率为0
        for (char c : text) {
            if (isalpha(c)) { // 只处理字母
                c = tolower(c); // 不区分大小写
                freq[c - 'a']++;
            }
        }
    }

    // 递归生成Huffman编码（从根节点遍历到叶子节点）
    // 递归生成Huffman编码（从根节点遍历到叶子节点）
    void generateCodes(BinNode<HuffNodeData>* node, Bitmap& currentCode) {
        if (!node) return;

        // 如果是叶子节点（存储字母），保存编码
        if (!node->left && !node->right) {
            node->data.code = currentCode;
            return;
        }

        // 左子树：添加0
        currentCode.set(currentCode.size()); // 扩展一位
        currentCode.clear(currentCode.size() - 1); // 设为0
        generateCodes(node->left, currentCode);
        currentCode.clear(currentCode.size() - 1); // 回溯（清除最后一位）

        // 右子树：添加1
        currentCode.set(currentCode.size() - 1); // 设为1
        generateCodes(node->right, currentCode);
        currentCode.clear(currentCode.size() - 1); // 回溯（清除最后一位）
    }

public:
    // 构建Huffman树
    void build(const string& text) {
        vector<int> freq;
        countFrequencies(text, freq); // 统计频率

        // 优先队列（小顶堆）：存储Huffman树节点
        priority_queue<BinNode<HuffNodeData>*> pq;

        // 1. 创建叶子节点（只处理频率大于0的字母）
        for (int i = 0; i < 26; i++) {
            if (freq[i] > 0) {
                BinNode<HuffNodeData>* leaf = new BinNode<HuffNodeData>(
                    HuffNodeData('a' + i, freq[i])
                );
                pq.push(leaf);
            }
        }

        // 2. 构建Huffman树
        while (pq.size() > 1) {
            // 取出频率最小的两个节点
            BinNode<HuffNodeData>* left = pq.top(); pq.pop();
            BinNode<HuffNodeData>* right = pq.top(); pq.pop();

            // 创建合并节点（字符为'#'，频率为两个节点之和）
            BinNode<HuffNodeData>* parent = new BinNode<HuffNodeData>(
                HuffNodeData('#', left->data.frequency + right->data.frequency),
                left, right
            );

            pq.push(parent);
        }

        // 3. 设置根节点
        if (!pq.empty()) {
            setRoot(pq.top());
            // 生成Huffman编码
            Bitmap currentCode;
            generateCodes(getRoot(), currentCode);
        }
    }

    // 根据字符获取对应的Huffman编码（返回字符串形式）
    char* getCode(char ch) {
        if (!isalpha(ch)) return nullptr;
        ch = tolower(ch);

        // 递归查找字符对应的节点
        BinNode<HuffNodeData>* node = findNode(getRoot(), ch);
        if (!node) return nullptr;

        // 将位图编码转换为字符串
        return node->data.code.bits2string(node->data.code.size());
    }

private:
    // 递归查找存储指定字符的节点
    BinNode<HuffNodeData>* findNode(BinNode<HuffNodeData>* node, char ch) {
        if (!node) return nullptr;

        // 找到叶子节点且字符匹配
        if (!node->left && !node->right && node->data.ch == ch) {
            return node;
        }

        // 递归查找左右子树
        BinNode<HuffNodeData>* leftResult = findNode(node->left, ch);
        if (leftResult) return leftResult;
        return findNode(node->right, ch);
    }
};

// 测试用例：《I Have a Dream》演讲片段（用于统计频率）
const string I_HAVE_A_DREAM = R"(
I have a dream that one day this nation will rise up and live out the true meaning of its creed:
"We hold these truths to be self-evident, that all men are created equal."
I have a dream that one day on the red hills of Georgia, the sons of former slaves and the sons of former slave owners will be able to sit down together at the table of brotherhood.
I have a dream that one day even the state of Mississippi, a state sweltering with the heat of injustice, sweltering with the heat of oppression, will be transformed into an oasis of freedom and justice.
I have a dream that my four little children will one day live in a nation where they will not be judged by the color of their skin but by the content of their character.
I have a dream today!
)";

// 对单词进行Huffman编码
string encodeWord(const string& word, HuffTree& huffTree) {
    string encoded;
    for (char c : word) {
        if (!isalpha(c)) continue; // 只编码字母
        char* code = huffTree.getCode(c);
        if (code) {
            encoded += code;
            encoded += " "; // 编码之间用空格分隔
            delete[] code; // 释放内存
        }
    }
    return encoded;
}

int main() {
    // 1. 构建Huffman树
    HuffTree huffTree;
    huffTree.build(I_HAVE_A_DREAM);

    // 2. 测试编码功能
    vector<string> testWords = {"dream", "freedom", "justice", "equal", "character"};
    
    cout << "Huffman编码结果：" << endl;
    cout << "==================" << endl;
    for (const string& word : testWords) {
        string encoded = encodeWord(word, huffTree);
        cout << word << ": " << encoded << endl;
    }

    // 3. 单独展示每个字母的编码
    cout << "\n字母编码对照表：" << endl;
    cout << "==================" << endl;
    for (char c = 'a'; c <= 'z'; c++) {
        char* code = huffTree.getCode(c);
        if (code) {
            cout << c << ": " << code << endl;
            delete[] code;
        }
    }

    return 0;

}
