## 5.2
The induction used to find a k-core fails at the inductive step for finding a maximum-degree maximal induced subgraph. When finding a k-core, we take advantage of the following property:

**If a vertex has degree `< k` in graph `G`, it has degree `< k` in all subgraphs of `G`**

Since the inverse/converse of this property is not true (for any vertex of degree `>= k`, there is an induced subgraph where it has degree `< k`), the inductive step in finding a k-core cannot be applied to finding a maximum-degree MIS.

## 5.3
If `S` is finite, it is empty after finding automorphisms if and only if it was empty to start with.

If `S` is empty before running the algorithm, it is trivial to show that it is empty after the algorithm finishes.

Suppose `S` is not empty before running the algorithm.

If there exists an element `x` that maps onto itself, the element is part of an automorphism, so the algorithm will not return an empty set.

If no such element exists, we mark an arbitrary element `x`, then look at the value `x` is mapped to, repeating this process. Because `S` is finite, we must eventually reach an element we have already marked (which we call `y`).
It follows that every element visited after (and including) `y` is part of an automorphism, so the algorithm will not return an empty set.

QED

## 5.6
See provided implementation

## 5.11
Given two generated skylines, the intersection can be calculated by modifying the "merge" portion of the algorithm to take the lower of the skyline, instead of the higher.

## 5.12
To find the maximum product, we modify Kadane's algorithm to keep track of both the greatest positive value and the lowest negative value.
For each index i, we consider the following three values: `mostPositive[i-1] * input[i]`, `mostNegative[i-1] * input[i]`, input[i]`.
One of these is the most positive consecutive product ending at index i, and one is the most negative.
We can then scan the mostPositive array for the maximum consecutive product in the whole array.

We increase each iterative step by at most a constant, so the runtime is still жи(n).

## 5.13
**Inductive hypothesis**: We know how to determine if each child of a vertex `v` is an AVL node with only AVL nodes as descendants ("accepted AVL node"), marked, or neither, as well as the height of the subtrees rooted at each child.

The height of the tree rooted at v is max(left subtree, right subtree) + 1

If either of v's children is not an accepted AVL node (or null), v is categorized as "neither".

If both of v's children are accepted AVL nodes (or null), and the difference in subtree heights is <= 1, then v is categorized as an accepted AVL node.

If both of v's children are accepted AVL nodes (or null), but the difference in subtree heights is > 1, then we mark v.

## 5.14
It may be difficult to determine the root of a binary tree given in adjacency list form, but we can easily find a leaf: they have degree 1.

**Inductive hypothesis**: We know how to find the distance between all vertices of a binary tree with size `n-1`.

Suppose we have a tree of size `n`. If we peel off a leaf `x``(call its parent `y`), we have a tree of size `n-1`. Once we find the distance between all vertices in this tree, the distance from any of them to `x` is the distance to `y` + 1.

We need to fill an entire n * n matrix, but it takes constant time to calculate one entry (distance to parent + 1, or 0 if distance to self), while the process of "peeling off" leaves can be done in linear time. Hence this is an жи(n^2) algorithm.

## 5.15
Recursively find the diameter of each subtree and subtree height
Subtree height = max(left subtree height, right subtree height) + 1
Diameter = max(left diameter, right diameter, (left subtree height + 1) + (right subtree height + 1))
Note: If subtree is empty (does not exist), use 0 instead of (height + 1) in third value

T(n) = 2T(n/2) + O(1) -> O(n)

## 5.16
It is possible to use O(K) space by storing the last added item at a given capacity instead of just a single flag. When we reconstruct the solution, we start with the last entry and work backwards until we reach 0, adding each element that we find.

## 5.17
For each element of size x, look at the array in groups of x. Mark indices that have been occupied; starting with index x, mark the corresponding index (mod x) with element x.

## 5.20
Note that this problem is essentially asking us to solve the knapsack problem for value `S/2`.


