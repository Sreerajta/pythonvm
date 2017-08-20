import os
import sys
import filecmp
os.system("sh test.sh")

success=0
failed=0
files=["t1","t2","t3","t4","t5","t6","t7","t8","t9","t10"]
FL=[]
for file in files:
 f1="testcase/"+file+".expected"
 f2="testcase/"+file+".pyc.out"
 a=filecmp.cmp(f1,f2)
 if a==True:
     success+=1
 else:
     failed+=1
     FL+=[file,]

total=success+failed
print "A total of "+str(total)+" cases tested"
print "Passed test cases: "+str(success)
print "Failed test cases: " +str(failed)
print "Failures: " + str(FL) 
    
