import sys
import os
fileno = sys.argv[1]
for i in range(int(fileno)):
    i=i+1
    f = open("./test_result/"+sys.argv[2]+"_"+str(i)+".txt")
    lines = f.readlines()

    sys.stdout.writelines(lines[-1:])
    result = lines[-1:]
    result2=result[0]
    f.close()

    w= open("./test_result/"+sys.argv[2]+"_result.txt","a+")


    # w= open("./test_result/"+sys.argv[2]+"_result.txt","w")
    w.write(result2[:15])
    w.write("\n")

    w.close()
