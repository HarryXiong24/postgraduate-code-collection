# 445. Add Two Numbers II

# You are given two non-empty linked lists representing two non-negative integers. The most significant digit comes first and each of their nodes contains a single digit. Add the two numbers and return the sum as a linked list.

# You may assume the two numbers do not contain any leading zero, except the number 0 itself.

# Example 1:
# Input: l1 = [7,2,4,3], l2 = [5,6,4]
# Output: [7,8,0,7]

# Example 2:
# Input: l1 = [2,4,3], l2 = [5,6,4]
# Output: [8,0,7]

# Example 3:
# Input: l1 = [0], l2 = [0]
# Output: [0]

# Definition for singly-linked list.
from typing import Optional


class ListNode:
    def __init__(self, val=0, next=None):
        self.val = val
        self.next = next


class Solution:
    def reverseLinkedList(self, head):

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

    def addTwoNumbers(self, l1: Optional[ListNode], l2: Optional[ListNode]) -> Optional[ListNode]:
        l1 = self.reverseLinkedList(l1)
        l2 = self.reverseLinkedList(l2)
        print(l1, l2)
        carry = 0
        newHead = ListNode(0, None)

        while l1 or l2:
            num1 = 0
            num2 = 0
            if l1:
                num1 = l1.val
            if l2:
                num2 = l2.val
            numSum = num1 + num2 + carry
            digit = numSum % 10
            carry = numSum // 10
            newNode = ListNode(digit, None)
            newNode.next = newHead.next
            newHead.next = newNode
            if l1:
                l1 = l1.next
            if l2:
                l2 = l2.next

        if carry > 0:
            newNode = ListNode(1, None)
            newNode.next = newHead.next
            newHead.next = newNode

        return newHead.next
