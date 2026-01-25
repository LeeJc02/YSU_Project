// 主程序：读取 CSV 数据并比较 Prim 与 Kruskal 两种最小生成树方案。
#include <cmath>      // fabs
#include <cstdlib>    // system
#include <fstream>    // ifstream, ofstream
#include <iomanip>    // setprecision
#include <iostream>   // cout
#include <map>        // map
#include <set>        // set
#include <sstream>    // stringstream
#include <string>     // string
#include <vector>     // vector

using namespace std;

// 边结构：记录一条无向边的两个端点索引与权重。
struct Edge {
    int u;
    int v;
    double w;
};

const double INF = 1e9;
const string kDataFile = "data/edges_readable.csv";
const string kOutRoot = "output";
const string kOutPrim = "output/prim";
const string kOutKruskal = "output/kruskal";
const string kOutShared = "output/shared";
const string kMatrixFile = "adjacency_matrix.csv";

enum class RunMode {
    kBoth,
    kPrimOnly,
    kKruskalOnly,
};

bool run_prim(const vector<string> &nodes,
              const vector<vector<double>> &matrix,
              const vector<Edge> &edges,
              const string &out_dir,
              string &route,
              double &total,
              bool interactive);

bool run_kruskal(const vector<string> &nodes,
                 const vector<Edge> &edges,
                 const string &out_dir,
                 string &route,
                 double &total,
                 bool interactive);

// 将目录与文件名拼成简单路径（无需处理复杂兼容）。
string join(const string &dir, const string &name) {
    if (dir.empty()) {
        return name;
    }
    if (dir.back() == '/') {
        return dir + name;
    }
    return dir + "/" + name;
}

// 调用系统命令创建输出目录。
void prepare_dirs() {
    system(("mkdir -p " + kOutRoot).c_str());
    system(("mkdir -p " + kOutPrim).c_str());
    system(("mkdir -p " + kOutKruskal).c_str());
    system(("mkdir -p " + kOutShared).c_str());
}

// 读取 CSV 并填充节点列表、邻接矩阵以及边集。
bool read_graph(vector<string> &nodes,
                vector<vector<double>> &matrix,
                vector<Edge> &edges) {
    ifstream in(kDataFile.c_str());
    if (!in) {
        return false;
    }

    string header;
    getline(in, header);  // 跳过第一行标题

    set<string> node_set;
    struct RawEdge {
        string a;
        string b;
        double w;
    };
    vector<RawEdge> raw;

    string line;
    while (getline(in, line)) {  // 按行读取边信息
        if (line.empty()) {
            continue;
        }
        stringstream ss(line);
        string a, b, w;
        getline(ss, a, ',');
        getline(ss, b, ',');
        getline(ss, w, ',');
        if (a.empty() || b.empty() || w.empty()) {
            continue;
        }
        raw.push_back({a, b, atof(w.c_str())});
        node_set.insert(a);
        node_set.insert(b);
    }

    nodes.assign(node_set.begin(), node_set.end());
    int n = static_cast<int>(nodes.size());
    if (n == 0) {
        return false;
    }

    map<string, int> index;
    for (int i = 0; i < n; ++i) {
        index[nodes[i]] = i;
    }

    matrix.assign(n, vector<double>(n, INF));
    for (int i = 0; i < n; ++i) {
        matrix[i][i] = 0.0;
    }

    edges.clear();
    for (size_t i = 0; i < raw.size(); ++i) {  // 同时填充矩阵及边集
        int u = index[raw[i].a];
        int v = index[raw[i].b];
        double w = raw[i].w;
        matrix[u][v] = w;
        matrix[v][u] = w;
        edges.push_back({min(u, v), max(u, v), w});
    }
    return true;
}

// 将邻接矩阵写到共享目录中，方便检查。
void write_matrix(const vector<string> &nodes,
                  const vector<vector<double>> &matrix) {
    ofstream out(join(kOutShared, kMatrixFile).c_str());
    out << ",";
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (i) {
            out << ",";
        }
        out << nodes[i];
    }
    out << "\n";

    out << fixed << setprecision(6);
    for (size_t i = 0; i < nodes.size(); ++i) {
        out << nodes[i];
        for (size_t j = 0; j < nodes.size(); ++j) {
            out << ",";
            if (matrix[i][j] >= INF / 2) {
                out << "INF";
            } else {
                out << matrix[i][j];
            }
        }
        out << "\n";
    }
}

// 从保存好的邻接矩阵 CSV 中重新读取矩阵（满足需求 1）。
bool read_matrix_file(vector<string> &nodes,
                      vector<vector<double>> &matrix) {
    ifstream in(join(kOutShared, kMatrixFile).c_str());
    if (!in) {
        return false;
    }
    string header;
    if (!getline(in, header)) {
        return false;
    }
    stringstream hs(header);
    string cell;
    vector<string> labels;
    while (getline(hs, cell, ',')) {
        labels.push_back(cell);
    }
    nodes.clear();
    for (size_t i = 1; i < labels.size(); ++i) {
        nodes.push_back(labels[i]);
    }
    int n = static_cast<int>(nodes.size());
    matrix.assign(n, vector<double>(n, INF));
    int row = 0;
    while (getline(in, header) && row < n) {
        stringstream rs(header);
        vector<string> cols;
        while (getline(rs, cell, ',')) {
            cols.push_back(cell);
        }
        for (int col = 1; col <= n; ++col) {
            if (cols[col] == "INF") {
                matrix[row][col - 1] = INF;
            } else {
                matrix[row][col - 1] = atof(cols[col].c_str());
            }
        }
        ++row;
    }
    return true;
}

