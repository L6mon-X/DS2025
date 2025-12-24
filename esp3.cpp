#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <climits>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <tuple>
using namespace std;

// ============================== 图数据结构类 ==============================
class Graph {
public:
    map<char, int> vertexToIdx;
    vector<char> idxToVertex;
    vector<vector<int>> adjMatrix;
    bool isDirected;

    Graph(const vector<char>& vertices, bool directed = false) {
        this->isDirected = directed;
        int n = vertices.size();
        for (int i = 0; i < n; ++i) {
            vertexToIdx[vertices[i]] = i;
            idxToVertex.push_back(vertices[i]);
        }
        adjMatrix.resize(n, vector<int>(n, 0));
    }

    void addEdge(char v1, char v2, int weight) {
        int i = vertexToIdx[v1];
        int j = vertexToIdx[v2];
        adjMatrix[i][j] = weight;
        if (!isDirected) {
            adjMatrix[j][i] = weight;
        }
    }

    void printAdjMatrix() {
        int n = idxToVertex.size();
        cout << "邻接矩阵（行/列：";
        for (char v : idxToVertex) cout << v << " ";
        cout << "）：\n";
        for (int i = 0; i < n; ++i) {
            cout << idxToVertex[i] << ":\t";
            for (int j = 0; j < n; ++j) {
                cout << adjMatrix[i][j] << "\t";
            }
            cout << "\n";
        }
    }

    vector<pair<char, int>> getAdjacent(char v) {
        vector<pair<char, int>> adj;
        int i = vertexToIdx[v];
        for (int j = 0; j < adjMatrix.size(); ++j) {
            if (adjMatrix[i][j] > 0) {
                adj.emplace_back(idxToVertex[j], adjMatrix[i][j]);
            }
        }
        return adj;
    }
};

// ============================== 任务2：BFS 和 DFS 算法 ==============================
vector<char> BFS(Graph& graph, char start) {
    vector<char> result;
    set<char> visited;
    queue<char> q;

    q.push(start);
    visited.insert(start);

    while (!q.empty()) {
        char curr = q.front();
        q.pop();
        result.push_back(curr);

        vector<pair<char, int>> adj = graph.getAdjacent(curr);
        sort(adj.begin(), adj.end(), [](const pair<char, int>& a, const pair<char, int>& b) {
            return a.first < b.first;
        });

        for (auto& p : adj) {
            char nextV = p.first;
            if (visited.find(nextV) == visited.end()) {
                visited.insert(nextV);
                q.push(nextV);
            }
        }
    }
    return result;
}

void DFSRecursive(Graph& graph, char curr, set<char>& visited, vector<char>& result) {
    visited.insert(curr);
    result.push_back(curr);

    vector<pair<char, int>> adj = graph.getAdjacent(curr);
    sort(adj.begin(), adj.end(), [](const pair<char, int>& a, const pair<char, int>& b) {
        return a.first < b.first;
    });

    for (auto& p : adj) {
        char nextV = p.first;
        if (visited.find(nextV) == visited.end()) {
            DFSRecursive(graph, nextV, visited, result);
        }
    }
}

vector<char> DFS(Graph& graph, char start) {
    vector<char> result;
    set<char> visited;
    DFSRecursive(graph, start, visited, result);
    return result;
}

// ============================== 任务3：最短路径（Dijkstra）和最小支撑树（Prim） ==============================
pair<map<char, int>, map<char, char>> Dijkstra(Graph& graph, char start) {
    int n = graph.idxToVertex.size();
    map<char, int> dist;
    map<char, char> prev;
    set<char> visited;

    for (char v : graph.idxToVertex) {
        dist[v] = INT_MAX;
        prev[v] = '\0';
    }
    dist[start] = 0;

    while (visited.size() < n) {
        char curr = '\0';
        int minDist = INT_MAX;
        for (char v : graph.idxToVertex) {
            if (visited.find(v) == visited.end() && dist[v] < minDist) {
                minDist = dist[v];
                curr = v;
            }
        }
        if (curr == '\0') break;
        visited.insert(curr);

        vector<pair<char, int>> adj = graph.getAdjacent(curr);
        for (auto& p : adj) {
            char nextV = p.first;
            int weight = p.second;
            if (visited.find(nextV) == visited.end() && dist[curr] != INT_MAX) {
                int newDist = dist[curr] + weight;
                if (newDist < dist[nextV]) {
                    dist[nextV] = newDist;
                    prev[nextV] = curr;
                }
            }
        }
    }

    return make_pair(dist, prev);
}

