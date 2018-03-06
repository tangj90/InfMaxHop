REM Begin

SET exec=InfMaxHop1.1.exe
CALL %exec% -func=0 -gname=facebook
PAUSE
CALL %exec% -func=1 -gname=facebook -alg=oneHop -seedsize=50
CALL %exec% -func=1 -gname=facebook -alg=twoHop -seedsize=100 -model=LT -pdist=WC
CALL %exec% -func=1 -gname=facebook -alg=twoHop -opt=0 -seedsize=100 -model=IC -pdist=TRI
PAUSE