// 打印命令行帮助，方便答辩演示时快速提示。
void print_usage(const string &exe) {
    cout << "用法: " << exe << " [--prim | --kruskal | --both]\n";
    cout << "  --prim / -p      仅运行 Prim 算法\n";
    cout << "  --kruskal / -k   仅运行 Kruskal 算法\n";
    cout << "  --both           两种算法都运行（默认）\n";
    cout << "  --step / -s      启用命令行步进模式\n";
    cout << "  --no-step        禁用步进模式\n";
    cout << "  --help / -h      显示此帮助信息\n";
}

// 程序入口：解析参数后加载数据并按需求执行 Prim/Kruskal。
int main(int argc, char *argv[]) {
    RunMode mode = RunMode::kBoth;
    string exe_name = (argc > 0 ? string(argv[0]) : "mst_app");
    bool interactive = false;
    bool step_flag_specified = false;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--prim" || arg == "-p") {
            mode = RunMode::kPrimOnly;
        } else if (arg == "--kruskal" || arg == "-k") {
            mode = RunMode::kKruskalOnly;
        } else if (arg == "--both") {
            mode = RunMode::kBoth;
        } else if (arg == "--step" || arg == "-s") {
            interactive = true;
            step_flag_specified = true;
        } else if (arg == "--no-step") {
            interactive = false;
            step_flag_specified = true;
        } else if (arg == "--help" || arg == "-h") {
            print_usage(exe_name);
            return 0;
        } else {
            cout << "未知参数: " << arg << "\n";
            print_usage(exe_name);
            return 1;
        }
    }

    bool need_prim = (mode == RunMode::kBoth || mode == RunMode::kPrimOnly);
    bool need_kruskal = (mode == RunMode::kBoth || mode == RunMode::kKruskalOnly);

    if (!step_flag_specified) {
        // 若用户未指定，单算法默认启用步进，两算法默认一次跑完。
        interactive = (mode != RunMode::kBoth);
    }

    prepare_dirs();

    vector<string> nodes;
    vector<vector<double>> matrix;
    vector<Edge> edges;
    if (!read_graph(nodes, matrix, edges)) {
        cout << "读取数据失败，请检查 data/edges_readable.csv。\n";
        return 1;
    }

    write_matrix(nodes, matrix);

    vector<string> nodes_from_file;
    vector<vector<double>> matrix_from_file;
    if (!read_matrix_file(nodes_from_file, matrix_from_file)) {
        cout << "读取邻接矩阵文件失败。\n";
        return 1;
    }

    vector<Edge> edges_for_alg = edges;
    vector<string> nodes_for_alg = nodes_from_file;
    vector<vector<double>> matrix_for_alg = matrix_from_file;

    string prim_route;
    double prim_total = 0.0;
    string kruskal_route;
    double kruskal_total = 0.0;
    bool ran_prim = false;
    bool ran_kruskal = false;

    if (need_prim) {
        ran_prim = run_prim(nodes_for_alg, matrix_for_alg, edges_for_alg, kOutPrim,
                            prim_route, prim_total, interactive);
    }
    if (need_kruskal) {
        ran_kruskal = run_kruskal(nodes_for_alg, edges_for_alg, kOutKruskal,
                                  kruskal_route, kruskal_total, interactive);
    }

    cout << fixed << setprecision(3);
    cout << "===== 结果汇总 =====\n";
    if (ran_prim) {
        cout << "Prim   总长度: " << prim_total << "\n";
    } else if (need_prim) {
        cout << "Prim   运行失败，请检查输入数据。\n";
    }
    if (ran_kruskal) {
        cout << "Kruskal总长度: " << kruskal_total << "\n";
    } else if (need_kruskal) {
        cout << "Kruskal运行失败，请检查输入数据。\n";
    }
    if (ran_prim && ran_kruskal) {
        if (fabs(prim_total - kruskal_total) < 1e-9) {
            cout << "结论: 两种算法得到相同的最短距离。\n";
        }
    }
    if (ran_prim) {
        cout << "Prim   路径: " << prim_route << "\n";
        cout << "Prim   输出: " << kOutPrim << "/steps.txt, route.txt, mst.svg\n";
    }
    if (ran_kruskal) {
        cout << "Kruskal路径: " << kruskal_route << "\n";
        cout << "Kruskal输出: " << kOutKruskal << "/steps.txt, route.txt, mst.svg\n";
    }
    cout << "邻接矩阵: " << kOutShared << "/" << kMatrixFile << "\n";

    return 0;
}
