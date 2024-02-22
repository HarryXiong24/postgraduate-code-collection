# explanations for member functions are provided in requirements.py
from __future__ import annotations
from typing import List


class FibNode:
    def __init__(self, val: int):
        self.val: int = val
        self.parent: FibNode = None
        self.children: List[FibNode] = []
        self.flag: bool = False

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
        '''
        initializes the Fibonacci heap as an empty heap with no nodes. We have predefined a few variables for you. Feel free to use them or define your own.
        '''
        # you may define any additional member variables you need
        self.roots: List[FibNode] = []
        self.min = None        
        pass
    
    def get_roots(self) -> list:
        '''
        will be used to check the correctness of the heap structure. It returns a list of all the root nodes in the heap. 
        '''
        return self.roots
    
    def insert(self, val: int) -> FibNode:
        '''
        insert an item with the specified value to the Fibonacci heap. you want to return the node that you created.
        
        you can assume that the value is not already in the heap.
        '''
        new_node = FibNode(val)
        self.roots.append(new_node)
        if self.min is None or new_node.val < self.min.val:
            self.min = new_node
        return new_node

        
    def delete_min(self) -> None:
        '''
        deletes the minimum node from the Fibonacci heap. you can assume that the heap is non-empty when this is called.
        '''
        if self.min not in self.roots:
            return
        
        self.roots += self.min.children
        for child in self.min.children:
            child.parent = None
        self.roots.remove(self.min)
                           
        self.rebuild()

        # update the minimum node
        self.update_min_node()

    def find_min(self) -> FibNode:
        '''
        returns the node with the minimum value in the Fibonacci heap. you can assume that the heap is non-empty when this is called.
        '''
        return self.min

    def decrease_priority(self, node: FibNode, new_val: int) -> None:
        '''
        decreases the priority of the specified node to the new value. you can assume that the new value is less than the current value and that it will not decrease to a pre-existing priority in the heap.
        '''            
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
        table: dict[int, List[FibNode]] = {}
        
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
        if self.roots and len(self.roots) > 0:
            self.min = min(self.roots, key=lambda x: x.val)
        else:
            self.min = None
    
    def print_fib_node(self, obj: List[FibNode]) -> None:
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
