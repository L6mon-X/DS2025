#include <iostream>
#include <cmath>
#include <ctime>
#include <algorithm>

using namespace std;

// 复数类定义
class Complex {
private:
    double real;  // 实部
    double imag;  // 虚部

public:
    // 构造函数
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}

    // 获取实部和虚部
    double getReal() const { return real; }
    double getImag() const { return imag; }

    // 计算复数的模
    double modulus() const {
        return sqrt(real * real + imag * imag);
    }

    // 重载相等运算符（实部和虚部均相同才相等）
    bool operator==(const Complex& other) const {
        return (real == other.real) && (imag == other.imag);
    }

    // 重载不等运算符
    bool operator!=(const Complex& other) const {
        return !(*this == other);
    }

    // 重载比较运算符（用于排序）
    // 先比较模，模相等则比较实部
    bool operator>(const Complex& other) const {
        if (modulus() != other.modulus()) {
            return modulus() > other.modulus();
        }
        return real > other.real;
    }

    bool operator<(const Complex& other) const {
        if (modulus() != other.modulus()) {
            return modulus() < other.modulus();
        }
        return real < other.real;
    }

    // 重载输出运算符
    friend ostream& operator<<(ostream& os, const Complex& c) {
        os << "(" << c.real << ", " << c.imag << ")";
        return os;
    }
};

// 向量模板类（实现动态数组功能）
template <typename T>
class Vector {
private:
    T* data;       // 数据存储
    int size;      // 当前元素个数
    int capacity;  // 容量

    // 扩容函数
    void resize(int newCapacity) {
        if (newCapacity <= capacity) return;
        T* newData = new T[newCapacity];
        for (int i = 0; i < size; ++i) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    // 构造函数
    Vector() : data(nullptr), size(0), capacity(0) {}

    // 析构函数
    ~Vector() {
        delete[] data;
    }

    // 获取大小
    int getSize() const { return size; }

    // 判断是否为空
    bool isEmpty() const { return size == 0; }

    // 访问元素
    T& operator[](int index) { return data[index]; }
    const T& operator[](int index) const { return data[index]; }

    // 尾插
    void push_back(const T& elem) {
        if (size == capacity) {
            resize(capacity == 0 ? 1 : capacity * 2);
        }
        data[size++] = elem;
    }

    // 插入元素
    void insert(int pos, const T& elem) {
        if (pos < 0 || pos > size) return;
        if (size == capacity) {
            resize(capacity == 0 ? 1 : capacity * 2);
        }
        for (int i = size; i > pos; --i) {
            data[i] = data[i - 1];
        }
        data[pos] = elem;
        size++;
    }

    // 删除元素
    void erase(int pos) {
        if (pos < 0 || pos >= size) return;
        for (int i = pos; i < size - 1; ++i) {
            data[i] = data[i + 1];
        }
        size--;
    }

    // 查找元素
    int find(const T& elem) const {
        for (int i = 0; i < size; ++i) {
            if (data[i] == elem) {
                return i;
            }
        }
        return -1;
    }

    // 唯一化（删除重复元素）
    int deduplicate() {
        int oldSize = size;
        int i = 1;
        while (i < size) {
            if (find(data[i]) != i) {
                erase(i);
            } else {
                i++;
            }
        }
        return oldSize - size;
    }

    // 清空向量
    void clear() { size = 0; }

    // 冒泡排序
    void bubbleSort() {
        bool sorted = false;
        for (int i = 0; i < size - 1 && !sorted; ++i) {
            sorted = true;
            for (int j = 0; j < size - 1 - i; ++j) {
                if (data[j] > data[j + 1]) {
                    swap(data[j], data[j + 1]);
                    sorted = false;
                }
            }
        }
    }
};

// 生成随机复数
Complex randomComplex(double min, double max) {
    double r = min + (max - min) * rand() / RAND_MAX;
    double i = min + (max - min) * rand() / RAND_MAX;
    return Complex(r, i);
}

// 归并排序辅助函数
template <typename T>
void merge(Vector<T>& vec, int left, int mid, int right) {
    int n1 = mid - left;
    int n2 = right - mid;
    
    T* L = new T[n1];
    T* R = new T[n2];
    
    for (int i = 0; i < n1; ++i)
        L[i] = vec[left + i];
    for (int j = 0; j < n2; ++j)
        R[j] = vec[mid + j];
    
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] < R[j]) {
            vec[k++] = L[i++];
        } else {
            vec[k++] = R[j++];
        }
    }
    
    while (i < n1)
        vec[k++] = L[i++];
    while (j < n2)
        vec[k++] = R[j++];
    
    delete[] L;
    delete[] R;
}

// 归并排序
template <typename T>
void mergeSort(Vector<T>& vec, int left, int right) {
    if (left < right - 1) {
        int mid = left + (right - left) / 2;
        mergeSort(vec, left, mid);
        mergeSort(vec, mid, right);
        merge(vec, left, mid, right);
    }
}

// 打印向量
template <typename T>
void printVector(const Vector<T>& vec, const string& msg = "") {
    if (!msg.empty()) {
        cout << msg << ": ";
    }
    cout << "[ ";
    for (int i = 0; i < vec.getSize(); ++i) {
        cout << vec[i] << " ";
    }
    cout << "]" << endl;
}

// 区间查找：查找模介于[m1, m2)的所有元素
Vector<Complex> findByModulusRange(const Vector<Complex>& sortedVec, double m1, double m2) {
    Vector<Complex> result;
    int n = sortedVec.getSize();
    
    // 找到第一个模 >= m1的元素
    int left = 0, right = n;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (sortedVec[mid].modulus() < m1) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    int start = left;
    
    // 找到第一个模 >= m2的元素
    left = 0;
    right = n;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (sortedVec[mid].modulus() < m2) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    int end = left;
    
    // 收集结果
    for (int i = start; i < end; ++i) {
        result.push_back(sortedVec[i]);
    }
    
    return result;
}

