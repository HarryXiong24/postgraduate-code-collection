# explanations for member functions are provided in requirements.py
from __future__ import annotations
from typing import List


class FibNodeLazy:
    def __init__(self, val: int):
        self.val: int = val
        self.parent: FibNodeLazy = None
        self.children: List[FibNodeLazy] = []
        self.flag: bool = False
        self.isAvailable: bool = False

    def get_value_in_node(self):
        return self.val

    def get_children(self):
        return self.children

    def get_flag(self):
        return self.flag

    def __eq__(self, other: FibNodeLazy):
        return self.val == other.val


class FibHeapLazy:
    def __init__(self):
        # you may define any additional member variables you need
        self.roots: List[FibNodeLazy] = []
        self.size: int = 0
        self.min = None

    def get_roots(self) -> list:
        return self.roots

    def insert(self, val: int) -> FibNodeLazy:
        new_node = FibNodeLazy(val)
        self.roots.append(new_node)
        if self.min is None or new_node.val < self.min.val:
            self.min = new_node
        self.size += 1
        return new_node

    def delete_min_lazy(self) -> None:
        if not self.roots or self.min is None:
            return

        # remove the min node from the root l
        min_node = self.find_min_lazy()
        min_node.isAvailable = True  # Mark the minimum node as deleted
        self.size -= 1

    def find_min_lazy(self) -> FibNodeLazy:
        if self.min is None:
            self.rebuild_heap()
        return self.min

    def rebuild_heap(self) -> None:
        new_roots = []
        new_min = None
        for root in self.roots:
            self.destroy_vacant_nodes(root)  # 销毁所有空闲节点
            if not root.isAvailable:  # 如果根节点非空闲，则将其添加到新根节点列表中
                new_roots.append(root)
                if new_min is None or root.val < new_min.val:
                    new_min = root
        self.roots = new_roots
        self.min = new_min  # 更新最小节点

    def destroy_vacant_nodes(self, node: FibNodeLazy) -> None:
        if node is None or not node.isAvailable:
            return

        if node in self.roots:
            self.roots.remove(node)
        else:
            node.parent.children.remove(node)

        for child in node.children:
            if child is not None:
                self.destroy_vacant_nodes(child)

    def decrease_priority(self, node: FibNodeLazy, new_val: int) -> None:
        node.val = new_val
        if node in self.roots:
            if node.val < self.min.val:
                self.min = node
            return

        self.promote(node)

        # update the minimum node
        self.update_min_node()

    def promote(self, node: FibNodeLazy) -> None:
        """
        promote the node to the root list
        """
        if node not in self.roots:
            parent = node.parent
            if parent is not None:
                parent.children.remove(node)
                node.parent = None
                self.roots.append(node)
                if parent.flag:
                    self.promote(
                        parent
                    )  # Promote the parent if it has already lost a child.
                else:
                    if (
                        parent not in self.roots
                    ):  # Only set the flag if the parent is not a root.
                        parent.flag = True

    def update_min_node(self):
        if not self.roots or len(self.roots) == 0:
            self.min = None
        else:
            self.min = min(self.roots, key=lambda x: x.val)

    def print_fib_node(self, obj: List[FibNodeLazy]) -> None:
        if isinstance(obj, list):
            for item in obj:
                self.print_fib_node(item)
        else:
            print(obj.val)
            for child in obj.children:
                self.print_fib_node(child)

    # feel free to define new methods in addition to the above
    # fill in the definitions of each required member function (above),
    # and for any additional member functions you define
