#!/bin/bash
echo Begin

cmd="./InfMaxHop1.1.o"
${cmd} -func=0 -gname=facebook
read -rsp $'Press any key to continue...\n' -n 1 key
${cmd} -func=1 -gname=facebook -alg=oneHop -seedsize=50
${cmd} -func=1 -gname=facebook -alg=twoHop -seedsize=100 -model=LT -pdist=WC
${cmd} -func=1 -gname=facebook -alg=twoHop -opt=0 -seedsize=100 -model=IC -pdist=TRI
