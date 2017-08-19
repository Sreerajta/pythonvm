def factorial(n):
 fact=1
 if n==0:
  return fact
 else:
  while(n>1):
   fact=fact*n
   n=n-1
  return fact 



print "function to print factorial of given number:"

print "5 factorial is"
print factorial(5) 

print "3 factorial is"
print factorial(3)

print "10 factorial is"
print factorial(10)


print "0 factorial is"
print factorial(0)
