print "calling one function inside another function"

def fun1():
 print 5+1
 
def fun2():
 print "8"
 fun1()

fun2()
