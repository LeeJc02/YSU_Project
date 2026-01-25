// Kruskal 算法模块：并查集构建 MST，并输出日志、路线和 SVG 示意图。
// Kruskal 算法模块：并查集构建 MST，并输出日志、路线和 SVG 示意图。
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Kruskal 算法专用的边结构，与主程序保持一致。
struct Edge {
    int u;
    int v;
    double w;
};

namespace {

const double PI = 3.14159265358979323846;
const char *kKruskalLabel = "Kruskal";

// 简单的路径拼接工具。
string join(const string &dir, const string &name) {
    if (dir.empty()) {
        return name;
    }
    if (dir.back() == '/') {
        return dir + name;
    }
    return dir + "/" + name;
}

// 并查集：用于快速判断两节点是否在同一连通块。
struct DSU {
    vector<int> parent;
    vector<int> size;
    void init(int n) {
        parent.resize(n);
        size.assign(n, 1);
        for (int i = 0; i < n; ++i) {
            parent[i] = i;
        }
    }
    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }
    bool merge(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) {
            return false;
        }
        if (size[a] < size[b]) {
            swap(a, b);
        }
        parent[b] = a;
        size[a] += size[b];
        return true;
    }
};

string format_set(const vector<string> &items) {
    string text = "{";
    for (size_t i = 0; i < items.size(); ++i) {
        if (i) {
            text += ", ";
        }
        text += items[i];
    }
    text += "}";
    return text;
}

void write_steps(const string &path, const vector<string> &steps, double sum) {
    ofstream out(path.c_str());
    out << fixed << setprecision(3);
    for (size_t i = 0; i < steps.size(); ++i) {
        out << "STEP " << setw(2) << setfill('0') << (i + 1) << ": "
            << steps[i] << "\n";
    }
    out << setfill(' ');
    out << "TOTAL: " << sum << "\n";
}

void write_route(const string &path, const string &route, double sum) {
    ofstream out(path.c_str());
    out << route << "\n";
    out << fixed << setprecision(3) << "TOTAL: " << sum << "\n";
}

void write_svg(const vector<string> &nodes,
               const vector<Edge> &edges,
               const vector<Edge> &mst,
               const string &path) {
    int n = static_cast<int>(nodes.size());
    if (n == 0) {
        return;
    }

    const double width = 800.0;
    const double height = 800.0;
    const double radius = (n == 1 ? 0.0 : 320.0);
    const double cx = width / 2.0;
    const double cy = height / 2.0;

    vector<pair<double, double>> pos(n);
    for (int i = 0; i < n; ++i) {
        double ang = (n == 1 ? 0.0 : 2.0 * PI * i / n);
        pos[i] = make_pair(cx + radius * cos(ang), cy + radius * sin(ang));
    }

    set<pair<int, int>> mst_pairs;
    for (size_t i = 0; i < mst.size(); ++i) {
        int a = mst[i].u;
        int b = mst[i].v;
        if (a > b) {
            swap(a, b);
        }
        mst_pairs.insert(make_pair(a, b));
    }

    ostringstream svg;
    svg << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << width
        << "\" height=\"" << height << "\">";
    svg << "<rect width=\"100%\" height=\"100%\" fill=\"#ffffff\" />";
    svg << "<title>Kruskal Minimum Spanning Tree</title>";
    svg << fixed << setprecision(1);

    for (size_t i = 0; i < edges.size(); ++i) {
        int u = edges[i].u;
        int v = edges[i].v;
        int a = min(u, v);
        int b = max(u, v);
        bool in_mst = mst_pairs.count(make_pair(a, b)) > 0;
        string color = in_mst ? "#5cb85c" : "#cccccc";
        string stroke = in_mst ? "3" : "1";
        string opacity = in_mst ? "0.9" : "0.4";

        svg << "<g opacity=\"" << opacity << "\">";
        svg << "<line x1=\"" << pos[u].first << "\" y1=\"" << pos[u].second
            << "\" x2=\"" << pos[v].first << "\" y2=\"" << pos[v].second
            << "\" stroke=\"" << color << "\" stroke-width=\"" << stroke << "\" />";
        if (in_mst) {
            double mx = (pos[u].first + pos[v].first) / 2.0;
            double my = (pos[u].second + pos[v].second) / 2.0;
            svg << "<text x=\"" << mx << "\" y=\"" << my
                << "\" fill=\"#5cb85c\" font-size=\"14\" text-anchor=\"middle\">"
                << setprecision(3) << edges[i].w << "</text>";
            svg << setprecision(1);
        }
        svg << "</g>";
    }

    for (int i = 0; i < n; ++i) {
        svg << "<g>";
        svg << "<circle cx=\"" << pos[i].first << "\" cy=\"" << pos[i].second
            << "\" r=\"18\" fill=\"#5bc0de\" stroke=\"#000\" stroke-width=\"1.5\" />";
        svg << "<text x=\"" << pos[i].first << "\" y=\"" << pos[i].second + 5.0
            << "\" font-size=\"14\" text-anchor=\"middle\" fill=\"#000\">" << nodes[i] << "</text>";
        svg << "</g>";
    }
    svg << "</svg>";

    ofstream out(path.c_str());
    out << svg.str();
}

