# OMP Parallel pseudo-rand test

## Result

![result](./docs/parallel-rand.png)

- p1 (Parallel 1)
```cpp
#pragma omp parallel for
for (i=0;i<len;i++){
  std::mt19937 mt(i);
  array[i] = dist(mt);
  ...
}
```

- p2 (Parallel 2)
```cpp
#pragma omp parallel
{
  std::mt19937 mt(omp_get_thread_num());
  for (i=omp_get_thread_num();i<len;i+=omp_num_threads()) {
    array[i] = dist(mt);
    ...
  }
}
```

## License
MIT
