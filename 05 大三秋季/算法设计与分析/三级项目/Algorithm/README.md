# 最小生成树管线项目报告

## 摘要
本项目基于校园建筑之间的直线距离数据，利用最小生成树思想制定最短水管铺设方案。系统首先将建筑视为图节点、距离作为无向边权，并把生成的邻接矩阵写入 `output/shared/adjacency_matrix.csv`，再从该文本读取数据驱动两种 MST 算法：Prim 与 Kruskal。每种算法都输出步骤日志（`steps.txt`）、最优路线（`route.txt`）以及简易 SVG 示意图。程序运行后可清晰对比两种算法的构造过程与结果，为校园基础设施规划提供量化依据。

## 绪论
随着校园建设的发展，合理布设地下管网可以显著节约成本。本项目采集了若干建筑物之间的距离数据，以图论方式建模，并通过最小生成树算法给出覆盖全部建筑、总长度最短的管线连接方案。项目目标如下：
1. 抽象无向加权图并输出邻接矩阵文件，系统再次读取以保证数据闭环。
2. 采用 Prim 与 Kruskal 两种 MST 算法生成最优管线，并以文本展示每一步。
3. 绘制最优方案的简易图，用于辅助理解拓扑结构。

## 项目大纲
```
data/edges_readable.csv       原始边权数据
output/
  shared/ adjacency_matrix.csv  邻接矩阵文本
  prim/   steps.txt|route.txt|mst.svg
  kruskal/ steps.txt|route.txt|mst.svg
src/
  main.cpp                      主流程：读写矩阵、调度算法、汇总结果
  prim_algo.cpp                 Prim 算法实现与可视化
  kruskal_algo.cpp              Kruskal 算法实现与可视化
README.md                      项目报告
```

## 设计思路
### 数据读写与管线建模
1. **CSV → 邻接矩阵**：从 `edges_readable.csv` 读取 `source,target,value` 列，使用去重后的建筑名排序得到节点集合，以二维数组存放边权（若无边则视为 `INF`）。矩阵以 CSV 形式写入共享目录，方便验证。
2. **邻接矩阵再读入**：系统从生成的 `adjacency_matrix.csv` 重新读取节点顺序与边权值，再驱动后续算法满足“系统通过读取文本获取矩阵”的要求。

### Prim 算法设计
Prim 算法适合稠密图，核心思想是维护一个“已纳入生成树的节点集合”，每次选择一条横跨已选与未选之间的最短边扩展。

**伪代码：**
```
choose start node s
visited = {s}
while visited.size < n:
    find edge (u,v) with minimal weight
        where u in visited, v not in visited
    add v into visited
    record edge and log message
```

**实现要点：**
1. 使用 `vector<int> used` 标识已纳入节点。
2. 双层循环枚举 `used` 内节点与未访问节点之间所有边，挑选最小值。
3. 每次扩展后记录 `{当前节点集合}`、更新总权重和步骤日志。
4. 生成顺序即 Prim 的路线，因此 `route.txt` 直接写 `A-B-C` 格式。
5. SVG 中节点按圆环排列，最小生成树边用红色加粗。

### Kruskal 算法设计
Kruskal 更适用于稀疏图，依赖排序 + 并查集，按边权从小到大尝试加入，若两个端点不在同一集合则合并。

**伪代码：**
```
sort all edges by weight
make-set for each node
steps = ["起点 {start}"]
for each edge (u,v):
    if find(u) != find(v):
        union(u, v)
        record edge, add to MST
        update {当前节点集合}
```

**实现要点：**
1. 并查集（Disjoint Set Union, DSU）支持路径压缩与按规模合并。
2. 为保持与 Prim 一致的集合表示，先把统一起点（取 `nodes[0]`）标记为已纳入。
3. 每当添加新边时更新 `in_tree` 数组，并重新生成 `{当前节点集合}`，写入步骤日志。
4. 路线输出使用 DFS 以字典序遍历生成树，得到 `A-B-C` 顺序。
5. SVG 使用绿色突出 Kruskal 的 MST 边。

## 图结构表示报告
### 全局建模
- **节点标识**：以建筑名称为字符串主键，排序后映射为连续索引，贯穿 `matrix`、`edges` 以及算法输出，保证 Prim 与 Kruskal 共用同一节点序。
- **邻接矩阵**：`vector<vector<double>> matrix` 以 `INF` 表示缺边，`matrix[i][i]=0`；写入 `output/shared/adjacency_matrix.csv` 并再次读回，实现图结构的文本化与可追溯。
- **边列表**：`vector<Edge> edges` 存储已归一化的无向边（小索引在前），既供 Kruskal 排序也被两个算法的 SVG 输出复用，确保图结构一致。

### Prim 的图表示
- **访问状态矩阵视角**：Prim 计算过程中保留 `used` 与 `matrix` 的组合，直接在邻接矩阵上枚举最短横切边，体现“从已知子图向外扩散”的结构理解。
- **序列化路线**：`order` 记录 Prim 访问顺序，通过 `build_route` 输出 `A-B-C` 字符串，将生成树看作沿时间轴展开的链式结构。
- **可视化映射**：`write_svg` 以节点索引计算极坐标位置，选中的矩阵边在 SVG 中加粗为红色，将抽象的矩阵行列映射成直观空间布局。

