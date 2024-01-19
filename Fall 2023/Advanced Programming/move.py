from typing import List


def cal(arr: List[int]):
    firstRound = [];
    secondRound = [];
    
    for i in range(0, len(arr)):
        total = 1
        for j in range(i + 1, len(arr)):
            total = total * arr[j]
        firstRound.append(total)
    
    for i in range(len(arr), -1, -1):
        total = 1
        for j in range(i - 1, -1, -1):
            total = total * arr[j]
        secondRound.insert(0, total)
        
        
    print(firstRound)
    print(secondRound)
    
# test
res = cal([10,3,5,6,2])
