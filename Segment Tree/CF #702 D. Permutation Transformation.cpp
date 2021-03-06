#include <bits/stdc++.h>
#define ll long long int
using namespace std; 

// Codeforces Round #702 D. Permutation Transformation
// https://codeforces.com/contest/1490/problem/D

const int32_t SZ = 1e5;

template <typename NODE, typename UPDATE>
struct segtree {
	bool built = false, lazy[4 * SZ];
	NODE zero = NODE(), t[4 * SZ];
	UPDATE noop = UPDATE(), upds[4 * SZ];
	int32_t tl[4 * SZ], tr[4 * SZ];
	inline void pushdown(int32_t v)	{
		if (lazy[v]) {
			apply(v * 2, upds[v]);
			apply(v * 2 + 1, upds[v]);
			lazy[v] = 0;
			upds[v] = noop;
		}
	}
	inline void apply(int32_t v, UPDATE &val) {
		if (tl[v] != tr[v]) {
			lazy[v] = 1;
			upds[v].combine(val, tl[v], tr[v]);
		}
		val.apply(t[v], tl[v], tr[v]);
	}
	template <typename T>
	void build(T &a, int32_t v, int32_t l, int32_t r) {
		tl[v] = l;
		tr[v] = r;
		if (l == r) {
			t[v] = a[l];
			t[v].id = l; // ****NOTE: Changed line to store index
			return;
		}
		else {
			int32_t tm = (l + r) / 2;
			build(a, v * 2, l, tm);
			build(a, v * 2 + 1, tm + 1, r);
			t[v].merge(t[v * 2], t[v * 2 + 1]);
		}
	}
	NODE query(int32_t v, ll l, ll r) {
		if (l > tr[v] || r < tl[v])
			return zero;
		if (l <= tl[v] && tr[v] <= r) {
			return t[v];
		}
		pushdown(v);
		NODE a, b, ans;
		a = query(v * 2, l, r);
		b = query(v * 2 + 1, l, r);
		ans.merge(a, b);
		return ans;
	}
	void rupd(int32_t v, ll l, ll r, UPDATE &val) {
		if (l > tr[v] || r < tl[v])
			return;
		if (l <= tl[v] && tr[v] <= r) {
			apply(v, val);
			return;
		}
		pushdown(v);
		rupd(v * 2, l, r, val);
		rupd(v * 2 + 1, l, r, val);
		t[v].merge(t[v * 2], t[v * 2 + 1]);
	}
	template <typename T>
	ll descent_right(ll l, T x, int32_t v, NODE &prev) {
		if (l > tr[v]) // No overlap
			return leng;
		if (l <= tl[v])	{ // Complete overlap
			NODE cur;
			cur.merge(prev, t[v]);
			if (!cur.check(x)) { // Going further right than current node
				swap(prev, cur); // Including this node's contribution
				return leng;
			}
			if (tl[v] == tr[v]) {
				return tr[v];
			}
		}
		pushdown(v);
		ll ans = descent_right(l, x, v * 2, prev); // Finding in left child
		if (ans != leng) // Found in left child
			return ans;
		return descent_right(l, x, v * 2 + 1, prev); // Finding in right child
	}
	template <typename T>
	ll descent_left(ll r, T x, int32_t v, NODE &prev) {
		if (r < tl[v]) // No overlap
			return -1;
		if (r >= tr[v]) { // Complete overlap
			NODE cur;
			cur.merge(t[v], prev);
			if (!cur.check(x)) { // Going further right than current node
				swap(cur, prev); // Including node's contribution
				return -1;
			}
			if (tl[v] == tr[v]) {
				return tl[v];
			}
		}
		pushdown(v);
		ll ans = descent_left(r, x, v * 2 + 1, prev); // Finding in right child
		if (ans != -1) // Found in right child
			return ans;
		return descent_left(r, x, v * 2, prev); // Finding in left child
	}

