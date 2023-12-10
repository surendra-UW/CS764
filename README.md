# CS - 764

## Group 5:
Surendra Parla (smparla@wisc.edu) - 908 590 0984
Neha Chadaga (nchadaga@wisc.edu) - 908 587 6465
Srihari Sridharan (sridharan9@wisc.edu) - 908 575 4456

## Techniques Implemented in this Submission:
1. Quicksort
Source Files: internal_sort.cpp, internal_sort.h
Lines:

2. Tournament Trees
Source Files: TournamentTree.cpp, TournamentTree.h, TournamentTreeNode.h
Lines:

3. Offset-Value Coding
Source Files: TournamentTree.cpp, TournamentTree.h
Lines:

4. Minimum count of row and column comparisons
Source Files: TournamentTree.cpp, TournamentTree.h
Lines:

5. Cache-size mini runs
Source Files: TournamentTree.cpp, TournamentTree.h
Lines:

6. Device-optimized page sizes
Source Files: TournamentTree.cpp, TournamentTree.h
Lines:

7. Spilling memory-to-SSD
Source Files: TournamentTree.cpp, TournamentTree.h
Lines:

8. Spilling from SSD to disk
Source Files: TournamentTree.cpp, TournamentTree.h
Lines:

9. Verifying sets of rows & values
Source Files: test_scripts.cpp
Lines:

10. Verifying sort order 
Source Files: test_scripts.cpp
Lines:

## Justification of the choice of above techniques:
1. Quicksort - Quick sort is vital in internal sorting due to its efficient average-case time complexity, making it exceptionally fast for large datasets. Its divide-and-conquer strategy with low memory usage ensures swift sorting, often outperforming other algorithms like merge sort or insertion sort.

2. Tournament Trees - In external sorting, tournament trees serve as efficient structures to handle merging of sorted chunks from disk, reducing comparisons and improving the merge step's efficiency. They optimize the selection of the next smallest or largest element when merging sorted chunks from external memory, minimizing I/O operations and enhancing overall sorting performance.

3. Offset-Value Coding - 

4. Minimum count of row and column comparisons - Tree-of-losers priority queues are more efficient than the traditional and better known tree-of-winners priority queues because the former use only leaf-to-root. passes with 𝑙𝑜𝑔2𝑁 comparisons, whereas the latter also need root-to-leaf passes with up to 2×𝑙𝑜𝑔2𝑁 comparisons.

5. Cache-size mini runs - Spatial Locality avoiding cache miss

6. Device-optimized page sizes - Dont waste bandwidth

7. Spilling memory-to-SSD 

8. Spilling from SSD to disk - 

9. Verifying sets of rows & values - 

10. Verifying sort order - Verifying the sort order in external sorting ensures the accuracy and correctness of the sorting process, confirming that the sorted output meets the expected order and criteria, which is crucial for data integrity and error detection, especially when dealing with vast amounts of data distributed across multiple storage devices. It acts as a validation step, enhancing reliability and confidence in the sorting results, reducing the risk of errors or inconsistencies in the sorted data.

## Project's State:
Completed.

## Contributions:
Surendra Parla - Spilling from SSD to disk, Spilling memory-to-SSD, Cache-size mini runs, Device-optimized page sizes
Neha Chadaga - Quicksort, Verifying sets of rows & values, Verifying sort order
Srihari Sridharan - Tournament Tree, Offset Value Coding, Minimum count of row and column comparisons

