# explanations for member functions are provided in requirements.py
from __future__ import annotations

class FibNode:
    def __init__(self, val: int):
        self.val = val
        self.parent = None
        self.children = []
        self.flag = False

    def get_value_in_node(self):
        return self.val

    def get_children(self):
        return self.children

    def get_flag(self):
        return self.flag

    def __eq__(self, other: FibNode):
        return self.val == other.val

class FibHeap:
    def __init__(self):
        # you may define any additional member variables you need
        self.roots = []
        pass

    def get_roots(self) -> list:
        return self.roots

    def insert(self, val: int) -> FibNode:
        pass
        
    def delete_min(self) -> None:
        pass

    def find_min(self) -> FibNode:
        pass

    def decrease_priority(self, node: FibNode, new_val: int) -> None:
        pass

    # feel free to define new methods in addition to the above
    # fill in the definitions of each required member function (above),
    # and for any additional member functions you define
