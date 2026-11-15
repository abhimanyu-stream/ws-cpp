/**
 * TOPIC 17 — Data Structures and Algorithms in C++
 * ==================================================
 * Covers: linked list, stack, queue, hash table, BST, heap,
 *         graph (BFS/DFS), sorting, searching, recursion,
 *         dynamic programming, two pointers, sliding window,
 *         bit manipulation, complexity analysis
 *
 * Compile: g++ -std=c++17 -Wall 17_dsa.cpp -o dsa
 */

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <climits>

// ═══════════════════════════════════════════════════════════════════════════════
// 1. Linked List
// ═══════════════════════════════════════════════════════════════════════════════
struct ListNode {
    int       val;
    ListNode* next;
    explicit ListNode(int v) : val(v), next(nullptr) {}
};

class LinkedList {
public:
    ~LinkedList() { while (head_) { auto* t = head_->next; delete head_; head_ = t; } }

    void push_front(int v) { auto* n = new ListNode(v); n->next = head_; head_ = n; }
    void push_back(int v)  {
        auto* n = new ListNode(v);
        if (!head_) { head_ = n; return; }
        auto* cur = head_;
        while (cur->next) cur = cur->next;
        cur->next = n;
    }

    void print() const {
        for (auto* p = head_; p; p = p->next) std::cout << p->val << " → ";
        std::cout << "null\n";
    }

    // Reverse in-place — O(n)
    void reverse() {
        ListNode* prev = nullptr, *cur = head_, *next = nullptr;
        while (cur) { next = cur->next; cur->next = prev; prev = cur; cur = next; }
        head_ = prev;
    }

    // Detect cycle — Floyd's slow/fast pointer
    bool has_cycle() const {
        auto *slow = head_, *fast = head_;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
            if (slow == fast) return true;
        }
        return false;
    }

    // Find middle — slow/fast pointer
    ListNode* middle() const {
        auto *slow = head_, *fast = head_;
        while (fast && fast->next) { slow = slow->next; fast = fast->next->next; }
        return slow;
    }

private:
    ListNode* head_ = nullptr;
};

