# multithreaded-matrix-multiplication
Parallel Dense Matrix Multiplicationü

In this project, a study was conducted on matrix multiplication methods and their differences and the methods were compared in terms of time.

## Comparison Table and Output Results

According to the results obtained, it is seen that as the size of the matrix increases, loading matrix time and computing time values also increase. When matrix multiplication, matrix A and matrix B are multiplied and a C matrix is obtained. In the serial matrix multiplication algorithm, n^3 multiplication and n^3 addition are performed. This means that the computation time will take too long. In general, serial computing time seems to be the most time in the time taken to calculate matrices.

In using the cache matrix multiplication, the traditional series matrix multiplication is optimized. The transpose of the B matrix is taken. Matrix A and matrix the transpose of matrix B is multiplied. As shown in the computation time values, this method works faster than series matrix multiplication, especially as matrix size increases.

In addition, matrix multiplication was recalculated using the cache and data decomposition technique. First, the code was run using 1 thread, then 2, 4, 8, and 16 threads. In some cases, the cache and decomposition technique works faster than caching and in some cases, it works slower. Generally, it is thought that as the number of threads increases, the elapsed time will decrease continuously. However, while the elapsed time decreases due to the increase in the number of threads, it starts to increase after a while. Computers have an optimum number of threads. The optimum value is the time when the calculated time decreases the most.

![Comparison Table](https://github.com/tugbaguler/multithreaded-matrix-multiplication/blob/main/assets/Comparison_table.jpg)

The working speeds of the algorithms according to the matrix dimensions are shown in the table above.
The larger the matrix size, the longer the calculation times. Now let's compare the 3 techniques used according to the data obtained.

### Comparison with serial matrix multiplication and using the cache:

Looking at the 2048 X 2048 matrix and the 4096 X4096 matrix, we see that the seral computing time is very high. With the caching method used, this calculation time is greatly shortened.
When we look at the smaller matrices, it is observed that the calculation times are equal to each other or the time calculated using the cache is a little shorter, that is, faster. But the difference between them appears more clearly in large size matrices. So calculating using cache will give better results than serial.

### Comparison with serial matrix multiplication and using cache and decomposition technique:

The difference between them is also seen more clearly in the large-sized matrix. Here, separate calculations were made using 5 different threads and the results were compared. Using thread calculates much faster than serial.
Looking at the results, when using 8 and 16 threads, the calculation time increases a little more, so it runs slower. It would be more advantageous to use 1 or 2 threads.

### Comparison with using the only cache and using cache and decomposition technique:

Both of them work very fast compared to serial. But when we compare it among themselves, it seems that the use of the only cache is fast enough. In cases where 1 thread is used, the calculation time has decreased a little more. Using 2 threads gives approximately the same values as the calculation obtained using only the cache. When 4 threads are used, the time taken for calculation has increased a little more. The time taken for calculation increases even more when using 8 and 16 threads.
In this case, increasing the number of threads also increases the elapsed time. When we consider the effort spent in coding, the most optimum solution for this computer is to make calculations using the only cache. Because, when the thread is used, a significant shortening of the time could not occur in the results obtained.