### Kruskal 的图表示
- **边排序视角**：Kruskal 对 `edges` 排序后遍历，核心数据结构是并查集 `DSU`，从“边集合”角度维护若干连通块，强调图的组件合并过程。
- **生成树邻接表**：最终选中的 `mst` 被转换为 `adj` 邻接表，再 DFS 输出路线。该过程让边集合回归到树状结构，便于对生成树的拓扑进行多样遍历。
- **集合日志**：通过 `in_tree` 实时更新 `{节点集合}`，突出并查集中每次 union 后图的连通分量如何扩张，记录了图结构由多个森林并入单一树的演化。

### 两种表示的互补性
- **矩阵 vs. 边集**：Prim 偏向矩阵化表示，适合理解节点局部连接；Kruskal 则以边集和连通分量为中心，强调全局权重排序。
- **演进轨迹**：Prim 的 `order` 展示从起点扩散的路径；Kruskal 的 `mst` + `adj` 则展示全局选择后形成的树形结构，二者从不同视角揭示生成树的构建机理。
- **共享输出结构**：尽管数据结构不同，两个算法最终共享 `steps.txt`、`route.txt`、`mst.svg` 三种表示，将图结构的构造过程、线性序列和空间布局统一输出，便于交叉验证。

## 核心代码实现
### 主流程 (`src/main.cpp`)
```cpp
prepare_dirs();                         // 创建 output/...
read_graph(nodes, matrix, edges);       // CSV -> 邻接矩阵
write_matrix(nodes, matrix);            // 保存 adjacency_matrix.csv
read_matrix_file(nodes, matrix);        // 再次从文本加载矩阵
run_prim(nodes, matrix, edges, ...);    // Prim 生成树
run_kruskal(nodes, edges, ...);         // Kruskal 生成树
```
主程序确保所有算法输入来自文本文件，最终在终端输出总长度、路线及生成文件位置。

### Prim 模块 (`src/prim_algo.cpp`)
关键流程：
```cpp
used[0] = 1; order = {0};
for step in range(n-1):
    find best edge (u,v) with u visited, v not visited
    used[v] = 1; order.push_back(v); total += weight;
    steps.log("选择边 (u - v) 权重 weight -> 当前节点集合 { ... }");
build_route(order); write_steps(); write_route(); write_svg();
```
日志以 `STEP xx` 前缀，集合使用 `{节点名称}`，便于对照。

### Kruskal 模块 (`src/kruskal_algo.cpp`)
关键流程：
```cpp
sort(edges);
dsu.init(n);
in_tree[start] = 1;
steps.log("起点 {start}");
for edge (u,v,w) in edges:
    if dsu.merge(u, v):
        mark u,v in in_tree
        steps.log("选择边 ... -> 当前节点集合 {...}");
        mst.push_back(edge);
dfs(0); build_route(); write_steps(); write_route(); write_svg();
```
通过 `in_tree` 保证每一步的集合与 Prim 一样以 `{}` 展示。

## 分析结果
### Prim
- 总长度：`2.242`
- 路线：`信院-理学院-西里西亚-电气楼-西二-西三-西一-西五-艺术与设计学院-燕园餐厅-大食堂-西区5组团-里仁`
- 优势：实现简单，扩展顺序清晰；对稠密图性能稳定。

### Kruskal
- 总长度：`2.242`
- 路线：`信院-理学院-西里西亚-西五-艺术与设计学院-电气楼-大食堂-西区5组团-里仁-燕园餐厅-西二-西一-西三`
- 优势：排序后主循环简洁，适用于稀疏图；并查集能高效避免环。

### 综合对比
| 指标            | Prim                                | Kruskal                                 |
|----------------|--------------------------------------|------------------------------------------|
| 起点选择        | 固定 `信院`                          | 固定 `信院`                              |
| 扩展策略        | 逐步扩展最短横切边                  | 按权值全局排序 + 并查集合并             |
| 日志展示        | 已访问集合随扩展递增                | `in_tree` 跟踪已合并节点，集合格式一致 |
| 路线生成方式    | 直接按照 Prim 顺序输出              | 对 MST 图执行 DFS                        |
| 最终总长度      | `2.242`                              | `2.242`                                  |

两种算法均得到同一最小生成树，总长度一致，说明输入数据图是连通且 MST 唯一。

## 结论
1. 利用邻接矩阵、Prim 与 Kruskal 三步构建的水管铺设方案满足“覆盖全部建筑且总长度最短”的要求。
2. 文本日志、路线以及 SVG 示意图为规划与审查提供直观依据。
3. 两种算法在当前数据下表现一致，但面对不同拓扑结构时仍各有适用范围：Prim 更适合节点密集、权值已知的场景，Kruskal 更适合稀疏边集或动态边加入的情况。

## 心得体会
- **数据闭环的重要性**：先写邻接矩阵再读入，可检测数据完整性并方便第三方复核。
- **日志统一格式**：`STEP xx` + `{}` 集合让算法过程更清晰，即使初学者也能快速理解。
- **可视化辅助**：SVG 图提供简单且无依赖的查看方式，自定义配色（红/绿）在比较两个算法时非常醒目。
- **算法比较**：在同一数据集上对比 Prim 与 Kruskal，深化了对它们差异与适用范围的理解，强调选择合适算法的重要性。

## 运行方法
```bash
cmake -S . -B build
cmake --build build
./build/mst_app
```
运行后查看 `output/` 目录中的 CSV、步骤文本、路线文本与 SVG 即可完成验证。若需重新运行，可清空 `output/` 目录保持结构不变，或直接覆盖原文件。

运行时可附带以下参数以单独测试某个算法：
- `./build/mst_app --prim` 仅运行 Prim 算法
- `./build/mst_app --kruskal` 仅运行 Kruskal 算法
- `./build/mst_app --both` 同时运行两种算法（默认行为）
