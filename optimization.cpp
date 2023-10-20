#include <algorithm>
#include <assert.h>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#define debug(x) cout << #x << : << x << endl;
#define TIME (ld) clock() / CLOCKS_PER_SEC

using namespace std;
typedef long long ll;
typedef long double ld;

struct point {
  int x, y;
  point() : x(0), y(0) {}
  point(int x_, int y_) : x(x_), y(y_) {}
  point(const point &a) : x(a.x), y(a.y) {}

  friend ostream &operator<<(ostream &os, const point &p) {
    os << "X: " << p.x << ", Y: " << p.y;
    return os;
  }

  friend istream &operator>>(istream &is, point &p) {
    is >> p.x >> p.y;
    return is;
  }

  point &operator=(point a) {
    x = a.x;
    y = a.y;
    return *this;
  }
};

bool operator==(point a, point b) { return a.x == b.x && a.y == b.y; }

bool operator!=(point a, point b) { return !(a == b); }

point operator+(point a, point b) { return {a.x + b.x, a.y + b.y}; }

point operator-(point a, point b) { return {a.x - b.x, a.y - b.y}; }

bool operator<(point a, point b) {
  return make_pair(a.x, a.y) < make_pair(b.x, b.y);
}

int TIMER = 0;
int A, B, C, K;
int visited_cells;     /// visited cells
int known_cells;       /// cells that are known
map<point, char> grid; /// our known grid
map<point, int> used;  /// visited cells

vector<point> dirs = {point(0, -1), point(0, 1), point(-1, 0), point(1, 0)};

bool set_used(point p) {
  if (used.find(p) == used.end()) {
    ++visited_cells;
    used[p] = 1;
    return true;
  }
  return false;
}

/// fire the light if it is border of known grid
bool light_a_fire_check(point p) {
  for (int dir = 0; dir < 4; ++dir) {
    if (grid.find(p + dirs[dir]) == grid.end()) {
      return true;
    }
  }
  return false;
}

void set_cell(point p, char c) {
  if (grid.find(p) != grid.end())
    return;
  if (c == '_')
    ++known_cells;
  grid[p] = c;
}

vector<vector<char>> FIRE() {
  cout << "3" << endl;
  TIMER += C;
  vector<vector<char>> sub_grid(2 * K + 1, vector<char>(2 * K + 1));
  for (int i = 0; i < 2 * K + 1; ++i) {
    for (int j = 0; j < 2 * K + 1; ++j) {
      cin >> sub_grid[i][j];
    }
  }
  return sub_grid;
}

void fire(point p) {
  vector<vector<char>> sub_grid = FIRE();
  for (int i = -K; i <= K; ++i) {
    for (int j = -K; j <= K; ++j) {
      point new_point = p + point(i, j);
      if (sub_grid[i + K][j + K] == '_') {
        set_cell(new_point, '_');
      } else if (i == 0 || j == 0) {
        set_cell(new_point, '#');
      }
    }
  }
}

void terminate_dfs() {
  if (visited_cells == known_cells) {
    cout << 4 << " " << TIMER << endl;
    exit(0);
  }
}

/// we will chose the nearest possible cell what are reachable from us

struct state {
  point p;
  point dir;
  state() : p(), dir() {}
  state(point p_, point dir_) : p(p_), dir(dir_) {}
  state(const state &a) : p(a.p), dir(a.dir) {}
  void turn_clockwise() {
    int dir_x = dir.x;
    int dir_y = dir.y;
    dir.x = dir_y;
    dir.y = -dir_x;
  }

  void turn_overwise() {
    int dir_x = dir.x;
    int dir_y = dir.y;
    dir.x = -dir_y;
    dir.y = dir_x;
  }

  void move() {
    p.x += dir.x;
    p.y += dir.y;
  }
  state &operator=(state a) {
    p = a.p;
    dir = a.dir;
    return *this;
  }
};

bool operator==(state a, state b) { return a.p == b.p && a.dir == b.dir; }

bool operator!=(state a, state b) { return !(a == b); }

bool operator<(state a, state b) {
  return make_pair(a.p, a.dir) < make_pair(b.p, b.dir);
}

void MOVE_FORWARD() {
  cout << 1 << endl;
  int ok;
  cin >> ok;
  assert(ok);
  TIMER += A;
}

void ROTATE(int type) {
  cout << 2 << " " << type << endl;
  int ok;
  cin >> ok;
  assert(ok);
  TIMER += B;
}

void restore_optimal_path(state finish, state start,
                          map<state, state> &previous) {
  vector<state> operations;
  while (finish != start) {
    operations.push_back(finish);
    finish = previous[finish];
  }
  operations.push_back(start);
  reverse(operations.begin(), operations.end());
  for (int i = 1; i < (int)operations.size(); ++i) {
    if (operations[i - 1].p != operations[i].p) {
      MOVE_FORWARD();
    } else {
      state cur = operations[i - 1];
      cur.turn_clockwise();
      if (cur.dir == operations[i].dir) {
        ROTATE(1);
      } else {
        ROTATE(0);
      }
    }
  }
}

state find_nearest(point p, point dir) {
  map<state, int> cur_dist;
  set<pair<int, state>> que;

  que.insert(make_pair(0, state(p, dir)));
  cur_dist[state(p, dir)] = 0;
  map<state, state> previous;

  auto relax = [&](state new_state, state prev_state, int dist) {
    if (cur_dist.find(new_state) == cur_dist.end()) {
      cur_dist[new_state] = dist;
      que.insert(make_pair(cur_dist[new_state], new_state));
      previous[new_state] = prev_state;
    } else if (cur_dist[new_state] > dist) {
      que.erase(que.find(make_pair(cur_dist[new_state], new_state)));
      cur_dist[new_state] = dist;
      que.insert(make_pair(cur_dist[new_state], new_state));
      previous[new_state] = prev_state;
    }
  };

  while (que.size()) {
    pair<int, state> u = *(que.begin());
    que.erase(que.begin());

    if (used.find(u.second.p) == used.end()) {
      restore_optimal_path(u.second, state(p, dir), previous);
      return u.second;
    }

    /// move in direction
    {
      state new_state = u.second;
      new_state.move();
      if (grid.find(new_state.p) != grid.end() && grid[new_state.p] == '_') {
        relax(new_state, u.second, u.first + A);
      }
    }

    /// turn clockwise
    {
      state new_state = u.second;
      new_state.turn_clockwise();
      relax(new_state, u.second, u.first + B);
    }

    /// turn overwise
    {
      state new_state = u.second;
      new_state.turn_overwise();
      relax(new_state, u.second, u.first + B);
    }
  }
  assert(false);
}

void dfs(point p, point dir) {
  set_used(p);
  if (light_a_fire_check(p)) {
    fire(p);
  }
  terminate_dfs();
  state cur = find_nearest(p, dir);
  dfs(cur.p, cur.dir);
}

void solve() {
  point start;
  cin >> start;

  point point_looking_to;
  cin >> point_looking_to;

  point dir = point_looking_to - start;

  /// transfrom input math x, y to x', y' - for programmers
  swap(start.x, start.y);
  swap(dir.x, dir.y);
  dir.x *= -1;

  cin >> A >> B >> C >> K;

  dfs(start, dir);
}

int main() {
  ios_base::sync_with_stdio(0);
  cin.tie(0);
  int t = 1;
  for (int i = 0; i < t; ++i) {
    solve();
  }
}
