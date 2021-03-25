
'''
    Code to generate incremental datasets from MED_CRAN_CISI
'''

#modify hyperparameters 
batch_size = 70
n_batches = 25
want_randomization = True

#output file paths
f = open("../input.in","w")
h = open("../labels.in","w")

#To test the correctness of generated files using kpartition.cpp
g = open("kpart.in", "w")

#input file paths
file = open("labels.in")
input = open("input.in")


initial_size = 3891 - (n_batches*batch_size)
if(initial_size <= 0):
    exit(0)

# you should not have to change anything below this
###############################################################


line = file.readline()
labels = [int(i) for i in (line.split())]
adj = [[] for i in range(len(labels))]


data = input.readline().split()
print(data)
lines = input.readlines()
for line in lines:
    y = line.split()
    w = int(y[0])
    d = int(y[1])
    wt = float(y[2])
    adj[d].append((w,wt))
#print(adj)



        



import random
perm  = [i for i in range(len(labels))]
if(want_randomization):
    random.shuffle(perm)


def num_of_edges(l,r):
    ctr = 0
    for i in range(l,r):
        ctr = ctr + len(adj[perm[i]])
    return ctr


g.write(data[0] + " " + data[1] + " " + data[2] + "\n"  )
f.write("{} {} {} {}\n".format(int(data[0]),initial_size,3,num_of_edges(0,initial_size))) 

print(num_of_edges(0,initial_size))
total = 0
ctr = 0
for i in range(0,initial_size):
    for j in adj[perm[i]]:
        f.write("{} {} {}\n".format(j[0], perm[i], j[1]))
        g.write("{} {} {}\n".format(j[0], perm[i], j[1]))
        ctr += 1
total += ctr
print(ctr)

for i in range(n_batches):
    l = initial_size + i*batch_size
    r = initial_size + (i+1)*batch_size
    f.write("{} {} {}\n".format(batch_size, 0, num_of_edges(l,r)))
    print(num_of_edges(l,r))
    for i in range(l,r):
        f.write("{} {}\n".format(perm[i],1))
    ctr = 0
    for i in range(l,r):
        for j in adj[perm[i]]:
            f.write("{} {} {}\n".format(j[0],perm[i],j[1]))
            g.write("{} {} {}\n".format(j[0],perm[i],j[1]))
            ctr += 1
    print(ctr)
    total += ctr
f.close()
g.close()
print("Total = " + str(total + n_batches + 1 + 2000))


for i in range(len(labels)):
    h.write("{} ".format(labels[i]))
h.close()






