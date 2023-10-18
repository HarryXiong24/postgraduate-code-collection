# Write a function, bool nextToken(String s), that recognizes string literals according to the following rules
# Begins with a double quote
# Ends with a double quote
# has any characters inside
# to embed a quote, include two quotes in a row, e.g.,
# “She said ““Hello”””      is      She said “Hello”
# “”””     is     ”
# “hello”     is     hello

def nextToken(s: str) -> bool:
    if not s:
        return False
    if s[0] != '"' or s[-1] != '"':
        return False

    s = s[1:len(s)-1]
    i = 0  # start after the first quote
    while i < len(s):  # iterate until the character before the last quote
        if s[i] == '"':
            # If we encounter a quote inside the string
            # the next character must also be a quote
            if i + 1 < len(s) and s[i + 1] == '"':
                i += 1  # Skip the next quote
            else:
                return False
        i += 1
    return True


# test
s1 = '"she said ""hello"""'
s2 = '""""'
s3 = '"hello"'
s4 = '"""hello"'
res1 = nextToken(s1)
res2 = nextToken(s2)
res3 = nextToken(s3)
res4 = nextToken(s4)
print(res1, res2, res3, res4)
