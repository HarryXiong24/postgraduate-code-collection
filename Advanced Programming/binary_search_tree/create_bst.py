from typing import List

class TreeNode:
    def __init__(self, val: str):
        self.val = val
        self.left = None
        self.right = None
  
def build(a: List[str]):
    if len(a) == 0:
        return None
    mid = len(a) // 2
    root = TreeNode(a[mid])
    root.left = build(a[:mid])
    root.right = build(a[mid + 1:])
    return root
  
# test
a = ['a', 'b', 'c', 'd', 'e', 'f', 'g']
root = build(a)