void section_linked_list() {
    std::cout << "\n=== Linked List ===\n";
    LinkedList ll;
    ll.push_back(1); ll.push_back(2); ll.push_back(3); ll.push_back(4); ll.push_back(5);
    ll.print();
    ll.reverse();
    ll.print();
    std::cout << "has_cycle: " << ll.has_cycle() << "\n";
    std::cout << "middle: " << ll.middle()->val << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// 2. Stack (using std::stack) + use case: balanced parentheses
// ═══════════════════════════════════════════════════════════════════════════════
bool is_balanced(const std::string& s) {
    std::stack<char> st;
    for (char c : s) {
        if (c == '(' || c == '[' || c == '{') st.push(c);
        else if (c == ')' || c == ']' || c == '}') {
            if (st.empty()) return false;
            char top = st.top(); st.pop();
            if ((c==')' && top!='(') || (c==']' && top!='[') || (c=='}' && top!='{'))
                return false;
        }
    }
    return st.empty();
}

void section_stack() {
    std::cout << "\n=== Stack ===\n";
    for (const auto& expr : {"(())", "([{}])", "([)]", "((("}) {
        std::cout << "'" << expr << "' balanced: " << is_balanced(expr) << "\n";
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// 3. Binary Search Tree
// ═══════════════════════════════════════════════════════════════════════════════
struct BSTNode {
    int val;
    BSTNode *left = nullptr, *right = nullptr;
    explicit BSTNode(int v) : val(v) {}
};

class BST {
public:
    ~BST() { destroy(root_); }

    void insert(int v) { root_ = insert(root_, v); }
    bool search(int v) const { return search(root_, v); }
    void inorder()     const { inorder(root_); std::cout << "\n"; }

    // Height
    int height() const { return height(root_); }

private:
    BSTNode* root_ = nullptr;

    BSTNode* insert(BSTNode* n, int v) {
        if (!n) return new BSTNode(v);
        if (v < n->val)      n->left  = insert(n->left,  v);
        else if (v > n->val) n->right = insert(n->right, v);
        return n;
    }

    bool search(BSTNode* n, int v) const {
        if (!n) return false;
        if (v == n->val) return true;
        return v < n->val ? search(n->left, v) : search(n->right, v);
    }

    void inorder(BSTNode* n) const {
        if (!n) return;
        inorder(n->left);
        std::cout << n->val << " ";
        inorder(n->right);
    }

    int height(BSTNode* n) const {
        if (!n) return 0;
        return 1 + std::max(height(n->left), height(n->right));
    }

    void destroy(BSTNode* n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }
};

void section_bst() {
    std::cout << "\n=== Binary Search Tree ===\n";
    BST bst;
    for (int v : {5,3,7,1,4,6,8,2}) bst.insert(v);
    std::cout << "inorder (sorted): ";  bst.inorder();
    std::cout << "search(4): "       << bst.search(4) << "\n";
    std::cout << "search(9): "       << bst.search(9) << "\n";
    std::cout << "height: "          << bst.height()  << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// 4. Graph — BFS and DFS
// ═══════════════════════════════════════════════════════════════════════════════
class Graph {
public:
    explicit Graph(int n) : adj_(n) {}

    void add_edge(int u, int v) {
        adj_[u].push_back(v);
        adj_[v].push_back(u);  // undirected
    }

    // BFS — level-by-level traversal (finds shortest path in unweighted graph)
    std::vector<int> bfs(int start) const {
        std::vector<bool> visited(adj_.size(), false);
        std::queue<int>   q;
        std::vector<int>  order;

        visited[start] = true;
        q.push(start);

        while (!q.empty()) {
            int node = q.front(); q.pop();
            order.push_back(node);
            for (int neighbor : adj_[node]) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }
        return order;
    }

    // DFS — depth-first traversal
    std::vector<int> dfs(int start) const {
        std::vector<bool> visited(adj_.size(), false);
        std::vector<int>  order;
        dfs_helper(start, visited, order);
        return order;
    }

private:
    std::vector<std::vector<int>> adj_;

    void dfs_helper(int node, std::vector<bool>& visited, std::vector<int>& order) const {
        visited[node] = true;
        order.push_back(node);
        for (int neighbor : adj_[node])
            if (!visited[neighbor])
                dfs_helper(neighbor, visited, order);
    }
};

void section_graph() {
    std::cout << "\n=== Graph BFS / DFS ===\n";
    Graph g(6);
    g.add_edge(0,1); g.add_edge(0,2);
    g.add_edge(1,3); g.add_edge(1,4);
    g.add_edge(2,5);

    std::cout << "BFS from 0: ";
    for (auto v : g.bfs(0)) std::cout << v << " ";
    std::cout << "\n";

    std::cout << "DFS from 0: ";
    for (auto v : g.dfs(0)) std::cout << v << " ";
    std::cout << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// 5. Sorting Algorithms
// ═══════════════════════════════════════════════════════════════════════════════
void bubble_sort(std::vector<int>& v) {
    int n = v.size();
    for (int i = 0; i < n-1; ++i)
        for (int j = 0; j < n-1-i; ++j)
            if (v[j] > v[j+1]) std::swap(v[j], v[j+1]);
}

void selection_sort(std::vector<int>& v) {
    int n = v.size();
    for (int i = 0; i < n-1; ++i) {
        int min_i = i;
        for (int j = i+1; j < n; ++j)
            if (v[j] < v[min_i]) min_i = j;
        std::swap(v[i], v[min_i]);
    }
}

void insertion_sort(std::vector<int>& v) {
    for (int i = 1; i < (int)v.size(); ++i) {
        int key = v[i], j = i-1;
        while (j >= 0 && v[j] > key) { v[j+1] = v[j]; --j; }
        v[j+1] = key;
    }
}

void merge_sort(std::vector<int>& v, int l, int r) {
    if (l >= r) return;
    int mid = l + (r-l)/2;
    merge_sort(v, l, mid);
    merge_sort(v, mid+1, r);
    std::inplace_merge(v.begin()+l, v.begin()+mid+1, v.begin()+r+1);
}

int partition(std::vector<int>& v, int lo, int hi) {
    int pivot = v[hi], i = lo-1;
    for (int j = lo; j < hi; ++j)
        if (v[j] <= pivot) std::swap(v[++i], v[j]);
    std::swap(v[++i], v[hi]);
    return i;
}

void quick_sort(std::vector<int>& v, int lo, int hi) {
    if (lo < hi) {
        int p = partition(v, lo, hi);
        quick_sort(v, lo, p-1);
        quick_sort(v, p+1, hi);
    }
}

void section_sorting() {
    std::cout << "\n=== Sorting Algorithms ===\n";
    // Complexities: bubble O(n²), selection O(n²), insertion O(n²) best O(n)
    //               merge O(n log n) stable, quick O(n log n) avg O(n²) worst

    auto print_v = [](const std::vector<int>& v, const std::string& label) {
        std::cout << label << ": ";
        for (auto x : v) std::cout << x << " ";
        std::cout << "\n";
    };

    std::vector<int> base = {5,3,8,1,9,2,7,4,6};
    auto v = base; bubble_sort(v);    print_v(v, "bubble");
    v = base; selection_sort(v);  print_v(v, "selection");
    v = base; insertion_sort(v);  print_v(v, "insertion");
    v = base; merge_sort(v, 0, v.size()-1); print_v(v, "merge");
    v = base; quick_sort(v, 0, v.size()-1); print_v(v, "quick");
}

// ═══════════════════════════════════════════════════════════════════════════════
// 6. Binary Search  — O(log n)
// ═══════════════════════════════════════════════════════════════════════════════
int binary_search(const std::vector<int>& v, int target) {
    int lo = 0, hi = (int)v.size()-1;
    while (lo <= hi) {
        int mid = lo + (hi-lo)/2;
        if      (v[mid] == target) return mid;
        else if (v[mid] <  target) lo = mid+1;
        else                       hi = mid-1;
    }
    return -1;
}

void section_searching() {
    std::cout << "\n=== Searching ===\n";
    std::vector<int> v = {1,3,5,7,9,11,13,15};
    std::cout << "binary_search(7):  " << binary_search(v, 7)  << " (index)\n";
    std::cout << "binary_search(6):  " << binary_search(v, 6)  << " (-1=not found)\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// 7. Dynamic Programming
// ═══════════════════════════════════════════════════════════════════════════════

// Fibonacci — memoization (top-down DP)
int fib_memo(int n, std::unordered_map<int,int>& memo) {
    if (n <= 1) return n;
    if (memo.count(n)) return memo[n];
    return memo[n] = fib_memo(n-1, memo) + fib_memo(n-2, memo);
}

// Fibonacci — tabulation (bottom-up DP)
int fib_dp(int n) {
    if (n <= 1) return n;
    std::vector<int> dp(n+1);
    dp[0]=0; dp[1]=1;
    for (int i = 2; i <= n; ++i) dp[i] = dp[i-1]+dp[i-2];
    return dp[n];
}

// 0/1 Knapsack — O(n*W)
int knapsack(const std::vector<int>& weights,
             const std::vector<int>& values,
             int W) {
    int n = weights.size();
    std::vector<std::vector<int>> dp(n+1, std::vector<int>(W+1, 0));
    for (int i = 1; i <= n; ++i)
        for (int w = 0; w <= W; ++w) {
            dp[i][w] = dp[i-1][w];
            if (weights[i-1] <= w)
                dp[i][w] = std::max(dp[i][w], dp[i-1][w-weights[i-1]] + values[i-1]);
        }
    return dp[n][W];
}

// Longest Common Subsequence
int lcs(const std::string& a, const std::string& b) {
    int m = a.size(), n = b.size();
    std::vector<std::vector<int>> dp(m+1, std::vector<int>(n+1, 0));
    for (int i = 1; i <= m; ++i)
        for (int j = 1; j <= n; ++j)
            dp[i][j] = (a[i-1]==b[j-1]) ? dp[i-1][j-1]+1
                                         : std::max(dp[i-1][j], dp[i][j-1]);
    return dp[m][n];
}

void section_dp() {
    std::cout << "\n=== Dynamic Programming ===\n";
    std::unordered_map<int,int> memo;
    std::cout << "fib(10) memo: " << fib_memo(10, memo) << "\n";
    std::cout << "fib(10) dp:   " << fib_dp(10)         << "\n";

    std::vector<int> w = {2,3,4,5}, v = {3,4,5,6};
    std::cout << "knapsack(W=8): " << knapsack(w, v, 8) << "\n";   // 10

    std::cout << "LCS(ABCBDAB,BDCAB): " << lcs("ABCBDAB","BDCAB") << "\n";  // 4
}

// ═══════════════════════════════════════════════════════════════════════════════
// 8. Two Pointers + Sliding Window
// ═══════════════════════════════════════════════════════════════════════════════

// Two sum in sorted array — O(n)
std::pair<int,int> two_sum_sorted(const std::vector<int>& v, int target) {
    int lo = 0, hi = (int)v.size()-1;
    while (lo < hi) {
        int sum = v[lo] + v[hi];
        if      (sum == target) return {lo, hi};
        else if (sum <  target) ++lo;
        else                    --hi;
    }
    return {-1, -1};
}

// Max subarray sum (Kadane's) — O(n)
int max_subarray(const std::vector<int>& v) {
    int max_sum = v[0], cur = v[0];
    for (int i = 1; i < (int)v.size(); ++i) {
        cur     = std::max(v[i], cur + v[i]);
        max_sum = std::max(max_sum, cur);
    }
    return max_sum;
}

// Longest substring without repeating characters — O(n) sliding window
int longest_no_repeat(const std::string& s) {
    std::unordered_map<char,int> last;
    int max_len = 0, left = 0;
    for (int right = 0; right < (int)s.size(); ++right) {
        if (last.count(s[right]) && last[s[right]] >= left)
            left = last[s[right]] + 1;
        last[s[right]] = right;
        max_len = std::max(max_len, right - left + 1);
    }
    return max_len;
}

// Max sum subarray of size k — O(n) sliding window
int max_sum_k(const std::vector<int>& v, int k) {
    int window = 0;
    for (int i = 0; i < k; ++i) window += v[i];
    int max_sum = window;
    for (int i = k; i < (int)v.size(); ++i) {
        window += v[i] - v[i-k];
        max_sum = std::max(max_sum, window);
    }
    return max_sum;
}

void section_two_ptr_window() {
    std::cout << "\n=== Two Pointers / Sliding Window ===\n";
    std::vector<int> sorted = {1,3,5,7,9,11};
    auto [i,j] = two_sum_sorted(sorted, 12);
    std::cout << "two_sum(12): [" << i << "]=" << sorted[i] << " [" << j << "]=" << sorted[j] << "\n";

    std::vector<int> arr = {-2,1,-3,4,-1,2,1,-5,4};
    std::cout << "max_subarray: " << max_subarray(arr) << "\n";   // 6

    std::cout << "longest_no_repeat('abcabcbb'): " << longest_no_repeat("abcabcbb") << "\n";  // 3
    std::cout << "max_sum_k=3 of {2,1,5,1,3,2}: " << max_sum_k({2,1,5,1,3,2}, 3) << "\n";   // 9
}

// ═══════════════════════════════════════════════════════════════════════════════
// 9. Bit Manipulation
// ═══════════════════════════════════════════════════════════════════════════════
void section_bit_manipulation() {
    std::cout << "\n=== Bit Manipulation ===\n";

    int n = 0b1010'1100;  // 172

    // Check bit i
    auto check_bit  = [](int x, int i){ return (x >> i) & 1; };
    // Set bit i
    auto set_bit    = [](int x, int i){ return x | (1 << i); };
    // Clear bit i
    auto clear_bit  = [](int x, int i){ return x & ~(1 << i); };
    // Toggle bit i
    auto toggle_bit = [](int x, int i){ return x ^ (1 << i); };
    // Count set bits (popcount)
    auto popcount   = [](int x){ int c=0; while(x){c+=x&1;x>>=1;} return c; };
    // Check power of 2
    auto is_pow2    = [](int x){ return x>0 && (x & (x-1))==0; };
    // Lowest set bit
    auto lsb        = [](int x){ return x & (-x); };

    std::cout << "n = "            << n             << " (" << std::hex << n << std::dec << ")\n";
    std::cout << "bit 2: "         << check_bit(n,2) << "\n";
    std::cout << "set bit 0: "     << set_bit(n,0)   << "\n";
    std::cout << "clear bit 3: "   << clear_bit(n,3) << "\n";
    std::cout << "toggle bit 1: "  << toggle_bit(n,1)<< "\n";
    std::cout << "popcount: "      << popcount(n)    << "\n";
    std::cout << "is_pow2(16): "   << is_pow2(16)    << "\n";
    std::cout << "is_pow2(12): "   << is_pow2(12)    << "\n";
    std::cout << "lsb(12): "       << lsb(12)        << "\n";   // 4 (bit 2 is lowest set)

    // XOR trick: find single number in array where every other appears twice
    std::vector<int> nums = {4,1,2,1,2};
    int single = 0;
    for (int x : nums) single ^= x;   // pairs cancel out (a^a=0), single remains
    std::cout << "single number: " << single << "\n";   // 4
}

// ═══════════════════════════════════════════════════════════════════════════════
// 10. Complexity Quick Reference
// ═══════════════════════════════════════════════════════════════════════════════
void section_complexity() {
    std::cout << "\n=== Complexity Quick Reference ===\n";
    std::cout << "O(1)       constant  — hash table lookup, array access\n";
    std::cout << "O(log n)   log       — binary search, balanced BST\n";
    std::cout << "O(n)       linear    — linear search, single loop\n";
    std::cout << "O(n log n) linearithm— merge sort, heap sort, std::sort\n";
    std::cout << "O(n²)      quadratic — bubble/selection/insertion sort, nested loops\n";
    std::cout << "O(2^n)     exponential—naive fibonacci, subset enumeration\n";
    std::cout << "O(n!)      factorial — permutation generation\n";
}

int main() {
    std::cout << "╔══ Topic 17: Data Structures & Algorithms ══╗\n";

    section_linked_list();
    section_stack();
    section_bst();
    section_graph();
    section_sorting();
    section_searching();
    section_dp();
    section_two_ptr_window();
    section_bit_manipulation();
    section_complexity();

    std::cout << "\n✓ Topic 17 complete.\n";
}
