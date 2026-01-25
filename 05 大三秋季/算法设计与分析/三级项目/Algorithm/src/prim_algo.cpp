// Prim 算法模块：计算 MST 并输出日志、路线和 SVG 示意图。
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

// Prim 算法专用的边结构，与主程序保持一致。
struct Edge {
    int u;
    int v;
    double w;
};

namespace {

const double INF = 1e9;
const double PI = 3.14159265358979323846;
const char *kPrimLabel = "Prim";

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

// 将 "A, B, C" 等节点名包装成 {A, B, C} 格式。
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

// Prim 过程记录。
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

// Prim 路径文字结果。
void write_route(const string &path, const string &route, double sum) {
    ofstream out(path.c_str());
    out << route << "\n";
    out << fixed << setprecision(3) << "TOTAL: " << sum << "\n";
}

// 简易 SVG 示意图，圆环排布节点，红色显示 MST 边。
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
    svg << "<title>Prim Minimum Spanning Tree</title>";
    svg << fixed << setprecision(1);

    for (size_t i = 0; i < edges.size(); ++i) {
        int u = edges[i].u;
        int v = edges[i].v;
        int a = min(u, v);
        int b = max(u, v);
        bool in_mst = mst_pairs.count(make_pair(a, b)) > 0;
        string color = in_mst ? "#d9534f" : "#cccccc";
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
                << "\" fill=\"#d9534f\" font-size=\"14\" text-anchor=\"middle\">"
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

// 将节点访问顺序转为 “A-B-C” 的字符串。
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

// 从标准输入读取一行，用于步进模式。
void wait_for_enter() {
    string dummy;
    getline(cin, dummy);
}

// 打印单步信息并等待用户确认。
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

// 展示最终输出概览，便于说明算法结果。
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

// 朴素 Prim：逐次扩展当前最短边，记录过程并输出结果文件。
bool run_prim(const vector<string> &nodes,
              const vector<vector<double>> &matrix,
              const vector<Edge> &edges,
              const string &out_dir,
              string &route,
              double &total,
              bool interactive) {
    // 算法流程总览：
    // 1. 以节点 0 为起点，将其视为当前生成树；used/order 记录“已纳入”的节点。
    // 2. 每一轮扫描所有“树内节点”到“树外节点”的边，寻找权重最小者 best。
    // 3. 将 best 的目标节点并入树，累加 total，同时把该边推入 mst 便于 SVG。
    // 4. 将步骤描述 push 到 steps，若开启 interactive 则调用 show_step_prompt 等待用户确认。
    // 5. 循环 n-1 轮或直到找不到可拓展的边，最后落盘 steps/route/svg 并返回 true。
    int n = static_cast<int>(nodes.size());
    if (n == 0) {
        total = 0.0;
        route.clear();
        return false;
    }

    vector<int> used(n, 0);          // used[i] = 1 表示节点 i 已加入生成树。
    used[0] = 1;                     // 默认以 nodes[0] 为起点。
    vector<int> order(1, 0);         // order 记录被选中的节点顺序，用于 route 文本。
    vector<Edge> mst;                // mst 保存已选择的边，用来生成 SVG。
    vector<string> steps;            // steps 记录每一步的人类可读描述。
    steps.push_back("起点 " + format_set({nodes[0]}));
    total = 0.0;
    int step_counter = 1;            // step_counter 用于给交互提示编号，便于观众跟进。
    string start_route = build_route(nodes, order);  // 只有起点时的初始路径文本。
    // show_step_prompt：打印当前步骤详情并在 interactive==true 时等待 Enter。
    show_step_prompt(kPrimLabel, step_counter++, steps.back(),
                     start_route.empty() ? "" : ("当前路径: " + start_route),
                     interactive);

    for (int step = 0; step < n - 1; ++step) {
        // 寻找一条连接当前树与未访问节点之间的最短边。
        double best = INF;
        int from = -1;
        int to = -1;
        for (int i = 0; i < n; ++i) {
            if (!used[i]) {
                continue;  // 只从树内节点出发。
            }
            for (int j = 0; j < n; ++j) {
                if (used[j] || matrix[i][j] >= INF / 2) {
                    continue;  // 跳过树内目标或不存在的边。
                }
                if (matrix[i][j] < best - 1e-9) {
                    best = matrix[i][j];  // 更新当前最短横切边权重。
                    from = i;             // 记录横切边的树内端点。
                    to = j;               // 记录横切边的树外端点。
                }
            }
        }
        if (to == -1) {
            break;  // 没有可扩展节点，图已不连通。
        }
        used[to] = 1;            // 将最短边的目标节点纳入生成树。
        order.push_back(to);     // 记录访问顺序，用于 route 输出。
        total += best;           // 累加总权重。
        mst.push_back({from, to, best});  // 保存边信息，后续生成 SVG。

        // 记录当前树中已纳入的节点名称，便于写日志。
        vector<string> current;
        for (int i = 0; i < n; ++i) {
            if (used[i]) {
                current.push_back(nodes[i]);
            }
        }
        sort(current.begin(), current.end());
        stringstream info;
        info << fixed << setprecision(3);
        info << "选择边 (" << nodes[from] << " - " << nodes[to] << ") 权重 "
             << best << " -> 当前节点集合 " << format_set(current);
        steps.push_back(info.str());  // 将文字描述追加到步骤记录。
        string current_route = build_route(nodes, order);  // 更新现有路线串。
        // 再次调用 show_step_prompt，向用户展示此次扩展并等待下一步。
        show_step_prompt(kPrimLabel, step_counter++, steps.back(),
                         current_route.empty() ? "" : ("当前路径: " + current_route),
                         interactive);
    }

    route = build_route(nodes, order);
    // write_steps：落盘所有文字步骤 + TOTAL，方便答辩或查错。
    write_steps(join(out_dir, "steps.txt"), steps, total);
    // write_route：输出 A-B-C 路线与总长度，为终端概览提供支撑。
    write_route(join(out_dir, "route.txt"), route, total);
    // write_svg：用红色高亮 mst 内的边，生成图形化结果。
    write_svg(nodes, edges, mst, join(out_dir, "mst.svg"));
    if (static_cast<int>(mst.size()) < n - 1 && interactive) {
        cout << "[" << kPrimLabel
             << "] 注意：图可能不连通，已无法继续扩展生成树。\n\n";
    }
    // show_completion：在交互模式下打印最终路径和权重，为观众总结一次。
    show_completion(kPrimLabel, route, total, interactive);
    return true;
}
