# I apply Array to simulate Stack
def isBalanced(s: str) -> bool:
    stack = []
    pairs = {
        "(": ")",
        "[": "]",
        "{": "}"
    }

    if len(s) % 2 != 0:
        return False

    for item in s:
        if (item in pairs):
            stack.append(item)
        else:
            cur = stack.pop()
            if item != pairs[cur]:
                return False

    return True

# Time complexity: O(n)
# Space complexity: O(n)


# test
res1 = isBalanced("((([[[{{{}}}]]])))")
res2 = isBalanced("[[[]]]{{{}}}((()))")
res3 = isBalanced("((([[[{{{}}}]]]))")
res4 = isBalanced("[[[]]]{{{}}}((())))")
res5 = isBalanced("[{(}])")
print(res1)
print(res2)
print(res3)
print(res4)
print(res5)
