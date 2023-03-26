# Text-index-and-search


##### Compile
```
cmake .
make
```

##### Execute
```
./indexer <dataset_metadata> <index_name>
./searcher <index_name> <top-k> <keyword1> <keyword2> ...
./ssearch <index_name>
```

**Add '+' in front of keywords to execute AND search**

![1](./assets/1.jpg)

![2](./assets/2.jpg)

![3](./assets/3.jpg)

![4](./assets/4.jpg)

![5](./assets/top-K search time versus K.png)

![6](./assets/topk_search_time_vs_cardinality.png)