string build_route(const vector<string> &nodes, const vector<int> &order) {
    if (order.empty()) {
        return "";
    }
    string line = nodes[order[0]];
    for (size_t i = 1; i < order.size(); ++i) {
        line += "-";
        line += nodes[order[i]];
    }
    return line;
}

// DFS 用以生成 "A-B-C" 顺序，帮助理解管线走向。
void dfs(int u,
         const vector<vector<int>> &adj,
         const vector<string> &nodes,
         vector<int> &vis,
         vector<int> &order) {
    vis[u] = 1;
    order.push_back(u);
    vector<int> next = adj[u];
    sort(next.begin(), next.end(), [&](int a, int b) { return nodes[a] < nodes[b]; });
    for (size_t i = 0; i < next.size(); ++i) {
        int v = next[i];
        if (!vis[v]) {
            dfs(v, adj, nodes, vis, order);
        }
    }
}

// 基于当前邻接表跑 DFS 生成临时路径描述。
string build_partial_route(const vector<vector<int>> &adj,
                           const vector<string> &nodes) {
    int n = static_cast<int>(nodes.size());
    int start = -1;
    for (int i = 0; i < n; ++i) {
        if (!adj[i].empty()) {
            start = i;
            break;
        }
    }
    if (start == -1) {
        return "";
    }
    vector<int> vis(n, 0);
    vector<int> order;
    dfs(start, adj, nodes, vis, order);
    return build_route(nodes, order);
}

// 从标准输入消费一行以实现“敲回车继续”。
void wait_for_enter() {
    string dummy;
    getline(cin, dummy);
}

// 打印逐步提示并附带当前连通状况。
void show_step_prompt(const string &algo,
                      int step_index,
                      const string &message,
                      const string &extra_line,
                      bool interactive) {
    if (!interactive) {
        return;
    }
    cout << "[" << algo << " STEP " << setw(2) << setfill('0') << step_index << "] "
         << message << "\n";
    cout << setfill(' ');
    if (!extra_line.empty()) {
        cout << extra_line << "\n";
    }
    cout << "按 Enter 键继续..." << flush;
    wait_for_enter();
    cout << "\n";
}

// 在算法结束时汇报最终路径和权值。
void show_completion(const string &algo,
                     const string &route,
                     double total,
                     bool interactive) {
    if (!interactive) {
        return;
    }
    ostringstream oss;
    oss << fixed << setprecision(3) << total;
    cout << "[" << algo << "] 构建完成，最终路径: "
         << (route.empty() ? "(无)" : route) << "，总长度: " << oss.str() << "\n\n";
}

}  // namespace