void printDijkstraPath(map<char, char>& prev, char start, char target) {
    vector<char> path;
    for (char v = target; v != '\0'; v = prev[v]) {
        path.push_back(v);
    }
    reverse(path.begin(), path.end());

    cout << "路径 " << start << "->" << target << "：";
    if (path[0] != start) {
        cout << "无\n";
        return;
    }
    for (int i = 0; i < path.size(); ++i) {
        if (i > 0) cout << "->";
        cout << path[i];
    }
    cout << "\n";
}

pair<vector<tuple<char, char, int>>, int> Prim(Graph& graph, char start) {
    int n = graph.idxToVertex.size();
    set<char> inMST;
    vector<tuple<char, char, int>> mstEdges;
    int totalWeight = 0;

    inMST.insert(start);

    while (inMST.size() < n) {
        tuple<char, char, int> minEdge = make_tuple('\0', '\0', INT_MAX);
        for (char u : inMST) {
            vector<pair<char, int>> adj = graph.getAdjacent(u);
            for (auto& p : adj) {
                char v = p.first;
                int weight = p.second;
                if (inMST.find(v) == inMST.end() && weight < get<2>(minEdge)) {
                    minEdge = make_tuple(u, v, weight);
                }
            }
        }
        if (get<2>(minEdge) == INT_MAX) break;

        char u = get<0>(minEdge), v = get<1>(minEdge), w = get<2>(minEdge);
        mstEdges.push_back(minEdge);
        totalWeight += w;
        inMST.insert(v);
    }

    return make_pair(mstEdges, totalWeight);
}

// ============================== 任务4：双连通分量和关节点（Tarjan算法） ==============================
class TarjanBiconnected {
private:
    Graph& graph;
    map<char, int> disc;
    map<char, int> low;
    map<char, char> parent;
    set<char> visited;
    stack<pair<char, char>> edgeStack;
    vector<vector<pair<char, char>>> bcc;
    set<char> articulationPoints;
    int time;

    void dfs(char u) {
        visited.insert(u);
        disc[u] = low[u] = ++time;
        int children = 0;

        vector<pair<char, int>> adj = graph.getAdjacent(u);
        for (auto& p : adj) {
            char v = p.first;
            if (!visited.count(v)) {
                children++;
                parent[v] = u;
                edgeStack.push(make_pair(u, v));
                dfs(v);

                low[u] = min(low[u], low[v]);

                if ((parent[u] == '\0' && children > 1) || (parent[u] != '\0' && low[v] >= disc[u])) {
                    articulationPoints.insert(u);
                    vector<pair<char, char>> component;
                    while (edgeStack.top() != make_pair(u, v)) {
                        component.push_back(edgeStack.top());
                        edgeStack.pop();
                    }
                    component.push_back(edgeStack.top());
                    edgeStack.pop();
                    bcc.push_back(component);
                }
            } else if (v != parent[u] && disc[v] < disc[u]) {
                edgeStack.push(make_pair(u, v));
                low[u] = min(low[u], disc[v]);
            }
        }
    }

public:
    TarjanBiconnected(Graph& g) : graph(g), time(0) {}

    pair<vector<vector<pair<char, char>>>, set<char>> run(char start) {
        disc.clear(); low.clear(); parent.clear(); visited.clear();
        edgeStack = stack<pair<char, char>>();
        bcc.clear(); articulationPoints.clear(); time = 0;

        if (!visited.count(start)) {
            dfs(start);
            if (!edgeStack.empty()) {
                vector<pair<char, char>> component;
                while (!edgeStack.empty()) {
                    component.push_back(edgeStack.top());
                    edgeStack.pop();
                }
                bcc.push_back(component);
            }
        }

        return make_pair(bcc, articulationPoints);
    }
};

void printBCC(vector<vector<pair<char, char>>>& bcc) {
    cout << "双连通分量（边集合）：\n";
    for (int i = 0; i < bcc.size(); ++i) {
        cout << "分量 " << i+1 << "：";
        for (auto& p : bcc[i]) {
            cout << "(" << p.first << "," << p.second << ") ";
        }
        cout << "\n";
    }
}

void printArticulationPoints(set<char>& aps) {
    cout << "关节点（割点）：";
    for (char v : aps) {
        cout << v << " ";
    }
    cout << "\n";
}