// 测试排序效率
void testSortingEfficiency() {
    const int size = 5000;  // 测试数据规模
    Vector<Complex> original;
    
    // 生成随机复数向量
    srand(time(0));
    for (int i = 0; i < size; ++i) {
        original.push_back(randomComplex(0, 100));
    }
    
    // 1. 测试顺序情况（已排序）
    Vector<Complex> sortedVec = original;
    mergeSort(sortedVec, 0, sortedVec.getSize());
    
    clock_t start = clock();
    sortedVec.bubbleSort();
    clock_t end = clock();
    double bubbleSorted = (double)(end - start) / CLOCKS_PER_SEC;
    
    start = clock();
    mergeSort(sortedVec, 0, sortedVec.getSize());
    end = clock();
    double mergeSorted = (double)(end - start) / CLOCKS_PER_SEC;
    
    // 2. 测试乱序情况
    Vector<Complex> randomVec = original;
    
    start = clock();
    randomVec.bubbleSort();
    end = clock();
    double bubbleRandom = (double)(end - start) / CLOCKS_PER_SEC;
    
    randomVec = original;  // 恢复乱序
    start = clock();
    mergeSort(randomVec, 0, randomVec.getSize());
    end = clock();
    double mergeRandom = (double)(end - start) / CLOCKS_PER_SEC;
    
    // 3. 测试逆序情况
    Vector<Complex> reversedVec = original;
    mergeSort(reversedVec, 0, reversedVec.getSize());
    // 反转向量得到逆序
    for (int i = 0; i < reversedVec.getSize() / 2; ++i) {
        swap(reversedVec[i], reversedVec[reversedVec.getSize() - 1 - i]);
    }
    
    start = clock();
    reversedVec.bubbleSort();
    end = clock();
    double bubbleReversed = (double)(end - start) / CLOCKS_PER_SEC;
    
    // 恢复逆序
    reversedVec = original;
    mergeSort(reversedVec, 0, reversedVec.getSize());
    for (int i = 0; i < reversedVec.getSize() / 2; ++i) {
        swap(reversedVec[i], reversedVec[reversedVec.getSize() - 1 - i]);
    }
    
    start = clock();
    mergeSort(reversedVec, 0, reversedVec.getSize());
    end = clock();
    double mergeReversed = (double)(end - start) / CLOCKS_PER_SEC;
    
    // 输出结果
    cout << "\n=== 排序效率对比（单位：秒）===" << endl;
    cout << "数据规模: " << size << "个元素" << endl;
    cout << "排序算法 | 顺序情况 | 乱序情况 | 逆序情况" << endl;
    cout << "-----------------------------------------" << endl;
    cout << "冒泡排序 | " << bubbleSorted << " | " << bubbleRandom << " | " << bubbleReversed << endl;
    cout << "归并排序 | " << mergeSorted << " | " << mergeRandom << " | " << mergeReversed << endl;
}

int main() {
    srand(time(0));  // 初始化随机数生成器

    // 1. 测试无序向量的基本操作
    cout << "=== 测试无序向量操作 ===" << endl;
    Vector<Complex> vec;
    
    // 生成包含重复项的随机复数向量
    for (int i = 0; i < 5; ++i) {
        vec.push_back(randomComplex(0, 10));
    }
    // 添加重复元素
    vec.push_back(vec[0]);
    vec.push_back(vec[2]);
    printVector(vec, "初始向量（含重复项）");
    
    // 置乱操作
    Vector<Complex> shuffled = vec;
    for (int i = shuffled.getSize() - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(shuffled[i], shuffled[j]);
    }
    printVector(shuffled, "置乱后向量");
    
    // 查找操作
    Complex target = shuffled[2];
    int pos = shuffled.find(target);
    cout << "查找 " << target << " 的位置: " << pos << endl;
    
    // 插入操作
    Complex newComp(10.5, 20.5);
    shuffled.insert(2, newComp);
    printVector(shuffled, "插入 " + to_string(newComp.getReal()) + "+" + 
               to_string(newComp.getImag()) + "i 后");
    
    // 删除操作
    shuffled.erase(3);
    printVector(shuffled, "删除索引3的元素后");
    
    // 唯一化操作
    int removed = shuffled.deduplicate();
    printVector(shuffled, "唯一化后（移除了" + to_string(removed) + "个重复元素）");
    
    // 2. 测试排序效率
    testSortingEfficiency();
    
    // 3. 测试区间查找
    cout << "\n=== 测试区间查找 ===" << endl;
    Vector<Complex> sortedVec;
    for (int i = 0; i < 20; ++i) {
        sortedVec.push_back(randomComplex(0, 10));
    }
    mergeSort(sortedVec, 0, sortedVec.getSize());
    printVector(sortedVec, "排序后的向量");
    
    double m1 = 3.0, m2 = 7.0;
    Vector<Complex> rangeResult = findByModulusRange(sortedVec, m1, m2);
    printVector(rangeResult, "模介于[" + to_string(m1) + ", " + to_string(m2) + ")的元素");
    
    // 验证结果
    cout << "验证结果：" << endl;
    for (int i = 0; i < rangeResult.getSize(); ++i) {
        double mod = rangeResult[i].modulus();
        cout << rangeResult[i] << " 的模为 " << mod << "，"
             << (mod >= m1 && mod < m2 ? "符合条件" : "不符合条件") << endl;
    }

    return 0;
}