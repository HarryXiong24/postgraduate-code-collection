# Write a function, bool isBalanced(String s), that returns true only if the parenthesis in parameter s are correctly balanced. Handle three types of parens: () {} []  You can ignore any other characters in s.
# isBalanced(“((([[[{{{}}}]]])))”); yields true
# isBalanced(“[[[]]]{{{}}}((()))”); yields true
# isBalanced(“((([[[{{{}}}]]]))”); yields false, missing )
# isBalanced(“[[[]]]{{{}}}((())))”); yields false, extra )
# isBalanced(“[{(}])”); yields false, its “crazy” mismatched

def isBalanced(s: str) -> bool:
    stack = []
    pair = {
        '(': ')',
        '[': ']',
        '{': '}',
    }

    if len(s) % 2 != 0:
        return False

    for item in s:
        if item in pair:
            stack.append(item)
        else:
            cur = stack.pop()
            if pair.get(cur) == item:
                continue
            else:
                return False
    return True


# test
res1 = isBalanced("((([[[{{{}}}]]])))")
res2 = isBalanced("[[[]]]{{{}}}((()))")
res3 = isBalanced("((([[[{{{}}}]]]))")
res4 = isBalanced("[[[]]]{{{}}}((())))")
res5 = isBalanced("[{(}])")
print(res1, res2, res3, res4, res5)
