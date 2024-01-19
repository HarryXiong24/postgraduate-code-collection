def reverseLinkedList(head):

    if not head:
        return None

    def recursive(node):
        if not node.next:
            return node

        newHead = recursive(node.next)
        temp = node.next
        temp.next = node
        node.next = None

        return newHead

    return recursive(head)
