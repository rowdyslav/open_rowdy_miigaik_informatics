a, b = 56, 98
while a != b:
    if a > b:
        a -= b
    else:
        b -= a
print("GCD:", a)
