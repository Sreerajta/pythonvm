def fact(n):
    if n == 0:
    	return 1
    return n * fact(n - 1)



n=1
print "printing factorial of numbers upto 10 using a recursive function and a while loop"
while(n<=10):
 print fact(n)
 n=n+1
