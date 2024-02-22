# explanations for member functions are provided in requirements.py
from __future__ import annotations
from typing import List

class FibNodeLazy:
    def __init__(self, val: int):
        self.val: int = val
        self.parent: FibNodeLazy = None
        self.children: List[FibNodeLazy] = []
        self.flag: bool = False

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
        self.min = None       

    def get_roots(self) -> list:
        return self.roots

    def insert(self, val: int) -> FibNodeLazy:
        new_node = FibNodeLazy(val)
        self.roots.append(new_node)
        if self.min is None or new_node.val < self.min.val:
            self.min = new_node
        return new_node
    
    def delete_min_lazy(self) -> None:
        if not self.roots or self.min is None or self.min not in self.roots:
            return
        
        self.roots += self.min.children
        for child in self.min.children:
            child.parent = None
        self.roots.remove(self.min)
                           
        self.rebuild()

        # update the minimum node
        self.update_min_node()
        

    def find_min_lazy(self) -> FibNodeLazy:
        return self.min

    def decrease_priority(self, node: FibNodeLazy, new_val: int) -> None:
        if node in self.roots:
            node.val = new_val
            if node.val < self.min.val:
                self.min = node
            return 
        
        if node.parent is not None:
            node.val = new_val
            if node in node.parent.children:
                node.parent.children.remove(node)
            self.roots.append(node)
            parent = node.parent
            if not parent.flag:
                parent.flag = True
            else:
                grandparent = parent.parent
                if grandparent is not None:
                    if parent in grandparent.children:
                        grandparent.children.remove(parent)
                    self.roots.append(parent)
                    grandparent.flag = True
                    self.decrease_priority(grandparent, grandparent.val) # recursive call
                else:
                    self.roots.append(parent)
                    parent.flag = False
            parent = None

        # update the minimum node
        self.update_min_node()
        
    def rebuild(self):
        '''
        rebuild the roots of the heap so that no two roots have the same degree.
        '''
        table: dict[int, List[FibNodeLazy]] = {}
        
        while True:
            for root in self.roots:
                degree = len(root.children)
                if degree in table:
                    table[degree].append(root)
                else:
                    table[degree] = [root]
                    
            is_rebuild = False
            for degree, roots_with_same_degree in table.items():
                if len(roots_with_same_degree) >= 2:
                    is_rebuild = True
                    break
                
            if not is_rebuild:
                return 
            else:
                self.roots = []
                for degree, roots_with_same_degree in table.items():
                    while len(roots_with_same_degree) >= 2:
                        root1 = roots_with_same_degree.pop()
                        root2 = roots_with_same_degree.pop()
                        if root1.val < root2.val:
                            root1.children.append(root2)
                            root2.parent = root1
                            if root2 in self.roots:
                                self.roots.remove(root2)
                            self.roots.append(root1)
                        else:
                            root2.children.append(root1)
                            root1.parent = root2
                            if root1 in self.roots: 
                                self.roots.remove(root1)
                            self.roots.append(root2)
                
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
