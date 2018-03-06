# InfMaxHop version 1.1
This project implements the hop-based influence maximization algorithms for the following papers:

- Jing Tang, Xueyan Tang, Junsong Yuan, "[An Efficient and Effective Hop-Based Approach for Influence Maximization in Social Networks,](https://link.springer.com/article/10.1007%2Fs13278-018-0489-y)" in Social Network Analysis and Mining, vol. 8, no. 10, 2018.

- Jing Tang, Xueyan Tang, Junsong Yuan, "[Influence Maximization Meets Efficiency and Effectiveness: A Hop-Based Approach,](https://dl.acm.org/citation.cfm?id=3110025.3110041)" in Proc. IEEE/ACM ASONAM, 2017, pp. 64–71.

**Author: Jing Tang (Nanyang Technological University)**
> Cross-platform supported, including **\*nix** and **Windows** systems!\
Under **\*nix** system, compile and build codes:\
`g++ -std=c++14 -O3 -W3 InfMaxHop.cpp oneHop.cpp twoHop.cpp SFMT/dSFMT/dSFMT.c -o InfMaxHop1.1.o`\
Note: `c++1x` is required.

**_Before running influence maximization algorithms, please format the graph first!_**

Execute the command: `{your_exec} [options]`. `{your_exec}` may be `InfMaxHop1.1.o` under **\*nix** or `InfMaxHop1.1.exe` under **Windows**.

See some sample codes in **sample-code.sh** or **sample-code.bat**. For example,

For example,

- Format the graph with the WC setting: 

	`{your_exec} -func=0 -gname=facebook`

- Run OneHop algorithm to select 50 nodes under the IC model (diffusion probability is loaded directly from the file, which is the WC setting if the graph is formatted in the above procedure):

	`{your_exec} -func=1 -gname=facebook -alg=oneHop -seedsize=50`

- Run TwoHop algorithm to select 100 nodes under the LT-WC model:

	`{your_exec} -func=1 -gname=facebook -alg=twoHop -seedsize=100 -model=LT -pdist=WC`

- Run TwoHop algorithm without using upper bounding approach to select 100 nodes under the IC-TRI model:

	`{your_exec} -func=1 -gname=facebook -alg=twoHop -opt=0 -seedsize=100 -model=IC -pdist=TRI`

### Options
- **-func=integer**

	Specify the function to execute. Possible values:
  + **0**: Format the graph.
  + **1** *[default]*: Run influence maximization algorithms.
  + **2**: Format the graph and run influence maximization algorithms.
	
- **-dir=string**

	Specify the direction for the input files, e.g., **-dir=graphInfo** *[default]*.
    
- **-gname=string**
	
	Specify the graph name to process, e.g., **-gname=facebook** *[default]*.
    
- **-mode=string** *(works only when -func=0 or -func=2)*

  Specify how to format the graph file. Two types of original graph file are supported. The file should have *m+1* lines where the first line has two integers to indicate the number *n* of nodes and the number *m* of edges. The following *m* lines list the edges. The node is indexed from *0* to *n-1*. Each edge line has two integers to indicate the source node and target node.
	+ **-mode=g** *[default]*: Weighted Cascade (WC) setting is used where *p(u,v)=1/indeg(v)*. For example,
    	> 3 4\
    	0 1\
    	0 2\
    	1 0\
    	1 2
    
   + **-mode=w**: Each edge line can also follow with a positive number to indicate the diffusion probability. In this case, the diffusion probability is associated by the given value. For example,
    	> 3 4\
    	0 1 0.2\
    	0 2 0.3\
    	1 0 0.1\
    	1 2 0.4

	The following options are valid for *-func=1* or *-func=2*.

- **-outpath=string**

  Specify the folder to save results, e.g., **-outpath=result** *[default]*.

- **-alg=string**

  Specify the algorithm used for influence maximization. Possible values:
	+ **oneHop** *[default]*: OneHop algorithm.
	+ **twoHop**: TwoHop algorithm.

- **-opt=integer**

  Specify whether the improvement for TwoHop via an upper bounding approach is used. Possible values:
	+ **0**: Optimization is NOT used for TwoHop.
	+ **1** *[default]*: Optimization is used for TwoHop.
  
- **-seedsize=integer**

  Specify the number of nodes to be selected. The default value is *50*.
  
- **-model=string**
  
  Specify the cascade model. Possible values:
	+ **IC** *[default]*: Independent Cascade model.
	+ **LT**: Linear Threshold cascade model.
	
- **-pdist=string**

  Specify the diffusion probability. Possible values:
  + *[default]*: Load directly from the file if unspecified. Note that if the graph is formatted from the edge list without weights, **WC** setting is used already. So this parameter can be unspecified for the WC setting in this case. 
  + **WC**: Weighted Cascade (WC) setting, i.e., *p(u,v)=1/indeg(v)*. Support the IC and LT models.
  + **TRI**: Trivalency (TRI) setting, i.e., *p(u,v)* of each edge *(u,v)* is set by choosing a probability from the set *{0.1, 0.01, 0.001}* at random. Support the IC model. It may violate the LT model.
  + **UNI**: Uniform (UNI) setting, i.e., *p(u,v)* of each edge *(u,v)* is set to a given value specified by **-pedge=decimal** (default is *0.1*). Support the IC model. It may violate the LT model.
  
- **-pedge=decimal**

  Specify the diffusion probability for every edge under the *UNI* setting. The default value is *0.1*. 
