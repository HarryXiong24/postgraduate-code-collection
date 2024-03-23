# explanations for member functions are provided in requirements.py
# each file that uses a skip list should import it from this file.

from typing import List


class PrefixTreeNode:
    def __init__(self, label: str = ""):
        self.children = {}
        self.is_end_of_word = False
        self.label = label

    def __str__(self) -> str:
        print(f"children: {self.children}")
        print(f"is_end_of_word: {self.is_end_of_word}")
        print(f"label: {self.label}")


class SuffixTreeNode:
    def __init__(self, label=""):
        self.label = label
        self.children = {}
        self.is_end_of_word = False

    def recursive_str(self, children: dict) -> str:
        for key, child in children.items():
            print(f"{key, child}")
            child.recursive_str(child.children)

    def __str__(self) -> str:
        return self.recursive_str(self.children)


class Trie:

    # Trie data structure class
    def __init__(self, is_compressed: bool):
        self.prefix_root = PrefixTreeNode()
        self.suffix_root = SuffixTreeNode()
        self.is_compressed = is_compressed
        self.is_prefix_tree = True

    def construct_trie_from_text(self, keys: List[str]) -> None:
        # TODO
        self.is_prefix_tree = True
        for key in keys:
            node = self.prefix_root
            for char in key:
                if char not in node.children:
                    node.children[char] = PrefixTreeNode(char)
                node = node.children[char]
            node.is_end_of_word = True

        if self.is_compressed:
            self.prefix_compress(self.prefix_root)

    def prefix_compress(self, node: PrefixTreeNode, parent=None, char=None) -> None:
        while len(node.children) == 1 and not node.is_end_of_word:
            next_node = next(iter(node.children.values()))
            node.label += next_node.label  # Concatenate labels
            node.children = next_node.children
            node.is_end_of_word = next_node.is_end_of_word
            if parent:
                parent.children[char] = node
        for char, child in list(node.children.items()):
            self.prefix_compress(child, node, char)

    def construct_suffix_tree_from_text(self, keys: List[str]) -> None:
        # TODO
        self.is_prefix_tree = False
        for key in keys:
            for i in range(len(key)):
                suffix = key[i:]
                node = self.suffix_root
                for char in suffix:
                    if char not in node.children:
                        node.children[char] = SuffixTreeNode(char)
                    node = node.children[char]
                node.is_end_of_word = True

        if self.is_compressed:
            self.suffix_compress(self.suffix_root)

    def suffix_compress(self, node: SuffixTreeNode) -> None:
        keys_to_delete = []
        for key, child in node.children.items():
            self.suffix_compress(child)
            if len(child.children) == 1:
                next_key, next_child = next(iter(child.children.items()))
                child.label += next_child.label
                child.children = next_child.children
                child.is_end_of_word = next_child.is_end_of_word
            elif len(child.children) == 0:
                keys_to_delete.append(key)
        for key in keys_to_delete:
            del node.children[key]

    def search_and_get_depth(self, key: str) -> int:
        # TODO
        if self.is_prefix_tree:
            return self.search_and_get_depth_prefix(key)
        else:
            return self.search_and_get_depth_suffix(key)

    def search_and_get_depth_prefix(self, key):
        node = self.prefix_root
        depth = 0
        i = 0
        while i < len(key):
            found = False
            for char, child in node.children.items():
                if key[i : i + len(child.label)] == child.label:
                    depth += 1
                    node = child
                    i += len(child.label)
                    found = True
                    break
            if not found:
                return -1
        return depth if node and node.is_end_of_word else -1

    def search_and_get_depth_suffix(self, key):
        node = self.suffix_root
        depth = 0
        for char in key:
            if char in node.children:
                node = node.children[char]
                depth += 1
            else:
                return -1
        return depth


# feel free to define new classes/methods in addition to the above
# fill in the definitions of each required member function (above),
# and any additional member functions you define
