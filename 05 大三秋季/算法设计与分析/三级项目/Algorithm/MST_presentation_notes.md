# MST 答辩演示稿（Markdown PPT）

> 用这个文件就能顺次演示，建议把“幻灯片”当作大纲在终端或 Markdown 查看器中逐页展示。

---

## SLIDE 1｜封面 & 核心信息
- **项目题目**：基于 Prim / Kruskal 的校园最小生成树对比
- **一句话亮点**：同源数据、双算法验证 + 步进演示 + SVG 可视化
- **运行命令**：`./build/mst_app [--prim|--kruskal|--both] [--step|--no-step]`
- **提示**：单算法默认步进，`--both` 默认一次跑完，可叠加 `--step`

---

## SLIDE 2｜整体结构 & 数据流
- `src/main.cpp:62-320`：负责 CSV 读取 → 邻接矩阵生成 → 运行算法 → 输出结果
- 输入：`data/edges_readable.csv`，输出目录：`output/prim|kruskal|shared`
- 步骤：准备目录 → 写入 `adjacency_matrix.csv` → 重新读取确保一致性
- 结尾打印：Prim/Kruskal 总权值、路径、输出文件位置

---

## SLIDE 3｜演示流程（建议节奏）
- 1️⃣ `./build/mst_app --prim`：启用步进，展示 Prim 如何“长树”
- 2️⃣ `./build/mst_app --kruskal`：同样步进，突出“全局排序”策略
- 3️⃣ `./build/mst_app --both --no-step`：一次性对比两个算法的最终结果
- 4️⃣ 打开 `output/prim/mst.svg`、`output/kruskal/mst.svg` 展示图形差异

---

## SLIDE 4｜Prim 算法概念
- **思想**：维护“已选节点集合”，每次找跨出集合的最小权值边扩展
- **复杂度**：当前邻接矩阵实现 `O(V^2)`，节点不多时更直观
- **适用场景**：稠密图、需要顺序演示“树如何扩展”的情况
- **关键词**：横切边、连续扩展、局部最优保证全局最优

---

## SLIDE 5｜Prim 实现细节
- 入口：`run_prim`（`src/prim_algo.cpp:205-285`）
  - `used` 标记、`order` 保存访问顺序、`mst` 记录选边
  - 双重循环遍历所有可能边，寻找 `best` 横切边
- 交互：`show_step_prompt`、`show_completion`（`src/prim_algo.cpp:162-200`）
  - 输出 `[Prim STEP xx]` + 当前路径，并等待 Enter
  - 若 `mst` < `n-1` 会提示图不连通
- 可视化：`write_steps/route/svg`（`src/prim_algo.cpp:51-120`），SVG 用红色突出 MST

---

## SLIDE 6｜Kruskal 算法概念
- **思想**：把所有边按权值排序，从小到大加入，只要不成环就收下
- **复杂度**：`O(E log E)`，排序主导，适合边多节点相对少的稀疏图
- **适用场景**：需要从全局选择最优边、或输入天然是边列表
- **关键词**：并查集、全局排序、无环约束

---

## SLIDE 7｜Kruskal 实现细节
- 入口：`run_kruskal`（`src/kruskal_algo.cpp:269-350`）
  - 先 `sort` 边集，再用 `DSU`（`src/kruskal_algo.cpp:38-68`）判断是否同集合
  - `adj` + `build_partial_route` + `dfs`（`src/kruskal_algo.cpp:189-333`）实时生成连通顺序
- 交互：每加入一条边输出 `[Kruskal STEP xx]`、当前节点集合、当前路径
- 可视化：`write_steps/route/svg` 输出绿色 MST，与 Prim 形成对比

---

## SLIDE 8｜Prim vs. Kruskal 对比
| 维度 | Prim | Kruskal |
| --- | --- | --- |
| 输入依赖 | 邻接矩阵/表，起点任选 | 边列表 + 排序 |
| 复杂度 | 矩阵版 `O(V^2)`；堆优化 `O(E log V)` | `O(E log E)` |
| 形象比喻 | 像水沿着最近的管道扩散 | 像从所有材料中挑最省的连接件 |
| 演示体验 | 路径连续、直观 | 边可能跨区跳跃，体现“全局最优” |
| 本项目表现 | 步进路径 = 已访问顺序（`src/prim_algo.cpp:219-274`） | DFS 预览当前连通块（`src/kruskal_algo.cpp:300-333`） |

**总结台词**：两种算法都给出总权值 2.242，互相验证正确性，可按图密度选择合适算法。

---

## SLIDE 9｜主程序 & 模块分工
- `src/main.cpp`
  - 解析参数，自动开关步进模式（`src/main.cpp:217-254`）
  - 读取 CSV，写/读邻接矩阵，统一数据来源（`src/main.cpp:70-203`）
  - 调用 Prim / Kruskal，输出汇总结果（`src/main.cpp:276-320`）
- `src/prim_algo.cpp`
  - 输出 + SVG 工具、步进提示、朴素 Prim 循环
- `src/kruskal_algo.cpp`
  - 并查集、DFS、步进提示、排序循环

---

## SLIDE 10｜Q&A 备忘
- **为何不用更快结构？** 节点规模不大，`O(V^2)` 已够；如需扩展可换优先队列实现。
- **如何确认正确性？** 输出总权值一致、`steps.txt` 有完整选择理由、SVG 可视化对照。
- **断开图怎么办？** 步进提示会警告“图可能不连通”，日志记录中断位置便于排查。
- **可否应用到别的场景？** 换 CSV 即可；输出路径与 SVG 的骨架已经适配。

---

## SLIDE 11｜结束语 & 演示提示
- 结语：本项目把 MST 教学、验证、可视化与交互演示整合到一个 CLI 工具
- 演示提醒：提前打开终端 + SVG 预览器，按提纲顺序演示即可
- 可扩展方向：改进权值获取、加入更多算法（如 Borůvka）、或接入 GUI

---

> 按照每页要点配合命令行 demo，就能完整讲完整个项目。祝答辩顺利！
