class TreeNode:
    def __init__(self) -> None:
        self.key = None
        self.left = None
        self.right = None


def copy(t: TreeNode) -> TreeNode:

    def recursive(node: TreeNode):
        if not node:
            return None
        newNode = TreeNode(node.key)
        newNode.left = recursive(node.left)
        newNode.right = recursive(node.right)
        return newNode

    return recursive(t)
