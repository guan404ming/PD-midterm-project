# 程式設計 - 期中專案

總共有 ${n_I}$ 位員工
${n_K}$ 代表除了第 0 號班型(休假班型)之外的班型個數
每個月月中要安排次月共 ${n_J}$ 天的班表

- 班型的種類 (shift category)
    - 正常 - 不會在 18 : 00 之後值班
    - 夜班 - 在 18 : 00 之後至少一任意時段值班
    - 休假 - 整天 24 個時段都不用上班

- 三點限制需要絕對遵守
    - 每位員工在接下來的 ${n_J}$ 天排班中，至少有 L 天完整的休假。
    - 每位員工每連續的七天中至少要有一天完整休假
    - 每位員工在一天內只能被指派恰好一種班型
- 我們希望在滿足限制的前提下最小化以下數值
    - 總缺工數 - 為接下來 ${n_J}$ 天中共 24 ${n_J}$ 個時段的缺工人數總和。
        - 缺工人數 (lackamount) =  ${max}$ {表定人力需求 − 實際值班人數, 0}
    - 未滿足的休假需求總數 - 當某員工提出希望在某一天休假，但仍被指派去上班，未滿足的休假需求總數就往上加 1。
    - 七天內的超額夜班總數 - 當一位員工在連續七天中被指派兩天含夜班的班型，則他在這七天中的超額夜班數為 2 − 1 = 1 天。

        - 「連續七天」的檢查是以「rolling window」的方式進行的，也就是這些「連續七天」是會重疊的。

<b>目標函數： ${min}$ ( ${總缺工數 + w1 × 未滿足的休假需求總數 + w2 × 超額夜班總數}$ )</b>

## 輸入輸出格式：

第一列存放七個整數 ${n_I、n_J、n_K、L、w_1、w_2 和 R}$

接下來的資訊分為三大部分，依序分別是 ${n_K}$ 種班型的各時段排班狀況、${n_J}$ 天中各時段的人力需求以及 ${R}$ 項休假需求。

- 第一部分
    - 從第2列至第 ${n_K + 2}$ 列
    - 每列各存放 24 個整數，其中第 ${k + 1}$ 列依序存著 ${s_{k,1}、s_{k,2}}$ 直到 ${s_{k,24}}$，表示第 k 號班型需要值班的時段，若為 1 表示要值班，若為 0 則否。
    - 第 ${n_K + 2}$ 列會固定存放第 0 號班型(休假)的排班狀態， 也就是 {0, 0, 0, 0, ..., 0}。

- 第二部分
    - 從第 ${n_K +3}$ 列至第 ${n_K +n_J +2}$ 列
    - 每列各存放 24 個整數，其中第 ${n_K + 2 + j}$ 列依序存著 ${d_{j,1}、d_{j,2}}$ 直到 ${d_{j,24}}$，表示第 ${j}$ 天各時段的人力需求。 
    - 請注意在資料被輸入時，其格式是圖 2 的轉置(transpose)，亦即在圖 2 中一列 是一個時段、一欄是一天，而在輸入資料中一列是一天，一欄則是一個時段。

- 第三部分
    - 從第 ${n_K +n_J +3}$ 列至第 ${n_K +n_J +4}$ 列，也就是最後的兩列資料。
    - 每列各存放 R 個整數，其中第 ${n_K +n_J +3}$ 列依序存著 ${i_1、i_2}$ 直到 ${i_R}$，依序表示提出請假需求的員工編號;第 ${n_K + n_J + 4}$ 列依序存著 ${j_1、j_2}$ 直到 ${j_R}$，依序表示第 ${i_1、i_2}$ 直到 ${i_R}$ 號員工希望請假的日期，舉例來說，若 ${i_1 = 3}$ 且 ${j_1 = 5}$，代表第一項請假需求為第三號員工希望在第五天請假。

- 變數值域
    - ${10 ≤ n_I ≤ 100}$
    - ${n_J}$ ∈ {28,29,30,31}
    - ${5 ≤ n_K ≤ 30}$
    - L ∈ {8,9,10}
    - ${1 ≤ w_1 ≤ 100}$
    - ${1 ≤ w_2 ≤ 100}$
    - ${0 ≤ R ≤ n_in_j}$
    - ${s_{k,t}}$ ∈ {0,1}
    - ${0 ≤ d_{j,t} ≤ n_I}$
    - ${1 ≤ i_r ≤ n_I}$
    - ${1 ≤ j_r ≤ n_J}$