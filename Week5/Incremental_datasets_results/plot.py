# First networkx library is imported  
# along with matplotlib 
import networkx as nx 
import matplotlib.pyplot as plt 

G = nx.DiGraph()
f=open("input5.in")
dim=f.readline().split(' ')
w,d,k=int(dim[0]),int(dim[1]),int(dim[2])

mp={"0":'red',"1":'blue',"2":'green'}
assnmnts=open("Cl_Assnmnts.txt")
assnmnts.readline()
labels=assnmnts.readline().replace(' ','')
labels=labels[0:len(labels)-1]

while(1):
	cur_edge=f.readline()
	if(cur_edge==''):
		break	
	st=cur_edge.split(' ')
	u,v,we=int(st[0]),int(st[1]),float(st[2][0:len(st[2])-1])
	G.add_edge('w'+str(u),'d'+str(v),weight=we)
	
color_map=list()
n_list=G.nodes
for x in n_list:
	num=int(x[1:len(x)])
	if x[0]=='d':	
		color_map.append(mp[labels[w+num]])
	else:
		color_map.append(mp[labels[num]])

pos={}
D0,W0,D1,W1,D2,W2=(0,0),(1,0),(2,0),(3,0),(4,0),(5,0)
for x in n_list:
	num=int(x[1:len(x)])
	if x[0]=='d' and labels[w+num]=="0":	
		pos[x]=D0
		t=list(D0)
		t[1]=D0[1]+1
		D0=tuple(t)
	if x[0]=='w' and labels[num]=="0":	
		pos[x]=W0
		t=list(W0)
		t[1]=W0[1]+1
		W0=tuple(t)
	if x[0]=='d' and labels[w+num]=="1":	
		pos[x]=D1
		t=list(D1)
		t[1]=D1[1]+1
		D1=tuple(t)
	if x[0]=='w' and labels[num]=="1":	
		pos[x]=W1
		t=list(W1)
		t[1]=W1[1]+1
		W1=tuple(t)
	if x[0]=='d' and labels[w+num]=="2":	
		pos[x]=D2
		t=list(D2)
		t[1]=D2[1]+1
		D2=tuple(t)
	if x[0]=='w' and labels[num]=="2":	
		pos[x]=W2
		t=list(W2)
		t[1]=W2[1]+1
		W2=tuple(t)

#print(pos)


nx.draw(G,pos=pos,node_color=color_map,with_labels = True)
plt.savefig("filename.png")