// ============================== 构建图1和图2 ==============================
Graph buildGraph1() {
    vector<char> vertices = {'A', 'B', 'D', 'E', 'G', 'H', 'I', 'K', 'L'};
    Graph graph(vertices, false);

    vector<tuple<char, char, int>> edges = {
        make_tuple('A', 'B', 2), make_tuple('A', 'D', 4), make_tuple('B', 'E', 12), make_tuple('B', 'H', 3),
        make_tuple('D', 'E', 13), make_tuple('D', 'G', 6), make_tuple('E', 'G', 11), make_tuple('E', 'H', 1),
        make_tuple('E', 'K', 5), make_tuple('G', 'H', 8), make_tuple('G', 'L', 14), make_tuple('H', 'I', 10)
    };

    for (auto& t : edges) {
        graph.addEdge(get<0>(t), get<1>(t), get<2>(t));
    }
    return graph;
}

Graph buildGraph2() {
    vector<char> vertices = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L'};
    Graph graph(vertices, false);

    vector<tuple<char, char, int>> edges = {
        make_tuple('A', 'B', 1), make_tuple('A', 'C', 1), make_tuple('B', 'C', 1), make_tuple('B', 'D', 1),
        make_tuple('C', 'D', 1), make_tuple('D', 'E', 1), make_tuple('E', 'F', 1), make_tuple('E', 'G', 1),
        make_tuple('F', 'G', 1), make_tuple('G', 'H', 1), make_tuple('H', 'I', 1), make_tuple('H', 'J', 1),
        make_tuple('I', 'J', 1), make_tuple('J', 'K', 1), make_tuple('K', 'L', 1), make_tuple('J', 'L', 1)
    };

    for (auto& t : edges) {
        graph.addEdge(get<0>(t), get<1>(t), get<2>(t));
    }
    return graph;
}

// ============================== 主函数 ==============================
int main() {
    cout << "==================== 任务1：图1的邻接矩阵 ====================\n";
    Graph graph1 = buildGraph1();
    graph1.printAdjMatrix();
    cout << "\n";

    cout << "==================== 任务2：图1的BFS和DFS（起点A） ====================\n";
    vector<char> bfsOrder = BFS(graph1, 'A');
    cout << "BFS遍历顺序：";
    for (char v : bfsOrder) cout << v << " ";
    cout << "\n";

    vector<char> dfsOrder = DFS(graph1, 'A');
    cout << "DFS遍历顺序：";
    for (char v : dfsOrder) cout << v << " ";
    cout << "\n\n";

    cout << "==================== 任务3：图1的最短路径和最小支撑树（起点A） ====================\n";
    pair<map<char, int>, map<char, char>> dijkstraRes = Dijkstra(graph1, 'A');
    map<char, int> dist = dijkstraRes.first;
    map<char, char> prev = dijkstraRes.second;
    cout << "各顶点到A的最短距离：\n";
    for (auto& p : dist) {
        char v = p.first;
        int d = p.second;
        cout << "A到" << v << "：" << (d == INT_MAX ? "无路径" : to_string(d)) << "\n";
        if (d != INT_MAX) printDijkstraPath(prev, 'A', v);
    }

    pair<vector<tuple<char, char, int>>, int> primRes = Prim(graph1, 'A');
    vector<tuple<char, char, int>> mstEdges = primRes.first;
    int totalWeight = primRes.second;
    cout << "\n最小支撑树（MST）：\n";
    cout << "总权重：" << totalWeight << "\n";
    cout << "边集合：";
    for (auto& t : mstEdges) {
        cout << "(" << get<0>(t) << "," << get<1>(t) << "," << get<2>(t) << ") ";
    }
    cout << "\n\n";

    cout << "==================== 任务4：图2的双连通分量和关节点（不同起点） ====================\n";
    Graph graph2 = buildGraph2();
    vector<char> starts = {'A', 'E', 'H'};
    for (char start : starts) {
        cout << "=== 起点：" << start << " ===\n";
        TarjanBiconnected tarjan(graph2);
        pair<vector<vector<pair<char, char>>>, set<char>> tarjanRes = tarjan.run(start);
        vector<vector<pair<char, char>>> bcc = tarjanRes.first;
        set<char> aps = tarjanRes.second;
        printBCC(bcc);
        printArticulationPoints(aps);
        cout << "\n";
    }

    return 0;
}