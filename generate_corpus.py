import os
import shutil
import matplotlib.pyplot as plt

keywords = ["Oregon", "Competitive", "Services", "program", "LSTA", "federal", "allotment", "through", "Institute", "Museum",
"funds", "from", "State", "funded", "grant", "projects", "Library", "Technology", "Art", "and"]

metadata = open("metadata.txt", "r")

corpus = [500, 10000, 15000, 20000, 25000, 30000, 35000, 40000]

for i in corpus:
    file = open(str(i) + ".txt", "w")
    for j in range(i):
        file.write(metadata.readline())
    file.close()
    metadata.seek(0, 0)

metadata.close()



indexer_time = []
searcher_time = []

for i in corpus:
    # out_file_index = " >" + str(i) + "Index.txt"
    index_cmd = "./indexer " + str(i) + ".txt " + "database"
    r = os.popen(index_cmd)
    indexer_time.append(float(r.readline()))
    r.close()
    # out_file_search = " >" + str(i) + "Search.txt"
    # os.system("./searcher database 10 " + keywords[0] + " " + keywords[1])
    search_cmd = "./searcher database 10 " + keywords[0] + " " + keywords[1]
    r = os.popen(search_cmd)
    searcher_time.append(float(r.readline()))
    r.close()
    print("Size " + str(i) + " test finished")
    shutil.rmtree("database")


print(indexer_time)
print(searcher_time)

plt.plot(corpus, indexer_time)
plt.ylabel("indexer time")
plt.xlabel("corpus size")
plt.xlim(100, 45000)
plt.ylim(0, 30)
plt.title("Indexing time versus data size")
plt.savefig('assets/1.jpg')

plt.plot(corpus, searcher_time)
plt.ylabel("searcher time")
plt.xlabel("corpus size")
plt.xlim(100, 45000)
plt.ylim(0.1, 0.3)
plt.title("Search time versus data size")
plt.savefig('assets/2.jpg')


search_test_AND = []
search_test_OR = []

corpus_size = [2, 5, 10, 15, 20]

os.system("./indexer " + "40000.txt " + "database")

for i in corpus_size:
    search_cmd = "./searcher database 10 "
    for j in range(i):
        search_cmd += keywords[j] + " "
    r = os.popen(search_cmd)
    search_test_OR.append(float(r.readline()))
    r.close()

plt.plot(corpus_size, search_test_OR)
plt.ylabel("searcher time")
plt.xlabel("corpus size")
plt.xlim(2, 20)
plt.ylim(0.1, 0.3)
plt.title("Search time versus number of OR-keywords in the query")
plt.savefig('assets/4.jpg')

for i in corpus_size:
    search_cmd = "./searcher database 10 "
    for j in range(i):
        search_cmd += "+" + keywords[j] + " "
    r = os.popen(search_cmd)
    search_test_AND.append(float(r.readline()))
    r.close()

plt.plot(corpus_size, search_test_AND)
plt.ylabel("searcher time")
plt.xlabel("corpus size")
plt.xlim(2, 20)
plt.ylim(0.1, 0.3)
plt.title("Search time versus number of AND-keywords in the query")
plt.savefig('assets/3.jpg')

print(search_test_AND)
print(search_test_OR)

shutil.rmtree("database")
