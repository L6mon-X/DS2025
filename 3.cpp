#include <iostream>
#include <vector>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

// 计算柱状图中最大矩形面积（单调栈算法，时间复杂度O(n)）
int largestRectangleArea(vector<int>& heights) {
    stack<int> stk;  // 存储柱子索引索引的栈，维持高度递增的柱子索引
    stk.push(-1);    // 哨兵元素，方便处理边界情况
    int maxArea = 0;
    int n = heights.size();
    
    for (int i = 0; i < n; ++i) {
        // 当当前高度高度当前当前栈顶索引的柱子子高度时，计算以栈顶索引为高度的矩形面积
        while (stk.top() != -1 && heights[stk.top()] >= heights[i]) {
            int height = heights[stk.top()];  // 当前计算的矩形高度
            stk.pop();
            int width = i - stk.top() - 1;    // 矩形宽度 = 当前索引 - 新栈顶索引 - 1
            maxArea = max(maxArea, height * width);
        }
        stk.push(i);  // 当前索引入栈
    }
    
    // 处理栈中剩余的索引（对应右侧没有更低柱子的情况）
    while (stk.top() != -1) {
        int height = heights[stk.top()];
        stk.pop();
        int width = n - stk.top() - 1;  // 宽度为数组长度 - 新栈顶索引 - 1
        maxArea = max(maxArea, height * width);
    }
    
    return maxArea;
}

// 生成随机测试数据
vector<int> generateRandomHeights(int size) {
    vector<int> heights(size);
    for (int i = 0; i < size; ++i) {
        heights[i] = rand() % 10001;  // 随机高度：0 <= height <= 10^4
    }
    return heights;
}

// 测试函数：生成10组数据并验证
void testLargestRectangle() {
    srand(time(0));  // 初始化随机数种子
    cout << "=== 柱状图最大矩形面积测试 ===" << endl;
    
    for (int i = 0; i < 10; ++i) {
        // 随机生成数组长度：1 <= length <= 10^5
        int size = rand() % 100000 + 1;
        vector<int> heights = generateRandomHeights(size);
        
        // 输出测试信息（大规模数据仅显示基本信息）
        cout << "\n测试组 " << i + 1 << endl;
        cout << "数组长度: " << size << endl;
        
        // 小规模数据显示具体内容（便于验证）
        if (size <= 10) {
            cout << "高度数组: ";
            for (int h : heights) {
                cout << h << " ";
            }
            cout << endl;
        } else {
            cout << "高度数组: （大规模数据，省略显示）" << endl;
        }
        
        // 计算最大面积并计时
        clock_t start = clock();
        int maxArea = largestRectangleArea(heights);
        clock_t end = clock();
        double timeCost = (double)(end - start) / CLOCKS_PER_SEC;
        
        cout << "最大矩形面积: " << maxArea << endl;
        cout << "计算耗时: " << timeCost << " 秒" << endl;
    }
}

int main() {
    testLargestRectangle();
    return 0;
}