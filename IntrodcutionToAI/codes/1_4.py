age = int(input("Enter your age: "))
if 0 < age and age <= 20:
    print('teenager')
elif 20 < age and age <= 40:
    print('Man')
elif 40 < age and age <= 60:
    print('Old')
else:
    print('Older')
