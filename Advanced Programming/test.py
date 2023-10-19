def compute_lps(pattern):
    """计算部分匹配表 (Longest Prefix Suffix)"""
    m = len(pattern)
    lps = [0] * m
    length = 0  # 当前最长的匹配长度
    i = 1

    while i < m:
        if pattern[i] == pattern[length]:
            length += 1
            lps[i] = length
            i += 1
        else:
            if length != 0:
                length = lps[length-1]
            else:
                lps[i] = 0
                i += 1
    print(lps)
    return lps


def KMP_search(text, pattern):
    """使用KMP算法查找子字符串的位置"""
    lps = compute_lps(pattern)
    i, j = 0, 0  # i为文本的指针，j为模式的指针

    while i < len(text):
        if text[i] == pattern[j]:
            i += 1
            j += 1
        if j == len(pattern):
            return i-j  # 模式全部匹配，返回开始匹配的位置
        elif i < len(text) and text[i] != pattern[j]:
            if j != 0:
                j = lps[j-1]
            else:
                i += 1
    return -1  # 没有匹配


# 测试
text = "AABABABABC"
pattern = "ABABC"
print(KMP_search(text, pattern))  # 期望输出: 6