// Kruskal：排序边，按权值尝试合并，用 DFS 给出连通顺序的展示。
bool run_kruskal(const vector<string> &nodes,
                 const vector<Edge> &edges,
                 const string &out_dir,
                 string &route,
                 double &total,
                 bool interactive) {
    // 算法流程总览：
    // 1. 复制 edges 到 sorted 并按权值升序排序，确保始终优先尝试最便宜的连边。
    // 2. 使用并查集 dsu 跟踪各节点所属连通分量，merge 成功代表新边不会成环。
    // 3. 维护 mst（最终边集合）、adj（用于 DFS 展示）、steps（文字日志）等状态。
    // 4. 每接纳一条边，更新 in_tree/current_set/total，并调用 show_step_prompt 展示变化。
    // 5. 最后用 DFS(0) 得到 route，写出 steps/route/svg，若边数不足 n-1 则提示图不连通。
    int n = static_cast<int>(nodes.size());
    if (n == 0) {
        total = 0.0;
        route.clear();
        return false;
    }

    vector<Edge> sorted = edges;  // sorted 存储按权值排序后的所有候选边。
    // sort：依据权重、端点名排序，保证边以“权值最小优先”顺序被遍历。
    sort(sorted.begin(), sorted.end(), [&](const Edge &a, const Edge &b) {
        if (abs(a.w - b.w) > 1e-9) {
            return a.w < b.w;
        }
        if (nodes[a.u] != nodes[b.u]) {
            return nodes[a.u] < nodes[b.u];
        }
        return nodes[a.v] < nodes[b.v];
    });

    DSU dsu;  // 并查集用于判断某条边是否会形成环。
    dsu.init(n);
    vector<Edge> mst;                    // mst 保存已加入生成树的边，供 SVG 使用。
    vector<string> steps;                // steps 记录日志，轨迹写入 steps.txt。
    vector<int> in_tree(n, 0);           // in_tree[i] = 1 表示节点 i 至少出现过一次在 MST 中。
    vector<string> current_set{nodes[0]};  // current_set 保存当前覆盖的节点名称，用于日志。
    in_tree[0] = 1;                      // 选定节点 0 为初始“探索点”，与 Prim 保持一致。
    steps.push_back("起点 " + format_set(current_set));
    total = 0.0;
    int step_counter = 1;                // 交互提示编号。
    vector<vector<int>> adj(n);          // adj 是已接受边构成的邻接表，用于 build_partial_route/dfs。
    show_step_prompt(kKruskalLabel, step_counter++, steps.back(),
                     "当前路径: " + nodes[0], interactive);  // 展示起点信息。

    for (size_t i = 0; i < sorted.size(); ++i) {
        // 边按权值从小到大尝试加入生成树。
        int u = sorted[i].u;
        int v = sorted[i].v;
        double w = sorted[i].w;
        if (dsu.merge(u, v)) {
            mst.push_back(sorted[i]);  // 成功 merge 说明这条边被正式接纳。
            total += w;                // 同时累加权值到总长度。
            in_tree[u] = in_tree[v] = 1;  // 节点 u/v 至少被某条 MST 边覆盖。
            adj[u].push_back(v);
            adj[v].push_back(u);       // 将边写入邻接表，供 DFS 构造路径。
            current_set.clear();
            for (int k = 0; k < n; ++k) {
                if (in_tree[k]) {
                    current_set.push_back(nodes[k]);
                }
            }
            sort(current_set.begin(), current_set.end());
            stringstream info;
            info << fixed << setprecision(3)
                 << "选择边 (" << nodes[u] << " - " << nodes[v] << ") 权重 "
                 << w << " -> 当前节点集合 " << format_set(current_set);
            steps.push_back(info.str());  // 记录本次选边详情。
            string preview = build_partial_route(adj, nodes);  // DFS 当前邻接表以便展示大致线路。
            string extra_line = preview.empty() ? "" : ("当前路径: " + preview);
            // show_step_prompt：打印“选择哪条边 + 当前路径”，并根据 interactive 等待 Enter。
            show_step_prompt(kKruskalLabel, step_counter++, steps.back(),
                             extra_line, interactive);
            if (static_cast<int>(mst.size()) == n - 1) {
                break;  // 接纳了 n-1 条边即可提前结束。
            }
        }
    }

    vector<int> vis(n, 0);
    vector<int> order;
    dfs(0, adj, nodes, vis, order);  // 深度优先生成最终 route，用于文字输出。

    route = build_route(nodes, order);
    // write_steps：写入所有 STEP 日志 + TOTAL。
    write_steps(join(out_dir, "steps.txt"), steps, total);
    // write_route：记录 DFS 顺序形成的线路串和总长度。
    write_route(join(out_dir, "route.txt"), route, total);
    // write_svg：以绿色高亮 MST 边，展示最终拓扑。
    write_svg(nodes, edges, mst, join(out_dir, "mst.svg"));
    if (static_cast<int>(mst.size()) < n - 1 && interactive) {
        cout << "[" << kKruskalLabel
             << "] 注意：未能覆盖所有节点，图可能不连通。\n\n";
    }
    // show_completion：在交互模式下总结一次路径与权值，方便观众收尾。
    show_completion(kKruskalLabel, route, total, interactive);
    return true;
}