	ll leng = SZ;
	void clear() {
		fill(t, t + 4 * leng, zero);
		fill(lazy, lazy + 4 * leng, false);
		fill(upds, upds + 4 * leng, noop);
		built = false;
	}
	template <typename T>
	void build(T &a) {
		build(a, 1, 0, leng - 1);
		built = true;
	}
	template <typename T>
	ll descent_right(ll l, T x) { 
		// Moves left to right, returns segtree.leng if not found
		NODE prev = zero;
		return descent_right(l, x, 1, prev);
	}
	template <typename T>
	ll descent_left(ll r, T x) { 
		// Moves right to left, returns -1 if not found
		NODE prev = zero;
		return descent_left(r, x, 1, prev);
	}
	NODE query(ll l, ll r) {
		if (!built)
			build(t);
		return query(1, l, r);
	}
	void rupd(ll l, ll r, UPDATE val) {
		if (!built)
			build(t);
		rupd(1, l, r, val);
	}
};
/*
KEYNOTES:
	1. merge(X,identity_element) = X (for nodes other than leaf node)
	2. identity_transformation.combine(X) = X (combining identity transformation with update)
	3. NOTE: older_update.combine(newer_update) (how updates will happen)
*/

// Example: In Addition the identity element is 0, 
//          i.e., -> a + 0 = a or 0 + a = a
//          Also: min(x,INT_MAX) = x

#define node node1
#define update update1

// Structure of each node in the segment tree
struct node {
	// Initialised with the identity element
	ll v = 0, id = 0;
	// Add more variables if needed
	node() {}
	node(ll val) { // Building the tree
		v = val;
	}
	void merge(node &l, node &r) {
		// v = l.v + r.v;
		// For maximum we'll do
		v = max(l.v, r.v);
		if(v == l.v) id = l.id;
		else id = r.id;
	}
	bool check(ll x) { // Gradient Descent
		// if (x <= v)
		// 	return true;
		return false;
	}
};

// Example: In Addition the identity transformation is 0, 
//          i.e., > old_update = new_update + 0;

struct update {
	// Initialised with the identity transformation
	ll v = 0;
	// Add more variables if needed
	update() {}
	update(ll val) {
		v = val;
	}
	// Combining old update with new updates
	void combine(update &other, int32_t tl, int32_t tr)	{
		v += other.v;
		// Only required for range updates
	}
 // Applying lazy values on nodes
	void apply(node &x, int32_t tl, int32_t tr)	{
		x.v += v;
		// Remember (tr - tl + 1) * v
	}
};

segtree<node, update> s;

#undef node
#undef update

// s.leng = n;
// s.clear();
// vector<ll> v(n); // segtree vector
// s.build(v);

// Single element: s.rupd(index, index, value);
// Range Update: s.rupd(start, end, value);
// Query: s.query(start, end).v
// Descent: s.descent_right(index, value); -> Left to right
//       -> Returns segtree.leng if not found
//          s.descent_left(index, value);  -> Right tp left
//       -> Returns -1 if not found

// NOTE: All indexing is 0 based


vector<ll> v(SZ), dep(SZ);

ll rec(ll start, ll end, ll d) {
	if(start > end) {
		return 0;
	}
	ll id = s.query(start, end).id;
	dep[id] = d;
	rec(id + 1, end, d + 1);
	rec(start, id - 1, d + 1);
	return 0;
}

int main() { 
	ios::sync_with_stdio(0); cin.tie(0); cout.tie(0);
	#ifndef ONLINE_JUDGE
		// freopen("input.txt", "r", stdin) ;
		// freopen("output.txt", "w", stdout) ;
	#endif 
	ll T;
	cin >> T;
	while(T--) {	
		ll n;
		cin >> n;
		s.leng = n;
		s.clear();
		for(ll i = 0 ; i < n ; i++) 
			cin >> v[i];
		s.build(v); 
		// NOTE:Changed a line in build function to 
		// to store index along with value

		rec(0ll, n - 1, 0ll);
		
		for(ll i = 0 ; i < n ; i++)
			cout << dep[i] << " ";
		cout << '\n';
	}
	return 0;
}
