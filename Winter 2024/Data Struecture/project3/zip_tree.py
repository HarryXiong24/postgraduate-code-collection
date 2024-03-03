# explanations for member functions are provided in requirements.py
# each file that uses a Zip Tree should import it from this file.

import json
import random
from typing import List, Optional, TypeVar

KeyType = TypeVar("KeyType")
ValType = TypeVar("ValType")


class TreeNode:
    def __init__(self, key: KeyType, val: ValType, rank: int):
        self.key: KeyType = key
        self.val: ValType = val
        self.rank: int = rank
        self.left: Optional[TreeNode] = None
        self.right: Optional[TreeNode] = None

    def __str__(self):
        return f"TreeNode(key={self.key}, val={self.val}, rank={self.rank}, left={self.left}, right={self.right}\n)\n---"


class ZipTree:
    def __init__(self):
        self.root: Optional[TreeNode] = None
        self.size = 0

    @staticmethod
    def get_random_rank() -> int:
        rank = 1
        # 50% chance to increase rank, 50% chance to stop
        while random.randint(0, 1) == 0:
            rank += 1
        return rank

    def insert(self, key: KeyType, val: ValType, rank: int = -1):
        if rank == -1:
            rank = self.get_random_rank()

        new_node: TreeNode = TreeNode(key, val, rank)

        if not self.root:
            self.root = new_node
            self.size += 1
            print(self.root)
            return

        parent = None
        current = self.root
        left_path: List[TreeNode] = []
        right_path: List[TreeNode] = []

        while current:
            if rank > current.rank or (rank == current.rank and key < current.key):
                if parent:
                    if parent.key > key:
                        parent.left = new_node
                    else:
                        parent.right = new_node
                else:
                    self.root = new_node

                new_node.left = current if key < current.key else None
                new_node.right = current if key > current.key else None

                self._find_path(current, key, left_path, right_path)

                new_node.left = self._build_tree_from_path(left_path, key, True)
                new_node.right = self._build_tree_from_path(right_path, key, False)
                self.size += 1
                print(self.root)
                return

            parent = current
            if key < current.key:
                current = current.left
            else:
                current = current.right

    def _find_path(
        self,
        start: TreeNode,
        key: KeyType,
        left_path: List[TreeNode],
        right_path: List[TreeNode],
    ) -> List[TreeNode]:
        current = start
        while current:
            if key < current.key:
                left_path.append(current)
                current = current.left
            else:
                right_path.append(current)
                current = current.right

    def _build_tree_from_path(self, path, key, is_left):
        root = None
        for node in reversed(path):
            if is_left and node.key > key:
                node.left = root
                root = node
            elif not is_left and node.key <= key:
                node.right = root
                root = node
        return root

    def remove(self, key: KeyType):
        pass

    def find(self, key: KeyType) -> ValType:
        current = self.root
        while current:
            if current.key == key:
                return current.val
            current = current.left if key < current.key else current.right
        print("find", self.root)
        return None

    def get_size(self) -> int:
        return self.size

    def get_height(self) -> int:
        """returns the height of the tree."""
        if not self.root:
            return 0
        queue = [self.root]
        height = 0
        while queue:
            for _ in range(len(queue)):
                node = queue.pop(0)
                if node.left:
                    queue.append(node.left)
                if node.right:
                    queue.append(node.right)
            height += 1
        return height

    def get_depth(self, key: KeyType):
        """returns the depth of the item with parameter key. you can assume that the item exists in the tree."""
        pass


# feel free to define new classes/methods in addition to the above
# fill in the definitions of each required member function (above),
# and any additional member functions you define
