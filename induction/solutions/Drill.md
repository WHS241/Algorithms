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

