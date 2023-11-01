def traverse(s: str):
    left = 0
    right = len(s)-1

    arr = list(s)

    while left < right:
        temp = arr[left]
        arr[left] = arr[right]
        arr[right] = temp
        left = left + 1
        right = right - 1

    return ''.join(arr)


# test
s = "asdfghj"
res = traverse(s)
print(res)
