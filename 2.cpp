#include <iostream>
#include <string>
#include <cctype>
#include <cmath>
#include <stdexcept>

using namespace std;

// 栈数据结构实现
template <typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {}
    };
    Node* topNode;  // 栈顶指针
    int size;       // 栈大小

public:
    // 构造函数
    Stack() : topNode(nullptr), size(0) {}
    
    // 析构函数
    ~Stack() {
        while (!isEmpty()) {
            pop();
        }
    }
    
    // 入栈操作
    void push(const T& val) {
        Node* newNode = new Node(val);
        newNode->next = topNode;
        topNode = newNode;
        size++;
    }
    
    // 出栈操作
    void pop() {
        if (isEmpty()) {
            throw runtime_error("栈为空，无法执行出栈操作");
        }
        Node* temp = topNode;
        topNode = topNode->next;
        delete temp;
        size--;
    }
    
    // 获取栈顶元素
    T top() const {
        if (isEmpty()) {
            throw runtime_error("栈为空，无法获取栈顶元素");
        }
        return topNode->data;
    }
    
    // 判断栈是否为空
    bool isEmpty() const {
        return topNode == nullptr;
    }
    
    // 获取栈大小
    int getSize() const {
        return size;
    }
};

// 运算符优先级判断（参考书上代码4.6的优先级表）
int precedence(char op) {
    switch (op) {
        case '(': return 0;
        case ')': return 1;
        case '+': case '-': return 2;
        case '*': case '/': return 3;
        case '^': return 4;  // 幂运算
        default: return -1;  // 无效运算符
    }
}

// 执行二元运算
double calculate(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (b == 0) throw runtime_error("除零错误");
            return a / b;
        case '^': return pow(a, b);  // 幂运算
        default: throw runtime_error("未知运算符: " + string(1, op));
    }
}

// 字符串计算器主函数
double evaluateExpression(const string& expr) {
    Stack<double> numStack;  // 存储数字的栈
    Stack<char> opStack;     // 存储运算符的栈
    int n = expr.length();
    int i = 0;
    
    while (i < n) {
        // 跳过空格
        if (isspace(expr[i])) {
            i++;
            continue;
        }
        
        // 处理数字（包括整数和小数）
        if (isdigit(expr[i]) || expr[i] == '.') {
            double num = 0.0;
            // 处理整数部分
            while (i < n && isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            // 处理小数部分
            if (i < n && expr[i] == '.') {
                i++;
                double frac = 0.1;
                while (i < n && isdigit(expr[i])) {
                    num += (expr[i] - '0') * frac;
                    frac *= 0.1;
                    i++;
                }
            }
            numStack.push(num);
        }
        // 处理左括号
        else if (expr[i] == '(') {
            opStack.push(expr[i]);
            i++;
        }
        // 处理右括号
        else if (expr[i] == ')') {
            // 计算括号内的表达式
            while (!opStack.isEmpty() && opStack.top() != '(') {
                char op = opStack.top();
                opStack.pop();
                
                if (numStack.getSize() < 2) {
                    throw runtime_error("表达式格式错误（括号内）");
                }
                
                double b = numStack.top(); numStack.pop();
                double a = numStack.top(); numStack.pop();
                numStack.push(calculate(a, b, op));
            }
            
            if (opStack.isEmpty()) {
                throw runtime_error("括号不匹配（缺少左括号）");
            }
            
            opStack.pop();  // 弹出左括号
            i++;
        }
        // 处理运算符
        else if (precedence(expr[i]) != -1) {
            // 处理负号（表达式开头或左括号后的减号）
            if (expr[i] == '-' && (i == 0 || expr[i-1] == '(' || precedence(expr[i-1]) != -1)) {
                numStack.push(0);  // 视为 0 - 数字
            }
            
            // 处理运算符优先级：弹出优先级更高或相等的运算符并计算
            while (!opStack.isEmpty() && opStack.top() != '(' && 
                  precedence(opStack.top()) >= precedence(expr[i])) {
                char op = opStack.top();
                opStack.pop();
                
                if (numStack.getSize() < 2) {
                    throw runtime_error("表达式格式错误（运算符）");
                }
                
                double b = numStack.top(); numStack.pop();
                double a = numStack.top(); numStack.pop();
                numStack.push(calculate(a, b, op));
            }
            
            opStack.push(expr[i]);
            i++;
        }
        // 无效字符
        else {
            throw runtime_error("无效字符: " + string(1, expr[i]));
        }
    }
    
    // 处理剩余的运算符
    while (!opStack.isEmpty()) {
        char op = opStack.top();
        opStack.pop();
        
        if (op == '(') {
            throw runtime_error("括号不匹配（缺少右括号）");
        }
        
        if (numStack.getSize() < 2) {
            throw runtime_error("表达式格式错误（结尾）");
        }
        
        double b = numStack.top(); numStack.pop();
        double a = numStack.top(); numStack.pop();
        numStack.push(calculate(a, b, op));
    }
    
    if (numStack.getSize() != 1) {
        throw runtime_error("表达式格式错误（结果数量异常）");
    }
    
    return numStack.top();
}

// 测试案例
void testCalculator() {
    // 有效表达式测试
    cout << "=== 有效表达式测试 ===" << endl;
    string validExpressions[] = {
        "3 + 4 * 2",                  // 11
        "(3 + 4) * 2",                // 14
        "10 / (2 + 3)",               // 2
        "2 ^ 3 + 5",                  // 13
        "10 - 2 * 3",                 // 4
        "3.5 + 2.5 * 2",              // 8.5
        "((10 + 5) / 3) * 2",         // 10
        "2 + 3 * (4 - 1)",            // 11
        "100 / 2 - 30",               // 20
        "5 + (3 * 2 ^ 2)"             // 17
    };
    
    for (const string& expr : validExpressions) {
        try {
            double result = evaluateExpression(expr);
            cout << expr << " = " << result << endl;
        } catch (const exception& e) {
            cout << expr << " 计算错误: " << e.what() << endl;
        }
    }
    
    // 无效表达式测试
    cout << "\n=== 无效表达式测试 ===" << endl;
    string invalidExpressions[] = {
        "3 + * 4",            // 运算符错误
        "10 / (5 - 5)",       // 除零错误
        "3 * (4 + 5",         // 括号不匹配
        "2 3 + 4",            // 数字格式错误
        "5 + ) 3 ( * 2"       // 括号顺序错误
    };
    
    for (const string& expr : invalidExpressions) {
        try {
            double result = evaluateExpression(expr);
            cout << expr << " = " << result << endl;
        } catch (const exception& e) {
            cout << expr << " 无效: " << e.what() << endl;
        }
    }
}

int main() {
    testCalculator();
    
    // 交互式计算
    cout << "\n=== 交互式计算器 ===" << endl;
    cout << "请输入表达式（支持 +, -, *, /, ^, 括号），输入 q 退出：" << endl;
    string expr;
    while (getline(cin, expr)) {
        if (expr == "q" || expr == "Q") {
            break;
        }
        try {
            double result = evaluateExpression(expr);
            cout << "结果: " << result << endl;
        } catch (const exception& e) {
            cout << "错误: " << e.what() << endl;
        }
        cout << "请输入下一个表达式：" << endl;
    }
    
    return 0;
}