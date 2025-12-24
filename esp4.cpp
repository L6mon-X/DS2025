#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <iomanip>
#include <cmath>

using namespace std;

// 边界框结构体：包含位置、大小、置信度
struct BoundingBox {
    float x1, y1;   // 左上角坐标
    float x2, y2;   // 右下角坐标
    float score;    // 置信度
};

// -------------------------- 排序算法实现 --------------------------
// 1. 快速排序（递归版，按置信度降序）
int partition(vector<BoundingBox>& arr, int low, int high) {
    float pivot = arr[high].score;
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j].score >= pivot) { // 降序排列
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(vector<BoundingBox>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// 2. 冒泡排序（按置信度降序）
void bubbleSort(vector<BoundingBox>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j].score < arr[j + 1].score) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// 3. 选择排序（按置信度降序）
void selectionSort(vector<BoundingBox>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        int maxIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j].score > arr[maxIdx].score) {
                maxIdx = j;
            }
        }
        swap(arr[i], arr[maxIdx]);
    }
}

// 4. 归并排序（按置信度降序）
void merge(vector<BoundingBox>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<BoundingBox> L(n1), R(n2);
    
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];
    
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].score >= R[j].score) { // 降序
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(vector<BoundingBox>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// -------------------------- 数据生成模块 --------------------------
// 随机分布数据生成（边界框位置、大小、置信度均随机，范围合理）
vector<BoundingBox> generateRandomData(int size) {
    vector<BoundingBox> data;
    srand(time(0)); // 随机种子
    for (int i = 0; i < size; i++) {
        BoundingBox box;
        // 位置：x1 ∈ [0, 800), y1 ∈ [0, 600)（模拟800x600图像）
        box.x1 = rand() % 800;
        box.y1 = rand() % 600;
        // 大小：宽度 ∈ [20, 100], 高度 ∈ [20, 100]
        float w = 20 + rand() % 81;
        float h = 20 + rand() % 81;
        box.x2 = box.x1 + w;
        box.y2 = box.y1 + h;
        // 置信度 ∈ [0.0, 1.0]
        box.score = (rand() % 1001) / 1000.0f;
        data.push_back(box);
    }
    return data;
}

// 聚集分布数据生成（边界框集中在图像中心区域，置信度呈正态分布）
vector<BoundingBox> generateClusteredData(int size) {
    vector<BoundingBox> data;
    srand(time(0));
    // 聚集中心：(400, 300)（800x600图像中心）
    const float centerX = 400.0f;
    const float centerY = 300.0f;
    for (int i = 0; i < size; i++) {
        BoundingBox box;
        // 位置：围绕中心±50范围内波动（聚集特性）
        box.x1 = centerX - 50 + rand() % 101;
        box.y1 = centerY - 50 + rand() % 101;
        // 大小：与随机分布一致
        float w = 20 + rand() % 81;
        float h = 20 + rand() % 81;
        box.x2 = box.x1 + w;
        box.y2 = box.y1 + h;
        // 置信度：正态分布（均值0.7，标准差0.15），截断到[0.0, 1.0]
        float mu = 0.7f, sigma = 0.15f;
        float u1 = (rand() % 1001) / 1000.0f;
        float u2 = (rand() % 1001) / 1000.0f;
        float z = sqrt(-2.0f * log(u1)) * cos(2.0f * M_PI * u2);
        float score = mu + sigma * z;
        box.score = max(0.0f, min(1.0f, score));
        data.push_back(box);
    }
    return data;
}

// -------------------------- NMS算法实现 --------------------------
// 计算两个边界框的交并比（IoU）
float calculateIoU(const BoundingBox& a, const BoundingBox& b) {
    float interX1 = max(a.x1, b.x1);
    float interY1 = max(a.y1, b.y1);
    float interX2 = min(a.x2, b.x2);
    float interY2 = min(a.y2, b.y2);
    
    // 计算交集面积
    float interArea = max(0.0f, interX2 - interX1) * max(0.0f, interY2 - interY1);
    if (interArea <= 0) return 0.0f;
    
    // 计算并集面积
    float areaA = (a.x2 - a.x1) * (a.y2 - a.y1);
    float areaB = (b.x2 - b.x1) * (b.y2 - b.y1);
    float unionArea = areaA + areaB - interArea;
    
    return interArea / unionArea;
}

// 基础NMS算法（输入排序后的边界框，IoU阈值默认0.5）
vector<BoundingBox> nms(const vector<BoundingBox>& sortedBoxes, float iouThreshold = 0.5f) {
    vector<BoundingBox> result;
    vector<bool> suppressed(sortedBoxes.size(), false);
    
    for (int i = 0; i < sortedBoxes.size(); i++) {
        if (suppressed[i]) continue;
        // 保留当前置信度最高的框
        result.push_back(sortedBoxes[i]);
        // 抑制与当前框IoU超过阈值的框
        for (int j = i + 1; j < sortedBoxes.size(); j++) {
            if (suppressed[j]) continue;
            float iou = calculateIoU(sortedBoxes[i], sortedBoxes[j]);
            if (iou >= iouThreshold) {
                suppressed[j] = true;
            }
        }
    }
    return result;
}

// -------------------------- 性能测试模块 --------------------------
// 测试单个排序算法的运行时间（返回毫秒数）
double testSortPerformance(void (*sortFunc)(vector<BoundingBox>&), vector<BoundingBox> data) {
    clock_t start = clock();
    sortFunc(data); // 调用排序函数（传值避免修改原数据）
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC * 1000; // 转换为毫秒
}

// 测试归并排序（需左右边界参数）
double testMergeSortPerformance(vector<BoundingBox> data) {
    clock_t start = clock();
    mergeSort(data, 0, data.size() - 1);
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC * 1000;
}

// 测试快速排序（需左右边界参数）
double testQuickSortPerformance(vector<BoundingBox> data) {
    clock_t start = clock();
    quickSort(data, 0, data.size() - 1);
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC * 1000;
}

// 完整实验测试（不同数据规模、不同分布）
void runExperiment() {
    // 测试数据规模：100, 1000, 5000, 10000
    vector<int> sizes = {100, 1000, 5000, 10000};
    // 测试数据分布：随机分布、聚集分布
    vector<string> distributions = {"随机分布", "聚集分布"};
    // 排序算法名称
    vector<string> sortNames = {"快速排序", "归并排序", "冒泡排序", "选择排序"};
    
    cout << "==================================== 排序算法性能测试 ====================================" << endl;
    cout << setw(10) << "数据规模" << setw(12) << "数据分布" << setw(12) << sortNames[0] << setw(12) << sortNames[1] 
         << setw(12) << sortNames[2] << setw(12) << sortNames[3] << " (单位：ms)" << endl;
    cout << "----------------------------------------------------------------------------------------" << endl;
    
    for (int size : sizes) {
        for (int distIdx = 0; distIdx < 2; distIdx++) {
            // 生成对应分布的数据
            vector<BoundingBox> data;
            if (distIdx == 0) {
                data = generateRandomData(size);
            } else {
                data = generateClusteredData(size);
            }
            
            // 测试四种排序算法的运行时间
            double quickTime = testQuickSortPerformance(data);
            double mergeTime = testMergeSortPerformance(data);
            double bubbleTime = testSortPerformance(bubbleSort, data);
            double selectTime = testSortPerformance(selectionSort, data);
            
            // 输出结果（保留2位小数）
            cout << setw(10) << size << setw(12) << distributions[distIdx] 
                 << setw(12) << fixed << setprecision(2) << quickTime
                 << setw(12) << fixed << setprecision(2) << mergeTime
                 << setw(12) << fixed << setprecision(2) << bubbleTime
                 << setw(12) << fixed << setprecision(2) << selectTime << endl;
        }
    }
    
    // 测试NMS算法（以10000个随机分布数据为例）
    cout << "\n==================================== NMS算法测试 ====================================" << endl;
    vector<BoundingBox> nmsData = generateRandomData(10000);
    quickSort(nmsData, 0, nmsData.size() - 1); // NMS前先排序
    
    clock_t nmsStart = clock();
    vector<BoundingBox> nmsResult = nms(nmsData);
    clock_t nmsEnd = clock();
    double nmsTime = (double)(nmsEnd - nmsStart) / CLOCKS_PER_SEC * 1000;
    
    cout << "NMS输入边界框数量：" << nmsData.size() << endl;
    cout << "NMS输出边界框数量：" << nmsResult.size() << endl;
    cout << "NMS算法运行时间：" << fixed << setprecision(2) << nmsTime << " ms" << endl;
}

int main() {
    runExperiment();
    return 0;
}