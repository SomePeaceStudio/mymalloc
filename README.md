# myalloc (My memory allocation)
This is testing tool for various memory allocation algorithms. Program runs different algorithms against input data and outputs calculated [fragmentation][frag]. 

Algorithms used:
  - BestFit
  - WorstFit
  - FirstFit
  - NextFit
# Compile:
```sh
$ gcc main.c -o main
```
# Run:
```sh
$ main.c -c <chunks.txt> -s <sizes.txt>
```
Where chunks.txt are free chunk sizes available in memory and sizes.txt are memory sizes to be allocated. Examples can be found in **mem-frag-tests-*** folders.
Output is **stdout** (it can be redirected with ">" to output in file, example: main.c -c chunks.txt -s sizes.txt > results.txt).

[frag]: https://en.wikipedia.org/wiki/Fragmentation_(computing)
